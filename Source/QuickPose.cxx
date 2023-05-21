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

#include "QuickPose.h"

#include "MathUtils.h"

#include <iostream>

#include <opencv2/opencv.hpp>

//void Triangulate_With_Conf(const float* &pointsOnEachCamera, const cv::Mat** &cameraMatrices, size_t size,
//						   cv::Mat &reconstructedPoint) {
//	// cameraMatrices store the P matrix of each camera. Please see the top for its calculation
//
//	cv::Mat A = cv::Mat::zeros(static_cast<int>(size) * 2, 4, CV_32F);
//
//	// Normalize the weights
//	// Note that z coordinate of each 2d point is its contribution weight for triangulation
//	float totWeight = 0.0;
//	for (int i = 0; i < size; ++i){
//		assert(pointsOnEachCamera[i]->z >= 0);
//		totWeight += pointsOnEachCamera[i]->z;
//	}
//	assert(totWeight > 0);
//
//	cv::Mat x = cameraMatrices[0]->row(0);
//
//	for (int i = 0; i < size; ++i) {
//		A.row(i * 2) = (pointsOnEachCamera[i]->x * cameraMatrices[i]->row(2) - cameraMatrices[i]->row(0)) *
//					   (pointsOnEachCamera[i]->z / totWeight); // confidence
//		A.row(i * 2 + 1) = (pointsOnEachCamera[i]->y * cameraMatrices[i]->row(2) - cameraMatrices[i]->row(1)) *
//						   (pointsOnEachCamera[i]->z / totWeight); // confidence
//	}
//	// Solve x for Ax = 0 --> SVD on A
//	cv::SVD::solveZ(A, reconstructedPoint);
//	reconstructedPoint /= reconstructedPoint.at<float>(3);
//}


constexpr float EPS = 0.0001f;
constexpr int NO_CHOICE = -1;

constexpr unsigned int I16 = 1 << 16;

QCluster::QCluster(int viewNum, int typeNum) {
	score = 0;
	choices.resize(viewNum);
	for (auto& choicesPerView : choices) {
		choicesPerView.resize(typeNum, NO_CHOICE);
	}
	worldPos.resize(typeNum);
}

int QCluster::getJoint(int view, int type) const {
	return choices[view][type];
}

void QCluster::setJoint(int view, int type, int choice) {
	choices[view][type] = choice;
}

void QuickPose::initBody25() {
	rootJointType = 8;
	parents = {
		1, 8, 1, 2, 3, 1, 5, 6, -1, 8, 9, 10, 8, 12, 13, 0, 0, 15, 16, 14, 19, 14, 11, 22, 11,
	};
	/* New Parent */
	parents = {
		1, 8, 1, 2, 3, 1, 5, 6, -1, 8, 9, 10, 8, 12, 13, 0, 0, 2, 5, 14, 19, 14, 11, 22, 11,
	};
	historyScores.resize(25);
}

MultiPersonPose QuickPose::compute(const MultiView& multiview) {
	viewNum = static_cast<int>(multiview.views.size());
	typeNum = static_cast<int>(multiview.views[0].joints.size());
	
	rays.resize(viewNum);
	confs.resize(viewNum);
	
	clusterNum = 0;
	preservedClusters.resize(maxClusterNum);
	
	QCluster cluster(viewNum, typeNum);
	
	std::vector<std::vector<int> > viewOrders = {
		{0, 1, 2, 3, 4},
		{1, 2, 3, 4, 0},
		{2, 3, 4, 0, 1},
		{3, 4, 0, 1, 2},
		{4, 0, 1, 2, 3},
	};
	
	std::vector<std::vector<int> > jointOrders = {
//		{8, 1, 2, 3, 4, 5, 6, 7, 0},
		{8, 1, 2, 3, 4},
		{8, 1, 5, 6, 7},
		{8, 1, 0},
		{8, 9, 10, 11},
		{8, 12, 13, 14},
		{8, 1, 2, 17},
		{8, 1, 5, 18},
	};
	
	for (auto& curViewOrder : viewOrders) {
		viewOrder = curViewOrder;
		cluster.mainView = viewOrder[0];
		for (auto& curJointOrder : jointOrders) {
			jointOrder = curJointOrder;
			compute(multiview, cluster, 0, 0);
		}
	}
	
	return postProcessing(multiview);
}

