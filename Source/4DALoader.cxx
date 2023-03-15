/**
 * Copyright (C) 2022-2023 Hypertheory
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "4DALoader.h"

#include "External/Json/json.hpp"

#include "opencv2/opencv.hpp"

#include <fstream>

MultiViews T4DALoader::loadDataset(const std::string& path) {
	std::ifstream stream(path + "/calibration.json", std::fstream::in);
	
	if (stream.fail()) {
		std::cerr << "T4DALoader: Failed to load dataset\n";
		return MultiViews();
	}
	
	nlohmann::json camerasJson;
	stream >> camerasJson;
	
	MultiView multiview;
	
	for (auto& [name, cameraJson] : camerasJson.items()) {
		multiview.views.emplace_back(View());
		multiview.views.back().camera = std::make_shared<Camera>();
		auto& camera = multiview.views.back().camera;
		
		camera->name = name;
		
		if (cameraJson.contains("K")) {
			auto matrix = cameraJson.at("K");
			for (int i = 0; i < 3; ++i) {
				for (int j = 0; j < 3; ++j) {
					camera->K[i][j] = matrix[i * 3 + j];
				}
			}
		}
		
		if (cameraJson.contains("R")) {
			auto matrix = cameraJson.at("R");
			for (int i = 0; i < 3; ++i) {
				for (int j = 0; j < 3; ++j) {
					camera->R[i][j] = matrix[i * 3 + j];
				}
			}
		}
		
		if (cameraJson.contains("T")) {
			auto vector = cameraJson.at("T");
			camera->t = {vector[0], vector[1], vector[2]};
		}
		
		if (cameraJson.contains("RT")) {
			auto matrix = cameraJson.at("RT");
			for (int i = 0; i < 3; ++i) {
				for (int j = 0; j < 3; ++j) {
					camera->R[i][j] = matrix[i * 4 + j];
				}
			}
			camera->t = {matrix[3], matrix[7], matrix[11]};
		}
		
		cv::Size_<float> imgSize;
		cv::Mat_<float> distCoeffs;
		float rectifyAlpha = 0;
		
		if (cameraJson.contains("imgSize")) {
			auto vector = cameraJson.at("imgSize");
			camera->screenSize = {vector[0], vector[1]};
			imgSize = cv::Size_<float>(vector[0], vector[1]);
		}
		
		if (cameraJson.contains("distCoeff")) {
			auto vector = cameraJson.at("distCoeff");
			size_t size = vector.size();
			for (int i = 0; i < size; ++i) {
				distCoeffs(i) = vector[i];
			}
		} else {
			distCoeffs = cv::Mat_<float>::zeros(5, 1);
		}
		
		if (cameraJson.contains("rectifyAlpha")) {
			rectifyAlpha = cameraJson.at("rectifyAlpha");
		}
		
		cv::Mat matK = cv::Mat(3, 3, CV_32F, camera->K[0]);
		matK = cv::getOptimalNewCameraMatrix(matK, distCoeffs, imgSize, rectifyAlpha);
		std::copy(matK.begin<float>(), matK.end<float>(), camera->K[0]);
		
		camera->computePos();
		camera->computeRtKi();
	}
	
	stream.close();
	
	Ink::Vec2 screenSize = multiview.views[0].camera->screenSize;
	int viewNum = static_cast<int>(multiview.views.size());
	
	MultiViews multiviews;
	
	int skeletonType = 0;
	int frameNum = 0;
	int jointTypeNum = 0;
	int boneNum = 0;
	std::vector<int> boneA;
	std::vector<int> boneB;
	
	std::string detectionRoot = path + "/detection/";
	std::string detectionPath = detectionRoot + multiview.views[0].camera->name + ".txt";
	stream = std::ifstream(detectionPath, std::fstream::in);
	
	if (stream.fail()) {
		std::cerr << "T4DALoader: Failed to load detection data\n";
		return MultiViews();
	}
	
	stream >> skeletonType >> frameNum;
	stream.close();
	
	if (skeletonType == 4) {
		jointTypeNum = 25;
		boneNum = 26;
		boneA = {
			1, 9, 10, 8, 8, 12, 13, 1 , 2 , 3 , 2 , 1 , 5 ,
			6, 5, 1 , 0, 0, 15, 16, 14, 19, 14, 11, 22, 11,
		};
		boneB = {
			8, 10, 11, 9 , 12, 13, 14, 2 , 3 , 4 , 17, 5 , 6 ,
			7, 18, 0 , 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
		};
	} else {
		std::cerr << "T4DALoader: Unknown skeleton type\n";
		return MultiViews();
	}
	
	multiviews.resize(frameNum, multiview);
	
	int jointID = 0;
	
	for (int viewI = 0; viewI < viewNum; ++viewI) {
		std::string detectionPath = detectionRoot + multiview.views[viewI].camera->name + ".txt";
		stream = std::ifstream(detectionPath, std::fstream::in);
		
		if (stream.fail()) {
			std::cerr << "T4DALoader: Failed to load detection data\n";
			return MultiViews();
		}
		
		stream >> skeletonType >> frameNum;
		
		for (int frame = 0; frame < frameNum; ++frame) {
			auto& view = multiviews[frame].views[viewI];
			view.joints.resize(jointTypeNum);
			
			for (auto& jointChoices : view.joints) {
				int jointChoiceNum = 0;
				stream >> jointChoiceNum;
				jointChoices.resize(jointChoiceNum);
				
				for (int i = 0; i < 3; ++i) {
					for (int j = 0; j < jointChoiceNum; ++j) {
						float value = 0;
						stream >> value;
						if (i == 0) jointChoices[j].uv.x = value * (screenSize.x - 1.f);
						if (i == 1) jointChoices[j].uv.y = value * (screenSize.y - 1.f);
					}
				}
				
				for (int j = 0; j < jointChoiceNum; ++j) {
					jointChoices[j].ID = jointID++;
					jointChoices[j].ray = view.camera->computeRay(jointChoices[j].uv);
				}
			}
			
			for (int boneI = 0; boneI < boneNum; ++boneI) {
				for (auto& jointA : view.joints[boneA[boneI]]) {
					for (auto& jointB : view.joints[boneB[boneI]]) {
						float PAF = 0;
						stream >> PAF;
						view.setPAF(jointA, jointB, powf(PAF, 0.2f));
					}
				}
			}
		}
		
		stream.close();
	}
	
	return multiviews;
}

MultiPersonPoses T4DALoader::loadGroundTruth(const std::string& path) {
	std::ifstream stream(path, std::fstream::in);
	
	if (stream.fail()) {
		std::cerr << "T4DALoader: Failed to load ground truth\n";
		return MultiPersonPoses();
	}
	
	int typeNum = 0;
	int frameNum = 0;
	stream >> typeNum >> frameNum;
	
	MultiPersonPoses multiPersonPoses(frameNum);
	
	for (int frame = 0; frame < frameNum; ++frame) {
		int personNum = 0;
		stream >> personNum;
		
		multiPersonPoses[frame].resize(personNum);
		
		for (int personI = 0; personI < personNum; ++personI) {
			auto& personPose = multiPersonPoses[frame][personI];
			
			stream >> personPose.ID;
			personPose.hasJoint.resize(typeNum);
			personPose.jointPos.resize(typeNum);
			
			for (int i = 0; i < 4; ++i) {
				for (int type = 0; type < typeNum; ++type) {
					switch (i) {
						case 0:
							stream >> personPose.jointPos[type].x;
							break;
						case 1:
							stream >> personPose.jointPos[type].y;
							break;
						case 2:
							stream >> personPose.jointPos[type].z;
							break;
						default:
							float hasJointF = 0;
							stream >> hasJointF;
							personPose.hasJoint[type] = fabs(hasJointF) > 1E-4;
							break;
					}
				}
			}
		}
	}
	
	stream.close();
	
	return multiPersonPoses;
}
