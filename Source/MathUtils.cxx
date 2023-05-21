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

#include "MathUtils.h"

float MathUtils::computeRayDistance(const Ink::Ray& ray1, const Ink::Ray& ray2) {
	if (fabsf(ray1.direction.dot(ray2.direction)) < 0.0001f) {
		return (ray1.origin - ray2.origin).cross(ray1.direction).magnitude();
	}
	return fabsf((ray1.origin - ray2.origin).dot(ray1.direction.cross(ray2.direction).normalize()));
}

Ink::Vec3 MathUtils::multiRayIntersect(const Ink::Ray** rays, size_t size) {
	Ink::Mat3 A;
	Ink::Vec3 b;
	for (int i = 0; i < size; ++i) {
		auto& dx = rays[i]->direction.x;
		auto& dy = rays[i]->direction.y;
		auto& dz = rays[i]->direction.z;
		Ink::Mat3 N = {
			dx * dx - 1.f, dx * dy, dx * dz,
			dy * dx, dy * dy - 1.f, dy * dz,
			dz * dx, dz * dy, dz * dz - 1.f,
		};
		A += N;
		b += N * rays[i]->origin;
	}
	return Ink::inverse_3x3(A) * b;
}

Ink::Vec3 MathUtils::multiRayIntersect(const Ink::Ray** rays, float* confs, size_t size) {
	Ink::Mat3 A;
	Ink::Vec3 b;
	for (int i = 0; i < size; ++i) {
		auto& dx = rays[i]->direction.x;
		auto& dy = rays[i]->direction.y;
		auto& dz = rays[i]->direction.z;
		Ink::Mat3 N = {
			dx * dx - 1.f, dx * dy, dx * dz,
			dy * dx, dy * dy - 1.f, dy * dz,
			dz * dx, dz * dy, dz * dz - 1.f,
		};
		A += N * confs[i];
		b += N * rays[i]->origin * confs[i];
	}
	return Ink::inverse_3x3(A) * b;
}