bool QuickPose::computeWorldPos(const MultiView& multiview, QCluster& cluster, int jointType) {
	int rayNum = 0;
	for (int viewI = 0; viewI < viewNum; ++viewI) {
		int view = viewOrder[viewI];
		int choice = cluster.getJoint(view, jointType);
		if (choice == NO_CHOICE) continue;
		rays[rayNum] = &multiview.views[view].joints[jointType][choice].ray;
		confs[rayNum] = multiview.views[view].joints[jointType][choice].conf;
		confs[rayNum] *= confs[rayNum];
		++rayNum;
	}
	
	if (rayNum < 2) return false;
	
	cluster.worldPos[jointType] = MathUtils::multiRayIntersect(rays.data(), rayNum);
	return true;
}

void QuickPose::compute(const MultiView& multiview, QCluster& cluster, int viewI, int jointI) {
	if (jointI == jointOrder.size()) {
		int validJointNum = 0;
		
		int jointOrderNum = static_cast<int>(jointOrder.size());
		for (int jointI = 0; jointI < jointOrderNum; ++jointI) {
			validJointNum += cluster.getJoint(viewOrder[0], jointOrder[jointI]) != NO_CHOICE;
		}
		
//		if (validJointNum > jointOrderNum / 4 * 3) {
			preservedClusters[clusterNum++] = cluster;
			++count;
//		}
		
		return; /* Finish */
	}
	
	int view = viewOrder[viewI];
	int jointType = jointOrder[jointI];
	
	bool isNotRoot = jointI != 0;
	
	int parentType = parents[jointType];
	int parentChoice = 0;
	if (isNotRoot) parentChoice = cluster.getJoint(view, parentType);
	
	bool successfulShift = false;
	
	/* 1. Parent must exist (have choice) */
	if (!isNotRoot || parentChoice != NO_CHOICE) {
		
		int choiceNum = static_cast<int>(multiview.views[view].joints[jointType].size());
		
		for (int choice = 0; choice < choiceNum; ++choice) {
			float scorePAF = 0.f;
			if (isNotRoot) {
				auto& curJoint = multiview.views[view].joints[jointType][choice];
				auto& parentJoint = multiview.views[view].joints[parentType][parentChoice];
				scorePAF = multiview.views[view].getPAF(parentJoint, curJoint);
				
				/* 2. PAF value must be greater than 0 */
				if (scorePAF < EPS) continue;
			}
			
			bool isValidShift = true;
			float scoreEpi = 0.f;
			for (int prevViewI = 0; prevViewI < viewI; ++prevViewI) {
				int prevView = viewOrder[prevViewI];
				int prevChoice = cluster.getJoint(prevView, jointType);
				if (prevChoice != NO_CHOICE) {
					auto& curJoint = multiview.views[view].joints[jointType][choice];
					auto& prevJoint = multiview.views[prevView].joints[jointType][prevChoice];
					float epipolar = multiview.getEpipolar(prevJoint, curJoint);
					if (epipolar < EPS) {
						isValidShift = false;
						break;
					}
					scoreEpi += epipolar;
				}
			}
			
			/* 3. All epipolars must be greater than 0 */
			if (!isValidShift) continue;
			
			float originalScore = cluster.score;
			cluster.setJoint(view, jointType, choice);
			cluster.score += scorePAF + scoreEpi;
			
			if (viewI == viewNum - 1) {
				
				/* Shift to next joint */
				
				int view0Choice = cluster.getJoint(viewOrder[0], jointType);
				
				computeWorldPos(multiview, cluster, jointType);
				
				/* RayNum must be 2 or more, no need to check */
				
				if (isNotRoot) {
					float boneLength = cluster.worldPos[parentType].distance(cluster.worldPos[jointType]);
					
					/* 4. Bone length must satisfy the constraints */
					if (boneLength > getMaxBoneLength(jointType, parentType)) {
						cluster.setJoint(viewOrder[0], jointType, NO_CHOICE);
						cluster.score = historyScores[jointI - 1];
					}
				}
				
				historyScores[jointI] = cluster.score;
				compute(multiview, cluster, 0, jointI + 1);
				
				cluster.setJoint(viewOrder[0], jointType, view0Choice);
				
			} else {
				/* Shift to the next view */
				compute(multiview, cluster, viewI + 1, jointI);
			}
			
			cluster.setJoint(view, jointType, NO_CHOICE);
			cluster.score = originalScore;
			
			successfulShift = true;
		}
	}
	
	/**
	 * Skip strategy:
	 * 1. Do not skip at the main view (view 0) if there is a successful shift.
	 * 2. Always skip at other views (view 1, 2...) no matter shift is successful or not.
	 */
	if (viewI == viewNum - 1) {
		
		/* Shift to the next joint */
		
		float originalScore = cluster.score;
		
		int view0Choice = cluster.getJoint(viewOrder[0], jointType);
		
		bool moreThanTwoRays = computeWorldPos(multiview, cluster, jointType);
		
		if (!moreThanTwoRays) {
			cluster.setJoint(viewOrder[0], jointType, NO_CHOICE);
			cluster.score = jointI == 0 ? 0.f : historyScores[jointI - 1];
		} else if (isNotRoot) {
			float boneLength = cluster.worldPos[parentType].distance(cluster.worldPos[jointType]);
			
			/* 4. Bone length must satisfy the constraints */
			if (boneLength > getMaxBoneLength(jointType, parentType)) {
				cluster.setJoint(viewOrder[0], jointType, NO_CHOICE);
				cluster.score = historyScores[jointI - 1];
			}
		}
		
		historyScores[jointI] = cluster.score;
		compute(multiview, cluster, 0, jointI + 1);
		
		cluster.setJoint(viewOrder[0], jointType, view0Choice);
		cluster.score = originalScore;
		
	} else if (viewI != 0) {
//		if (successfulShift) return; /* Wrong */
		
		/* Shift to the next view */
		compute(multiview, cluster, viewI + 1, jointI);
		
	} else if (!successfulShift) {
		
		historyScores[jointI] = cluster.score;
		compute(multiview, cluster, 0, jointI + 1);
	}
}

