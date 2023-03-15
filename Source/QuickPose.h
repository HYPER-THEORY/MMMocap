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

#pragma once

#include "Views.h"

class QCluster {
public:
	int mainView = 0;
	float score = 0;
	std::vector<std::vector<int> > choices;
	std::vector<Ink::Vec3> worldPos;
	
	explicit QCluster() = default;
	
	explicit QCluster(int viewNum, int typeNum);
	
	int getJoint(int view, int type) const;
	
	void setJoint(int view, int type, int choice);
};

class QuickPose {
public:
	int count = 0;
	
	explicit QuickPose() = default;
	
	void initBody25();
	
	MultiPersonPose compute(const MultiView& multiview);
	
	float getMaxBoneLength(int jointTypeA, int jointTypeB) const;
	
	void setMaxBoneLength(int jointTypeA, int jointTypeB, float length);
	
private:
	int viewNum = 0;
	
	int typeNum = 0;
	
	int rootJointType = 0;
	
	int maxPersonNum = 10;
	
	int maxClusterNum = 100000;
	
	int clusterNum = 0;
	
	std::vector<int> parents;
	
	std::vector<int> viewOrder;
	
	std::vector<int> jointOrder;
	
	std::vector<float> historyScores;
	
	std::vector<const Ink::Ray*> rays;
	
	std::unordered_map<unsigned int, float> maxBoneLengths;
	
	std::vector<QCluster> preservedClusters;
	
	bool computeWorldPos(const MultiView& multiview, QCluster& cluster, int jointType);
	
	void compute(const MultiView& multiview, QCluster& cluster, int viewI, int jointI);
	
	MultiPersonPose postProcessing(const MultiView& multiview);
};
