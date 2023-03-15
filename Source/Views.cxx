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

#include "Views.h"

#include "MathUtils.h"

constexpr unsigned long long I32 = 1ull << 32;

void Camera::computePos() {
	pos = -R.transpose() * t;
}

void Camera::computeRtKi() {
	RtKi = R.transpose() * Ink::inverse_3x3(K);
}

Ink::Ray Camera::computeRay(const Ink::Vec2& uv) const {
	return Ink::Ray(pos, Ink::Vec3(-RtKi * Ink::Vec3(uv, 1.f)).normalize());
}

float View::getPAF(const Joint& joint1, const Joint& joint2) const {
	if (PAFs.count(joint1.ID + joint2.ID * I32) != 0) {
		return PAFs.at(joint1.ID + joint2.ID * I32);
	}
	return PAFs.at(joint1.ID * I32 + joint2.ID);
}

void View::setPAF(const Joint& joint1, const Joint& joint2, float value) {
	PAFs.insert_or_assign(joint1.ID + joint2.ID * I32, value);
}

void MultiView::computeEpipolar(float maxDistance) {
	int viewNum = static_cast<int>(views.size());
	int jointTypeNum = static_cast<int>(views[0].joints.size());
	for (int viewIA = 0; viewIA < viewNum; ++viewIA) {
		for (int viewIB = viewIA + 1; viewIB < viewNum; ++viewIB) {
			for (int jointType = 0; jointType < jointTypeNum; ++jointType) {
				for (auto& jointA : views[viewIA].joints[jointType]) {
					for (auto& jointB : views[viewIB].joints[jointType]) {
						float distance = MathUtils::computeRayDistance(jointA.ray, jointB.ray);
						setEpipolar(jointA, jointB, 1.f - distance / maxDistance);
					}
				}
			}
		}
	}
}

float MultiView::getEpipolar(const Joint& joint1, const Joint& joint2) const {
	if (epipolars.count(joint1.ID + joint2.ID * I32) != 0) {
		return epipolars.at(joint1.ID + joint2.ID * I32);
	}
	return epipolars.at(joint1.ID * I32 + joint2.ID);
}

void MultiView::setEpipolar(const Joint& joint1, const Joint& joint2, float value) {
	epipolars.insert_or_assign(joint1.ID + joint2.ID * I32, value);
}
