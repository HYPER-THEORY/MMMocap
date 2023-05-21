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

#include "Visualizer2D.h"

#include "opencv2/opencv.hpp"

#include <thread>

cv::Scalar COLORS[4] = {
	{255, 0, 0},
	{255, 0, 255},
	{255, 255, 0},
	{255, 255, 255},
};

void Visualizer2D::visualize(const MultiPersonPose& multiPersonPose,
							 const View& view, const std::string& imagePath) {
	auto camera = view.camera;
	auto image = cv::imread(imagePath);
	
	for (int type = 0; type < 25; ++type) {
		for (auto& choice : view.joints[type]) {
			cv::Point point;
			point.x = choice.uv.x;
			point.y = choice.uv.y;
			double green = choice.conf * 255;
			cv::circle(image, point, 1, {0, green, 255}, 5);
			cv::putText(image, std::to_string(type), point, cv::FONT_HERSHEY_SIMPLEX, 0.5, {0, green, 255});
		}
	}
	
	Ink::Mat3 KR = camera->K * camera->R;
	cv::Point points[25];
	for (auto& pose : multiPersonPose) {
		auto color = COLORS[pose.ID];
		
		if (pose.hasJoint.empty()) continue;
		size_t jointSize = pose.hasJoint.size();
		for (int i = 0; i < jointSize; ++i) {
			if (!pose.hasJoint[i]) continue;
			Ink::Vec3 screen_pos = KR * (pose.jointPos[i] - camera->pos);
			screen_pos /= screen_pos.z;
			points[i].x = screen_pos.x;
			points[i].y = screen_pos.y;
			cv::circle(image, points[i], 7, color, 1);
		}
		
		std::vector<int> boneA = {
			5, 2, 6, 3, 12, 9, 13, 10, 1, 1, 1, 1, 8, 8, 2, 5
		};
		std::vector<int> boneB = {
			6, 3, 7, 4, 13, 10, 14, 11, 0, 8, 2, 5, 9, 12, 17, 18
		};
		
		for (int i = 0; i < boneA.size(); ++i) {
			if (pose.hasJoint[boneA[i]] && pose.hasJoint[boneB[i]]) {
				cv::line(image, points[boneA[i]], points[boneB[i]], color, 1);
			}
		}
	}
	
	cv::imshow(imagePath, image);
	cv::waitKey();
	cv::destroyWindow(imagePath);
}
