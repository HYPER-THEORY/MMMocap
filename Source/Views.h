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

#include "ink/Ink.h"

struct Joint {
	int ID = 0;
	Ink::Ray ray;
	Ink::Vec2 uv;
	float conf = 0;
};

class Camera {
public:
	std::string name;
	Ink::Vec2 screenSize;
	Ink::Vec3 t;
	Ink::Vec3 pos;
	Ink::Mat3 K;
	Ink::Mat3 R;
	Ink::Mat3 RtKi;
	
	explicit Camera() = default;
	
	void computePos();
	
	void computeRtKi();
	
	Ink::Ray computeRay(const Ink::Vec2& uv) const;
};

class View {
public:
	std::shared_ptr<Camera> camera;
	
	std::vector<std::vector<Joint> > joints;
	
	explicit View() = default;
	
	float getPAF(const Joint& joint1, const Joint& joint2) const;
	
	void setPAF(const Joint& joint1, const Joint& joint2, float value);
	
private:
	std::unordered_map<unsigned long long, float> PAFs;
};

class MultiView {
public:
	std::vector<View> views;
	
	explicit MultiView() = default;
	
	void computeEpipolar(float maxDistance);
	
	float getEpipolar(const Joint& joint1, const Joint& joint2) const;
	
	void setEpipolar(const Joint& joint1, const Joint& joint2, float value);
	
private:
	std::unordered_map<unsigned long long, float> epipolars;
};

using MultiViews = std::vector<MultiView>;

class Pose {
public:
	int ID = 0;
	
	std::vector<bool> hasJoint;
	
	std::vector<Ink::Vec3> jointPos;
	
	explicit Pose() = default;
};

using MultiPersonPose = std::vector<Pose>;
 
using MultiPersonPoses = std::vector<MultiPersonPose>;