MultiPersonPose QuickPose::postProcessing(const MultiView& multiview) {
	MultiPersonPose multiPersonPose;
	
	/* view, joint, choice => person */
	std::vector<std::vector<std::vector<int> > > VJCPersons(viewNum);
	for (int view = 0; view < viewNum; ++view) {
		VJCPersons[view].resize(typeNum);
		for (int type = 0; type < typeNum; ++type) {
			VJCPersons[view][type].resize(multiview.views[view].joints[type].size(), -1);
		}
	}
	
	/* view, joint, person => choice */
	std::vector<std::vector<std::vector<int> > > VJPChoices(viewNum);
	for (int view = 0; view < viewNum; ++view) {
		VJPChoices[view].resize(typeNum);
		for (int type = 0; type < typeNum; ++type) {
			VJPChoices[view][type].resize(maxPersonNum, NO_CHOICE);
		}
	}
	
	std::sort(preservedClusters.data(), preservedClusters.data() + clusterNum,
			  [](const QCluster& cluster1, const QCluster& cluster2) -> bool {
		return cluster1.score > cluster2.score;
	});
	
	for (int clusterI = 0; clusterI < clusterNum; ++clusterI) {
		auto& cluster = preservedClusters[clusterI];
		int mainView = cluster.mainView;
		
		bool isConflicting = false;
		bool isContributing = false;
		int personID = -1;
		for (int type = 0; type < typeNum; ++type) {
			if (cluster.getJoint(mainView, type) == NO_CHOICE) {
				continue;
			}
			for (int view = 0; view < viewNum; ++view) {
				int choice = cluster.getJoint(view, type);
				if (choice == NO_CHOICE) continue;
				int VJCPersonID = VJCPersons[view][type][choice];
				if (VJCPersonID == -1) {
					isContributing = true;
				} else if (personID == -1) {
					personID = VJCPersonID;
				} else if (personID != VJCPersonID) {
					isConflicting = true;
					break;
				}
			}
			if (isConflicting) break;
		}
		
		if (isConflicting || !isContributing) continue;
		
		if (personID == -1) {
			personID = static_cast<int>(multiPersonPose.size());
			Pose pose;
			pose.ID = personID;
			pose.hasJoint.resize(typeNum);
			pose.jointPos.resize(typeNum);
			multiPersonPose.emplace_back(pose);
		} else {
			for (int type = 0; type < typeNum; ++type) {
				if (cluster.getJoint(mainView, type) == NO_CHOICE) {
					continue;
				}
				for (int view = 0; view < viewNum; ++view) {
					int choice = cluster.getJoint(view, type);
					if (choice == NO_CHOICE) continue;
					int VJPChoice = VJPChoices[view][type][personID];
					if (VJPChoice != NO_CHOICE && choice != VJPChoice) {
						isConflicting = true;
						break;
					}
				}
				if (isConflicting) break;
			}
			if (isConflicting) continue;
		}
		
		auto& curPose = multiPersonPose[personID];
		
		for (int type = 0; type < typeNum; ++type) {
			for (int view = 0; view < viewNum; ++view) {
				if (cluster.getJoint(mainView, type) == NO_CHOICE) {
					continue;
				}
				int choice = cluster.getJoint(view, type);
				if (choice == NO_CHOICE) continue;
				VJCPersons[view][type][choice] = personID;
				VJPChoices[view][type][personID] = choice;
				if (!curPose.hasJoint[type]) {
					curPose.hasJoint[type] = true;
					curPose.jointPos[type] = cluster.worldPos[type];
				}
			}
		}
	}
	
//	for (auto& pose : multiPersonPose) {
//		if (pose.hasJoint.empty()) continue;
//		for (int type = 0; type < typeNum; ++type) {
//			if (pose.hasJoint[type]) {
//				int rayNum = 0;
//				for (int view = 0; view < viewNum; ++view) {
//					int choice = VJPChoices[view][type][pose.ID];
//					if (choice == NO_CHOICE) continue;
//					rays[rayNum++] = &multiview.views[view].joints[type][choice].ray;
//				}
//
//				if (rayNum < 2) {
//					std::cerr << "Error: rayNum should be greater than 2\n";
//				}
//
//				pose.jointPos[type] = MathUtils::multiRayIntersect(rays.data(), rayNum);
//			}
//		}
//	}
	
	return multiPersonPose;
}

float QuickPose::getMaxBoneLength(int jointTypeA, int jointTypeB) const {
	if (maxBoneLengths.count(jointTypeA + jointTypeB * I16) != 0) {
		return maxBoneLengths.at(jointTypeA + jointTypeB * I16);
	}
	if (maxBoneLengths.count(jointTypeA * I16 + jointTypeB) != 0) {
		return maxBoneLengths.at(jointTypeA * I16 + jointTypeB);
	}
	return std::numeric_limits<float>::max();
}

void QuickPose::setMaxBoneLength(int jointTypeA, int jointTypeB, float length) {
	maxBoneLengths.insert_or_assign(jointTypeA + jointTypeB * I16, length);
}
