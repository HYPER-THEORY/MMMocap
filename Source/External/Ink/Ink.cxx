/**
 * Copyright (C) 2021-2023 Hypertheory
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

#include "Ink.h"

#define FMT_HEADER_ONLY
#include "fmt/format.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include "opengl/glad.h"

#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <array>
#include <chrono>
#include <iostream>
#include <sstream>
#include <memory>
#include <fstream>
#include <limits>
#include <cstring>

/* -------------------------------------------------------------------------- */
/* ---- ink/core/Error.cxx -------------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

std::string Error::get() {
	if (message.empty()) return message;
	return message + '\n';
}

void Error::set(const std::string& m) {
	message = m;
	if (callback) std::invoke(callback, message);
}

void Error::set(const std::string& l, const std::string& m) {
	message = l + " Error: " + m;
	if (callback) std::invoke(callback, message);
}

void Error::clear() {
	message.clear();
}

void Error::set_callback(const ErrorCallback& f) {
	callback = f;
}

std::string Error::message;

Error::ErrorCallback Error::callback;

}

/* -------------------------------------------------------------------------- */
/* ---- ink/core/File.cxx --------------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

std::string File::read(const std::string& p) {
	std::string content;
	std::ifstream stream(p, std::fstream::in);
	if (stream.fail()) {
		Error::set("File", "Failed to read from file");
	}
	stream.ignore(std::numeric_limits<std::streamsize>::max());
	std::streamsize length = stream.gcount();
	content.resize(length);
	stream.seekg(0, stream.beg);
	stream.read(content.data(), length);
	stream.close();
	return content;
}

void File::write(const std::string& p, const std::string& c) {
	std::ofstream stream(p, std::fstream::out);
	stream.write(c.data(), c.size());
	if (stream.fail()) {
		Error::set("File", "Failed to write to file");
	}
	stream.close();
}

void File::write(const std::string& p, const char* c) {
	std::ofstream stream(p, std::fstream::out);
	stream.write(c, std::strlen(c));
	if (stream.fail()) {
		Error::set("File", "Failed to write to file");
	}
	stream.close();
}

void File::append(const std::string& p, const std::string& c) {
	std::ofstream stream(p, std::fstream::out | std::fstream::app);
	stream.write(c.data(), c.size());
	if (stream.fail()) {
		Error::set("File", "Failed to append to file");
	}
	stream.close();
}

void File::append(const std::string& p, const char* c) {
	std::ofstream stream(p, std::fstream::out | std::fstream::app);
	stream.write(c, std::strlen(c));
	if (stream.fail()) {
		Error::set("File", "Failed to append to file");
	}
	stream.close();
}

}

/* -------------------------------------------------------------------------- */
/* ---- ink/core/Date.cxx --------------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

Date::Date(int year, int month, int day, int h, int m, int s, int ms) :
year(year), month(month), day(day), hours(h), minutes(m), seconds(s), milliseconds(ms) {}

std::string Date::format() const {
	return fmt::format("{}-{:0>2}-{:0>2} {:0>2}:{:0>2}:{:0>2}",
					   year, month + 1, day, hours, minutes, seconds);
}

long long Date::get_time() {
	auto now = std::chrono::system_clock::now().time_since_epoch();
	return std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
}

Date Date::get_local(long long t) {
	if (t == -1) t = get_time();
	int tm_ms = t % 1000;
	time_t time_s = t / 1000;
	std::tm local = *std::localtime(&time_s);
	return Date(local.tm_year + 1900, local.tm_mon, local.tm_mday,
				local.tm_hour, local.tm_min, local.tm_sec, tm_ms);
}

Date Date::get_utc(long long t) {
	if (t == -1) t = get_time();
	int tm_ms = t % 1000;
	time_t time_s = t / 1000;
	std::tm local = *std::gmtime(&time_s);
	return Date(local.tm_year + 1900, local.tm_mon, local.tm_mday,
				local.tm_hour, local.tm_min, local.tm_sec, tm_ms);
}

}

/* -------------------------------------------------------------------------- */
/* ---- ink/math/Random.cxx ------------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

double Random::random() {
	return std::generate_canonical<double, 1>(generator);
}

float Random::random_f() {
	return std::generate_canonical<float, 1>(generator);
}

void Random::set_seed(unsigned int s) {
	generator.seed(s);
}

std::mt19937 Random::generator;

}

/* -------------------------------------------------------------------------- */
/* ---- ink/math/Vector2.cxx ------------------------------------------------ */
/* -------------------------------------------------------------------------- */

namespace Ink {

FVec2::FVec2(float x) : x(x), y(x) {}

FVec2::FVec2(float x, float y) : x(x), y(y) {}

FVec2 FVec2::operator-() const {
	return {-x, -y};
}

bool FVec2::operator==(const FVec2& v) const {
	return x == v.x && y == v.y;
}

void FVec2::operator+=(float v) {
	x += v;
	y += v;
}

void FVec2::operator+=(const FVec2& v) {
	x += v.x;
	y += v.y;
}

void FVec2::operator-=(float v) {
	x -= v;
	y -= v;
}

void FVec2::operator-=(const FVec2& v) {
	x -= v.x;
	y -= v.y;
}

void FVec2::operator*=(float v) {
	x *= v;
	y *= v;
}

void FVec2::operator*=(const FVec2& v) {
	x *= v.x;
	y *= v.y;
}

void FVec2::operator/=(float v) {
	x /= v;
	y /= v;
}

void FVec2::operator/=(const FVec2& v) {
	x /= v.x;
	y /= v.y;
}

float FVec2::dot(const FVec2& v) const {
	return x * v.x + y * v.y;
}

float FVec2::cross(const FVec2& v) const {
	return x * v.y - y * v.x;
}

float FVec2::magnitude() const {
	return sqrtf(x * x + y * y);
}

float FVec2::distance(const FVec2& v) const {
	return sqrtf((x - v.x) * (x - v.x) + (y - v.y) * (y - v.y));
}

FVec2 FVec2::normalize() const {
	float l = sqrtf(x * x + y * y);
	return {x / l, y / l};
}

FVec2 FVec2::rotate(float a) const {
	return {x * cosf(a) - y * sinf(a), x * sinf(a) + y * cosf(a)};
}

std::string FVec2::to_string(int p) const {
	static std::stringstream stream;
	stream.setf(std::ios::fixed, std::ios::floatfield);
	stream.precision(p);
	stream.str(std::string());
	stream << "(" << x << ", " << y << ")";
	return stream.str();
}

FVec2 FVec2::random() {
	float angle = Random::random_f() * PI * 2;
	return {cosf(angle), sinf(angle)};
}

FVec2 operator+(const FVec2& v1, float v2) {
	return {v1.x + v2, v1.y + v2};
}

FVec2 operator+(float v1, const FVec2& v2) {
	return {v1 + v2.x, v1 + v2.y};
}

FVec2 operator+(const FVec2& v1, const FVec2& v2) {
	return {v1.x + v2.x, v1.y + v2.y};
}

FVec2 operator-(const FVec2& v1, float v2) {
	return {v1.x - v2, v1.y - v2};
}

FVec2 operator-(float v1, const FVec2& v2) {
	return {v1 - v2.x, v1 - v2.y};
}

FVec2 operator-(const FVec2& v1, const FVec2& v2) {
	return {v1.x - v2.x, v1.y - v2.y};
}

FVec2 operator*(const FVec2& v1, float v2) {
	return {v1.x * v2, v1.y * v2};
}

FVec2 operator*(float v1, const FVec2& v2) {
	return {v1 * v2.x, v1 * v2.y};
}

FVec2 operator*(const FVec2& v1, const FVec2& v2) {
	return {v1.x * v2.x, v1.y * v2.y};
}

FVec2 operator/(const FVec2& v1, float v2) {
	return {v1.x / v2, v1.y / v2};
}

FVec2 operator/(float v1, const FVec2& v2) {
	return {v1 / v2.x, v1 / v2.y};
}

FVec2 operator/(const FVec2& v1, const FVec2& v2) {
	return {v1.x / v2.x, v1.y / v2.y};
}

DVec2::DVec2(double x) : x(x), y(x) {}

DVec2::DVec2(double x, double y) : x(x), y(y) {}

DVec2 DVec2::operator-() const {
	return {-x, -y};
}

bool DVec2::operator==(const DVec2& v) const {
	return x == v.x && y == v.y;
}

void DVec2::operator+=(double v) {
	x += v;
	y += v;
}

void DVec2::operator+=(const DVec2& v) {
	x += v.x;
	y += v.y;
}

void DVec2::operator-=(double v) {
	x -= v;
	y -= v;
}

void DVec2::operator-=(const DVec2& v) {
	x -= v.x;
	y -= v.y;
}

void DVec2::operator*=(double v) {
	x *= v;
	y *= v;
}

void DVec2::operator*=(const DVec2& v) {
	x *= v.x;
	y *= v.y;
}

void DVec2::operator/=(double v) {
	x /= v;
	y /= v;
}

void DVec2::operator/=(const DVec2& v) {
	x /= v.x;
	y /= v.y;
}

double DVec2::dot(const DVec2& v) const {
	return x * v.x + y * v.y;
}

double DVec2::cross(const DVec2& v) const {
	return x * v.y - y * v.x;
}

double DVec2::magnitude() const {
	return sqrt(x * x + y * y);
}

double DVec2::distance(const DVec2& v) const {
	return sqrt((x - v.x) * (x - v.x) + (y - v.y) * (y - v.y));
}

DVec2 DVec2::normalize() const {
	double l = sqrt(x * x + y * y);
	return {x / l, y / l};
}

DVec2 DVec2::rotate(double a) const {
	return {x * cos(a) - y * sin(a), x * sin(a) + y * cos(a)};
}

std::string DVec2::to_string(int p) const {
	static std::stringstream stream;
	stream.setf(std::ios::fixed, std::ios::floatfield);
	stream.precision(p);
	stream.str(std::string());
	stream << "(" << x << ", " << y << ")";
	return stream.str();
}

DVec2 DVec2::random() {
	double angle = Random::random() * PI * 2;
	return {cos(angle), sin(angle)};
}

DVec2 operator+(const DVec2& v1, double v2) {
	return {v1.x + v2, v1.y + v2};
}

DVec2 operator+(double v1, const DVec2& v2) {
	return {v1 + v2.x, v1 + v2.y};
}

DVec2 operator+(const DVec2& v1, const DVec2& v2) {
	return {v1.x + v2.x, v1.y + v2.y};
}

DVec2 operator-(const DVec2& v1, double v2) {
	return {v1.x - v2, v1.y - v2};
}

DVec2 operator-(double v1, const DVec2& v2) {
	return {v1 - v2.x, v1 - v2.y};
}

DVec2 operator-(const DVec2& v1, const DVec2& v2) {
	return {v1.x - v2.x, v1.y - v2.y};
}

DVec2 operator*(const DVec2& v1, double v2) {
	return {v1.x * v2, v1.y * v2};
}

DVec2 operator*(double v1, const DVec2& v2) {
	return {v1 * v2.x, v1 * v2.y};
}

DVec2 operator*(const DVec2& v1, const DVec2& v2) {
	return {v1.x * v2.x, v1.y * v2.y};
}

DVec2 operator/(const DVec2& v1, double v2) {
	return {v1.x / v2, v1.y / v2};
}

DVec2 operator/(double v1, const DVec2& v2) {
	return {v1 / v2.x, v1 / v2.y};
}

DVec2 operator/(const DVec2& v1, const DVec2& v2) {
	return {v1.x / v2.x, v1.y / v2.y};
}

}

/* -------------------------------------------------------------------------- */
/* ---- ink/math/Vector3.cxx ------------------------------------------------ */
/* -------------------------------------------------------------------------- */

namespace Ink {

FVec3::FVec3(float x) : x(x), y(x), z(x) {}

FVec3::FVec3(float x, float y, float z) : x(x), y(y), z(z) {}

FVec3::FVec3(const FVec2& v, float z) : x(v.x), y(v.y), z(z) {}

FVec3::FVec3(float x, const FVec2& v) : x(x), y(v.x), z(v.y) {}

FVec3 FVec3::operator-() const {
	return {-x, -y, -z};
}

bool FVec3::operator==(const FVec3& v) const {
	return x == v.x && y == v.y && z == v.z;
}

void FVec3::operator+=(float v) {
	x += v;
	y += v;
	z += v;
}

void FVec3::operator+=(const FVec3& v) {
	x += v.x;
	y += v.y;
	z += v.z;
}

void FVec3::operator-=(float v) {
	x -= v;
	y -= v;
	z -= v;
}

void FVec3::operator-=(const FVec3& v) {
	x -= v.x;
	y -= v.y;
	z -= v.z;
}

void FVec3::operator*=(float v) {
	x *= v;
	y *= v;
	z *= v;
}

void FVec3::operator*=(const FVec3& v) {
	x *= v.x;
	y *= v.y;
	z *= v.z;
}

void FVec3::operator/=(float v) {
	x /= v;
	y /= v;
	z /= v;
}

void FVec3::operator/=(const FVec3& v) {
	x /= v.x;
	y /= v.y;
	z /= v.z;
}

float FVec3::dot(const FVec3& v) const {
	return x * v.x + y * v.y + z * v.z;
}

FVec3 FVec3::cross(const FVec3& v) const {
	return {y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x};
}

float FVec3::magnitude() const {
	return sqrtf(x * x + y * y + z * z);
}

float FVec3::distance(const FVec3& v) const {
	return sqrtf((x - v.x) * (x - v.x) + (y - v.y) * (y - v.y) + (z - v.z) * (z - v.z));
}

FVec3 FVec3::normalize() const {
	float l = sqrtf(x * x + y * y + z * z);
	return {x / l, y / l, z / l};
}

FVec3 FVec3::rotate(const FVec3& v, float a) const {
	return {(cosf(a) + (1 - cosf(a)) * v.x * v.x) * x +
		((1 - cosf(a)) * v.x * v.y - sinf(a) * v.z) * y +
		((1 - cosf(a)) * v.x * v.z + sinf(a) * v.y) * z,
		((1 - cosf(a)) * v.x * v.y + sinf(a) * v.z) * x +
		(cosf(a) + (1 - cosf(a)) * v.y * v.y) * y +
		((1 - cosf(a)) * v.y * v.z - sinf(a) * v.x) * z,
		((1 - cosf(a)) * v.x * v.z - sinf(a) * v.y) * x +
		((1 - cosf(a)) * v.y * v.z + sinf(a) * v.x) * y +
		(cosf(a) + (1 - cosf(a)) * v.z * v.z) * z};
}

std::string FVec3::to_string(int p) const {
	static std::stringstream stream;
	stream.setf(std::ios::fixed, std::ios::floatfield);
	stream.precision(p);
	stream.str(std::string());
	stream << "(" << x << ", " << y << ", " << z << ")";
	return stream.str();
}

FVec3 FVec3::random() {
	float angle1 = Random::random_f() * PI * 2;
	float angle2 = Random::random_f() * PI * 2;
	return {cosf(angle1) * cosf(angle2), sinf(angle2), sinf(angle1) * cosf(angle2)};
}

FVec3 operator+(const FVec3& v1, float v2) {
	return {v1.x + v2, v1.y + v2, v1.z + v2};
}

FVec3 operator+(float v1, const FVec3& v2) {
	return {v1 + v2.x, v1 + v2.y, v1 + v2.z};
}

FVec3 operator+(const FVec3& v1, const FVec3& v2) {
	return {v1.x + v2.x, v1.y + v2.y, v1.z + v2.z};
}

FVec3 operator-(const FVec3& v1, float v2) {
	return {v1.x - v2, v1.y - v2, v1.z - v2};
}

FVec3 operator-(float v1, const FVec3& v2) {
	return {v1 - v2.x, v1 - v2.y, v1 - v2.z};
}

FVec3 operator-(const FVec3& v1, const FVec3& v2) {
	return {v1.x - v2.x, v1.y - v2.y, v1.z - v2.z};
}

FVec3 operator*(const FVec3& v1, float v2) {
	return {v1.x * v2, v1.y * v2, v1.z * v2};
}

FVec3 operator*(float v1, const FVec3& v2) {
	return {v1 * v2.x, v1 * v2.y, v1 * v2.z};
}

FVec3 operator*(const FVec3& v1, const FVec3& v2) {
	return {v1.x * v2.x, v1.y * v2.y, v1.z * v2.z};
}

FVec3 operator/(const FVec3& v1, float v2) {
	return {v1.x / v2, v1.y / v2, v1.z / v2};
}

FVec3 operator/(float v1, const FVec3& v2) {
	return {v1 / v2.x, v1 / v2.y, v1 / v2.z};
}

FVec3 operator/(const FVec3& v1, const FVec3& v2) {
	return {v1.x / v2.x, v1.y / v2.y, v1.z / v2.z};
}

DVec3::DVec3(double x) : x(x), y(x), z(x) {}
	
DVec3::DVec3(double x, double y, double z) : x(x), y(y), z(z) {}

DVec3::DVec3(const DVec2& v, double z) : x(v.x), y(v.y), z(z) {}

DVec3::DVec3(double x, const DVec2& v) : x(x), y(v.x), z(v.y) {}

DVec3 DVec3::operator-() const {
	return {-x, -y, -z};
}

bool DVec3::operator==(const DVec3& v) const {
	return x == v.x && y == v.y && z == v.z;
}

void DVec3::operator+=(double v) {
	x += v;
	y += v;
	z += v;
}

void DVec3::operator+=(const DVec3& v) {
	x += v.x;
	y += v.y;
	z += v.z;
}

void DVec3::operator-=(double v) {
	x -= v;
	y -= v;
	z -= v;
}

void DVec3::operator-=(const DVec3& v) {
	x -= v.x;
	y -= v.y;
	z -= v.z;
}

void DVec3::operator*=(double v) {
	x *= v;
	y *= v;
	z *= v;
}

void DVec3::operator*=(const DVec3& v) {
	x *= v.x;
	y *= v.y;
	z *= v.z;
}

void DVec3::operator/=(double v) {
	x /= v;
	y /= v;
	z /= v;
}

void DVec3::operator/=(const DVec3& v) {
	x /= v.x;
	y /= v.y;
	z /= v.z;
}

double DVec3::dot(const DVec3& v) const {
	return x * v.x + y * v.y + z * v.z;
}

DVec3 DVec3::cross(const DVec3& v) const {
	return {y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x};
}

double DVec3::magnitude() const {
	return sqrt(x * x + y * y + z * z);
}

double DVec3::distance(const DVec3& v) const {
	return sqrt((x - v.x) * (x - v.x) + (y - v.y) * (y - v.y) + (z - v.z) * (z - v.z));
}

DVec3 DVec3::normalize() const {
	double l = sqrt(x * x + y * y + z * z);
	return {x / l, y / l, z / l};
}

DVec3 DVec3::rotate(const DVec3& v, double a) const {
	return {(cos(a) + (1 - cos(a)) * v.x * v.x) * x +
		((1 - cos(a)) * v.x * v.y - sin(a) * v.z) * y +
		((1 - cos(a)) * v.x * v.z + sin(a) * v.y) * z,
		((1 - cos(a)) * v.x * v.y + sin(a) * v.z) * x +
		(cos(a) + (1 - cos(a)) * v.y * v.y) * y +
		((1 - cos(a)) * v.y * v.z - sin(a) * v.x) * z,
		((1 - cos(a)) * v.x * v.z - sin(a) * v.y) * x +
		((1 - cos(a)) * v.y * v.z + sin(a) * v.x) * y +
		(cos(a) + (1 - cos(a)) * v.z * v.z) * z};
}

std::string DVec3::to_string(int p) const {
	static std::stringstream stream;
	stream.setf(std::ios::fixed, std::ios::floatfield);
	stream.precision(p);
	stream.str(std::string());
	stream << "(" << x << ", " << y << ", " << z << ")";
	return stream.str();
}

DVec3 DVec3::random() {
	double angle1 = Random::random() * PI * 2;
	double angle2 = Random::random() * PI * 2;
	return {cos(angle1) * cos(angle2), sin(angle2), sin(angle1) * cos(angle2)};
}

DVec3 operator+(const DVec3& v1, double v2) {
	return {v1.x + v2, v1.y + v2, v1.z + v2};
}

DVec3 operator+(double v1, const DVec3& v2) {
	return {v1 + v2.x, v1 + v2.y, v1 + v2.z};
}

DVec3 operator+(const DVec3& v1, const DVec3& v2) {
	return {v1.x + v2.x, v1.y + v2.y, v1.z + v2.z};
}

DVec3 operator-(const DVec3& v1, double v2) {
	return {v1.x - v2, v1.y - v2, v1.z - v2};
}

DVec3 operator-(double v1, const DVec3& v2) {
	return {v1 - v2.x, v1 - v2.y, v1 - v2.z};
}

DVec3 operator-(const DVec3& v1, const DVec3& v2) {
	return {v1.x - v2.x, v1.y - v2.y, v1.z - v2.z};
}

DVec3 operator*(const DVec3& v1, double v2) {
	return {v1.x * v2, v1.y * v2, v1.z * v2};
}

DVec3 operator*(double v1, const DVec3& v2) {
	return {v1 * v2.x, v1 * v2.y, v1 * v2.z};
}

DVec3 operator*(const DVec3& v1, const DVec3& v2) {
	return {v1.x * v2.x, v1.y * v2.y, v1.z * v2.z};
}

DVec3 operator/(const DVec3& v1, double v2) {
	return {v1.x / v2, v1.y / v2, v1.z / v2};
}

DVec3 operator/(double v1, const DVec3& v2) {
	return {v1 / v2.x, v1 / v2.y, v1 / v2.z};
}

DVec3 operator/(const DVec3& v1, const DVec3& v2) {
	return {v1.x / v2.x, v1.y / v2.y, v1.z / v2.z};
}

}

/* -------------------------------------------------------------------------- */
/* ---- ink/math/Vector4.cxx ------------------------------------------------ */
/* -------------------------------------------------------------------------- */

namespace Ink {

FVec4::FVec4(float x) : x(x), y(x), z(x), w(x) {}

FVec4::FVec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

FVec4::FVec4(const FVec2& v, float z, float w) : x(v.x), y(v.y), z(z), w(w) {}

FVec4::FVec4(float x, const FVec2& v, float w) : x(x), y(v.x), z(v.y), w(w) {}

FVec4::FVec4(float x, float y, const FVec2& v) : x(x), y(y), z(v.x), w(v.y) {}

FVec4::FVec4(const FVec2& v1, const FVec2& v2) : x(v1.x), y(v1.y), z(v2.x), w(v2.y) {}

FVec4::FVec4(const FVec3& v, float w) : x(v.x), y(v.y), z(v.z), w(w) {}

FVec4::FVec4(float x, const FVec3& v) : x(x), y(v.x), z(v.y), w(v.z) {}

FVec4 FVec4::operator-() const {
	return {-x, -y, -z, -w};
}

bool FVec4::operator==(const FVec4& v) const {
	return x == v.x && y == v.y && z == v.z && w == v.w;
}

void FVec4::operator+=(float v) {
	x += v;
	y += v;
	z += v;
	w += v;
}

void FVec4::operator+=(const FVec4& v) {
	x += v.x;
	y += v.y;
	z += v.z;
	w += v.w;
}

void FVec4::operator-=(float v) {
	x -= v;
	y -= v;
	z -= v;
	w -= v;
}

void FVec4::operator-=(const FVec4& v) {
	x -= v.x;
	y -= v.y;
	z -= v.z;
	w -= v.w;
}

void FVec4::operator*=(float v) {
	x *= v;
	y *= v;
	z *= v;
	w *= v;
}

void FVec4::operator*=(const FVec4& v) {
	x *= v.x;
	y *= v.y;
	z *= v.z;
	w *= v.w;
}

void FVec4::operator/=(float v) {
	x /= v;
	y /= v;
	z /= v;
	w /= v;
}

void FVec4::operator/=(const FVec4& v) {
	x /= v.x;
	y /= v.y;
	z /= v.z;
	w /= v.w;
}

float FVec4::dot(const FVec4& v) const {
	return x * v.x + y * v.y + z * v.z + w * v.w;
}

float FVec4::magnitude() const {
	return sqrtf(x * x + y * y + z * z + w * w);
}

float FVec4::distance(const FVec4& v) const {
	return sqrtf((x - v.x) * (x - v.x) + (y - v.y) * (y - v.y) +
				 (z - v.z) * (z - v.z) + (w - v.w) * (w - v.w));
}

FVec4 FVec4::normalize() const {
	float l = sqrtf(x * x + y * y + z * z + w * w);
	return {x / l, y / l, z / l, w / l};
}

std::string FVec4::to_string(int p) const {
	static std::stringstream stream;
	stream.setf(std::ios::fixed, std::ios::floatfield);
	stream.precision(p);
	stream.str(std::string());
	stream << "(" << x << ", " << y << ", " << z << ", " << w << ")";
	return stream.str();
}

FVec4 operator+(const FVec4& v1, float v2) {
	return {v1.x + v2, v1.y + v2, v1.z + v2, v1.w + v2};
}

FVec4 operator+(float v1, const FVec4& v2) {
	return {v1 + v2.x, v1 + v2.y, v1 + v2.z, v1 + v2.w};
}

FVec4 operator+(const FVec4& v1, const FVec4& v2) {
	return {v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w};
}

FVec4 operator-(const FVec4& v1, float v2) {
	return {v1.x - v2, v1.y - v2, v1.z - v2, v1.w - v2};
}

FVec4 operator-(float v1, const FVec4& v2) {
	return {v1 - v2.x, v1 - v2.y, v1 - v2.z, v1 - v2.w};
}

FVec4 operator-(const FVec4& v1, const FVec4& v2) {
	return {v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w};
}

FVec4 operator*(const FVec4& v1, float v2) {
	return {v1.x * v2, v1.y * v2, v1.z * v2, v1.w * v2};
}

FVec4 operator*(float v1, const FVec4& v2) {
	return {v1 * v2.x, v1 * v2.y, v1 * v2.z, v1 * v2.w};
}

FVec4 operator*(const FVec4& v1, const FVec4& v2) {
	return {v1.x * v2.x, v1.y * v2.y, v1.z * v2.z, v1.w * v2.w};
}

FVec4 operator/(const FVec4& v1, float v2) {
	return {v1.x / v2, v1.y / v2, v1.z / v2, v1.w / v2};
}

FVec4 operator/(float v1, const FVec4& v2) {
	return {v1 / v2.x, v1 / v2.y, v1 / v2.z, v1 / v2.w};
}

FVec4 operator/(const FVec4& v1, const FVec4& v2) {
	return {v1.x / v2.x, v1.y / v2.y, v1.z / v2.z, v1.w / v2.w};
}

DVec4::DVec4(double x) : x(x), y(x), z(x), w(x) {}

DVec4::DVec4(double x, double y, double z, double w) : x(x), y(y), z(z), w(w) {}

DVec4::DVec4(const DVec2& v, double z, double w) : x(v.x), y(v.y), z(z), w(w) {}

DVec4::DVec4(double x, const DVec2& v, double w) : x(x), y(v.x), z(v.y), w(w) {}

DVec4::DVec4(double x, double y, const DVec2& v) : x(x), y(y), z(v.x), w(v.y) {}

DVec4::DVec4(const DVec2& v1, const DVec2& v2) : x(v1.x), y(v1.y), z(v2.x), w(v2.y) {}

DVec4::DVec4(const DVec3& v, double w) : x(v.x), y(v.y), z(v.z), w(w) {}

DVec4::DVec4(double x, const DVec3& v) : x(x), y(v.x), z(v.y), w(v.z) {}

DVec4 DVec4::operator-() const {
	return {-x, -y, -z, -w};
}

bool DVec4::operator==(const DVec4& v) const {
	return x == v.x && y == v.y && z == v.z && w == v.w;
}

void DVec4::operator+=(double v) {
	x += v;
	y += v;
	z += v;
	w += v;
}

void DVec4::operator+=(const DVec4& v) {
	x += v.x;
	y += v.y;
	z += v.z;
	w += v.w;
}

void DVec4::operator-=(double v) {
	x -= v;
	y -= v;
	z -= v;
	w -= v;
}

void DVec4::operator-=(const DVec4& v) {
	x -= v.x;
	y -= v.y;
	z -= v.z;
	w -= v.w;
}

void DVec4::operator*=(double v) {
	x *= v;
	y *= v;
	z *= v;
	w *= v;
}

void DVec4::operator*=(const DVec4& v) {
	x *= v.x;
	y *= v.y;
	z *= v.z;
	w *= v.w;
}

void DVec4::operator/=(double v) {
	x /= v;
	y /= v;
	z /= v;
	w /= v;
}

void DVec4::operator/=(const DVec4& v) {
	x /= v.x;
	y /= v.y;
	z /= v.z;
	w /= v.w;
}

double DVec4::dot(const DVec4& v) const {
	return x * v.x + y * v.y + z * v.z + w * v.w;
}

double DVec4::magnitude() const {
	return sqrt(x * x + y * y + z * z + w * w);
}

double DVec4::distance(const DVec4& v) const {
	return sqrt((x - v.x) * (x - v.x) + (y - v.y) * (y - v.y) +
				 (z - v.z) * (z - v.z) + (w - v.w) * (w - v.w));
}

DVec4 DVec4::normalize() const {
	double l = sqrt(x * x + y * y + z * z + w * w);
	return {x / l, y / l, z / l, w / l};
}

std::string DVec4::to_string(int p) const {
	static std::stringstream stream;
	stream.setf(std::ios::fixed, std::ios::floatfield);
	stream.precision(p);
	stream.str(std::string());
	stream << "(" << x << ", " << y << ", " << z << ", " << w << ")";
	return stream.str();
}

DVec4 operator+(const DVec4& v1, double v2) {
	return {v1.x + v2, v1.y + v2, v1.z + v2, v1.w + v2};
}

DVec4 operator+(double v1, const DVec4& v2) {
	return {v1 + v2.x, v1 + v2.y, v1 + v2.z, v1 + v2.w};
}

DVec4 operator+(const DVec4& v1, const DVec4& v2) {
	return {v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w};
}

DVec4 operator-(const DVec4& v1, double v2) {
	return {v1.x - v2, v1.y - v2, v1.z - v2, v1.w - v2};
}

DVec4 operator-(double v1, const DVec4& v2) {
	return {v1 - v2.x, v1 - v2.y, v1 - v2.z, v1 - v2.w};
}

DVec4 operator-(const DVec4& v1, const DVec4& v2) {
	return {v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w};
}

DVec4 operator*(const DVec4& v1, double v2) {
	return {v1.x * v2, v1.y * v2, v1.z * v2, v1.w * v2};
}

DVec4 operator*(double v1, const DVec4& v2) {
	return {v1 * v2.x, v1 * v2.y, v1 * v2.z, v1 * v2.w};
}

DVec4 operator*(const DVec4& v1, const DVec4& v2) {
	return {v1.x * v2.x, v1.y * v2.y, v1.z * v2.z, v1.w * v2.w};
}

DVec4 operator/(const DVec4& v1, double v2) {
	return {v1.x / v2, v1.y / v2, v1.z / v2, v1.w / v2};
}

DVec4 operator/(double v1, const DVec4& v2) {
	return {v1 / v2.x, v1 / v2.y, v1 / v2.z, v1 / v2.w};
}

DVec4 operator/(const DVec4& v1, const DVec4& v2) {
	return {v1.x / v2.x, v1.y / v2.y, v1.z / v2.z, v1.w / v2.w};
}

}

/* -------------------------------------------------------------------------- */
/* ---- ink/math/Matrix.cxx ------------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

float determinant_2x2(const Mat2& m) {
	/*
	 * d = M00 * M11 - M01 * M10
	 */
	return m[0][0] * m[1][1] - m[0][1] * m[1][0];
}

float determinant_3x3(const Mat3& m) {
	/*
	 * s0 = M11 * M22 - M21 * M12
	 * s1 = M12 * M20 - M10 * M22
	 * s2 = M10 * M21 - M20 * M11
	 * d = M00 * s0 + M01 * s1 + M02 * s2
	 */
	float sub0 = m[1][1] * m[2][2] - m[2][1] * m[1][2];
	float sub1 = m[1][2] * m[2][0] - m[1][0] * m[2][2];
	float sub2 = m[1][0] * m[2][1] - m[2][0] * m[1][1];
	return m[0][0] * sub0 + m[0][1] * sub1 + m[0][2] * sub2;
}

float determinant_4x4(const Mat4& m) {
	/*
	 * s0 = M22 * M33 - M32 * M23
	 * s1 = M21 * M33 - M31 * M23
	 * s2 = M21 * M32 - M31 * M22
	 * s3 = M20 * M33 - M30 * M23
	 * s4 = M20 * M32 - M30 * M22
	 * s5 = M20 * M31 - M30 * M21
	 * d = M00 * (M11 * s0 - M12 * s1 + M13 * s2) -
	 *     M01 * (M10 * s0 - M12 * s3 + M13 * s4) +
	 *     M02 * (M10 * s1 - M11 * s3 + M13 * s5) -
	 *     M03 * (M10 * s2 - M11 * s4 + M12 * s5)
	 */
	float sub0 = m[2][2] * m[3][3] - m[3][2] * m[2][3];
	float sub1 = m[2][1] * m[3][3] - m[3][1] * m[2][3];
	float sub2 = m[2][1] * m[3][2] - m[3][1] * m[2][2];
	float sub3 = m[2][0] * m[3][3] - m[3][0] * m[2][3];
	float sub4 = m[2][0] * m[3][2] - m[3][0] * m[2][2];
	float sub5 = m[2][0] * m[3][1] - m[3][0] * m[2][1];
	return (m[0][0] * (m[1][1] * sub0 - m[1][2] * sub1 + m[1][3] * sub2) -
		    m[0][1] * (m[1][0] * sub0 - m[1][2] * sub3 + m[1][3] * sub4) +
		    m[0][2] * (m[1][0] * sub1 - m[1][1] * sub3 + m[1][3] * sub5) -
		    m[0][3] * (m[1][0] * sub2 - m[1][1] * sub4 + m[1][2] * sub5));
}

Mat2 inverse_2x2(const Mat2& m) {
	/*
	 * d = M00 * M11 - M01 * M10
	 * M = [
	 *      M11 / d, -M01 / d,
	 *     -M10 / d,  M00 / d,
	 * ]
	 */
	float inv_det = 1 / (m[0][0] * m[1][1] - m[0][1] * m[1][0]);
	return {
		 inv_det * m[1][1],
		-inv_det * m[0][1],
		-inv_det * m[1][0],
		 inv_det * m[0][0],
	};
}

Mat3 inverse_3x3(const Mat3& m) {
	/*
	 * i0 = M11 * M22 - M21 * M12
	 * i1 = M12 * M20 - M10 * M22
	 * i2 = M10 * M21 - M20 * M11
	 * d = M00 * i0 + M01 * i1 + M02 * i2
	 * M = [
	 *     M11 * M22 - M21 * M12, M02 * M22 - M21 * M12, M01 * M12 - M02 * M11,
	 *     M12 * M20 - M10 * M22, M00 * M22 - M02 * M20, M10 * M02 - M00 * M12,
	 *     M10 * M21 - M20 * M11, M20 * M01 - M00 * M21, M00 * M11 - M10 * M01,
	 * ]
	 * M = M / d
	 */
	float inv0 = m[1][1] * m[2][2] - m[2][1] * m[1][2];
	float inv1 = m[1][2] * m[2][0] - m[1][0] * m[2][2];
	float inv2 = m[1][0] * m[2][1] - m[2][0] * m[1][1];
	float inv_det = 1 / (m[0][0] * inv0 + m[0][1] * inv1 + m[0][2] * inv2);
	return {
		inv_det * inv0,
		inv_det * (m[0][2] * m[2][1] - m[0][1] * m[2][2]),
		inv_det * (m[0][1] * m[1][2] - m[0][2] * m[1][1]),
		inv_det * inv1,
		inv_det * (m[0][0] * m[2][2] - m[0][2] * m[2][0]),
		inv_det * (m[1][0] * m[0][2] - m[0][0] * m[1][2]),
		inv_det * inv2,
		inv_det * (m[2][0] * m[0][1] - m[0][0] * m[2][1]),
		inv_det * (m[0][0] * m[1][1] - m[1][0] * m[0][1]),
	};
}

Mat4 inverse_4x4(const Mat4& m) {
	/*
	 * s00 = M22 * M33 - M23 * M32
	 * s01 = M21 * M33 - M23 * M31
	 * s02 = M21 * M32 - M22 * M31
	 * s03 = M20 * M33 - M23 * M30
	 * s04 = M20 * M32 - M22 * M30
	 * s05 = M20 * M31 - M21 * M30
	 * s06 = M12 * M33 - M13 * M32
	 * s07 = M11 * M33 - M13 * M31
	 * s08 = M11 * M32 - M12 * M31
	 * s09 = M12 * M23 - M13 * M22
	 * s10 = M11 * M23 - M13 * M21
	 * s11 = M11 * M22 - M12 * M21
	 * s12 = M10 * M33 - M13 * M30
	 * s13 = M10 * M32 - M12 * M30
	 * s14 = M10 * M23 - M13 * M20
	 * s15 = M10 * M22 - M12 * M20
	 * s16 = M10 * M31 - M11 * M30
	 * s17 = M10 * M21 - M11 * M20
	 * i00 = M11 * s00 - M12 * s01 + M13 * s02
	 * i01 = M10 * s00 - M12 * s03 + M13 * s04
	 * i02 = M10 * s01 - M11 * s03 + M13 * s05
	 * i03 = M10 * s02 - M11 * s04 + M12 * s05
	 * d = M00 * i00 - M01 * i01 + M02 * i02 - M03 * i03
	 * M = [
	 *      (M11 * s00 - M12 * s01 + M13 * s02),
	 *     -(M01 * s00 - M02 * s01 + M03 * s02),
	 *      (M01 * s06 - M02 * s07 + M03 * s08),
	 *     -(M01 * s09 - M02 * s10 + M03 * s11),
	 *     -(M10 * s00 - M12 * s03 + M13 * s04),
	 *      (M00 * s00 - M02 * s03 + M03 * s04),
	 *     -(M00 * s06 - M02 * s12 + M03 * s13),
	 *      (M00 * s09 - M02 * s14 + M03 * s15),
	 *      (M10 * s01 - M11 * s03 + M13 * s05),
	 *     -(M00 * s01 - M01 * s03 + M03 * s05),
	 *      (M00 * s07 - M01 * s12 + M03 * s16),
	 *     -(M00 * s10 - M01 * s14 + M03 * s17),
	 *     -(M10 * s02 - M11 * s04 + M12 * s05),
	 *      (M00 * s02 - M01 * s04 + M02 * s05),
	 *     -(M00 * s08 - M01 * s13 + M02 * s16),
	 *      (M00 * s11 - M01 * s15 + M02 * s17),
	 * ]
	 * M = M / d
	 */
	float sub00 = m[2][2] * m[3][3] - m[2][3] * m[3][2];
	float sub01 = m[2][1] * m[3][3] - m[2][3] * m[3][1];
	float sub02 = m[2][1] * m[3][2] - m[2][2] * m[3][1];
	float sub03 = m[2][0] * m[3][3] - m[2][3] * m[3][0];
	float sub04 = m[2][0] * m[3][2] - m[2][2] * m[3][0];
	float sub05 = m[2][0] * m[3][1] - m[2][1] * m[3][0];
	float sub06 = m[1][2] * m[3][3] - m[1][3] * m[3][2];
	float sub07 = m[1][1] * m[3][3] - m[1][3] * m[3][1];
	float sub08 = m[1][1] * m[3][2] - m[1][2] * m[3][1];
	float sub09 = m[1][2] * m[2][3] - m[1][3] * m[2][2];
	float sub10 = m[1][1] * m[2][3] - m[1][3] * m[2][1];
	float sub11 = m[1][1] * m[2][2] - m[1][2] * m[2][1];
	float sub12 = m[1][0] * m[3][3] - m[1][3] * m[3][0];
	float sub13 = m[1][0] * m[3][2] - m[1][2] * m[3][0];
	float sub14 = m[1][0] * m[2][3] - m[1][3] * m[2][0];
	float sub15 = m[1][0] * m[2][2] - m[1][2] * m[2][0];
	float sub16 = m[1][0] * m[3][1] - m[1][1] * m[3][0];
	float sub17 = m[1][0] * m[2][1] - m[1][1] * m[2][0];
	float inv00 = m[1][1] * sub00 - m[1][2] * sub01 + m[1][3] * sub02;
	float inv01 = m[1][0] * sub00 - m[1][2] * sub03 + m[1][3] * sub04;
	float inv02 = m[1][0] * sub01 - m[1][1] * sub03 + m[1][3] * sub05;
	float inv03 = m[1][0] * sub02 - m[1][1] * sub04 + m[1][2] * sub05;
	float inv_det = 1 / (m[0][0] * inv00 - m[0][1] * inv01 + m[0][2] * inv02 - m[0][3] * inv03);
	return {
		 inv_det * inv00,
		-inv_det * (m[0][1] * sub00 - m[0][2] * sub01 + m[0][3] * sub02),
		 inv_det * (m[0][1] * sub06 - m[0][2] * sub07 + m[0][3] * sub08),
		-inv_det * (m[0][1] * sub09 - m[0][2] * sub10 + m[0][3] * sub11),
		-inv_det * inv01,
		 inv_det * (m[0][0] * sub00 - m[0][2] * sub03 + m[0][3] * sub04),
		-inv_det * (m[0][0] * sub06 - m[0][2] * sub12 + m[0][3] * sub13),
		 inv_det * (m[0][0] * sub09 - m[0][2] * sub14 + m[0][3] * sub15),
		 inv_det * inv02,
		-inv_det * (m[0][0] * sub01 - m[0][1] * sub03 + m[0][3] * sub05),
		 inv_det * (m[0][0] * sub07 - m[0][1] * sub12 + m[0][3] * sub16),
		-inv_det * (m[0][0] * sub10 - m[0][1] * sub14 + m[0][3] * sub17),
		-inv_det * inv03,
		 inv_det * (m[0][0] * sub02 - m[0][1] * sub04 + m[0][2] * sub05),
		-inv_det * (m[0][0] * sub08 - m[0][1] * sub13 + m[0][2] * sub16),
		 inv_det * (m[0][0] * sub11 - m[0][1] * sub15 + m[0][2] * sub17),
	};
}

double determinant_2x2(const DMat2& m) {
	/*
	 * d = M00 * M11 - M01 * M10
	 */
	return m[0][0] * m[1][1] - m[0][1] * m[1][0];
}

double determinant_3x3(const DMat3& m) {
	/*
	 * s0 = M11 * M22 - M21 * M12
	 * s1 = M12 * M20 - M10 * M22
	 * s2 = M10 * M21 - M20 * M11
	 * d = M00 * s0 + M01 * s1 + M02 * s2
	 */
	double sub0 = m[1][1] * m[2][2] - m[2][1] * m[1][2];
	double sub1 = m[1][2] * m[2][0] - m[1][0] * m[2][2];
	double sub2 = m[1][0] * m[2][1] - m[2][0] * m[1][1];
	return m[0][0] * sub0 + m[0][1] * sub1 + m[0][2] * sub2;
}

double determinant_4x4(const DMat4& m) {
	/*
	 * s0 = M22 * M33 - M32 * M23
	 * s1 = M21 * M33 - M31 * M23
	 * s2 = M21 * M32 - M31 * M22
	 * s3 = M20 * M33 - M30 * M23
	 * s4 = M20 * M32 - M30 * M22
	 * s5 = M20 * M31 - M30 * M21
	 * d = M00 * (M11 * s0 - M12 * s1 + M13 * s2) -
	 *     M01 * (M10 * s0 - M12 * s3 + M13 * s4) +
	 *     M02 * (M10 * s1 - M11 * s3 + M13 * s5) -
	 *     M03 * (M10 * s2 - M11 * s4 + M12 * s5)
	 */
	double sub0 = m[2][2] * m[3][3] - m[3][2] * m[2][3];
	double sub1 = m[2][1] * m[3][3] - m[3][1] * m[2][3];
	double sub2 = m[2][1] * m[3][2] - m[3][1] * m[2][2];
	double sub3 = m[2][0] * m[3][3] - m[3][0] * m[2][3];
	double sub4 = m[2][0] * m[3][2] - m[3][0] * m[2][2];
	double sub5 = m[2][0] * m[3][1] - m[3][0] * m[2][1];
	return m[0][0] * (m[1][1] * sub0 - m[1][2] * sub1 + m[1][3] * sub2) -
		   m[0][1] * (m[1][0] * sub0 - m[1][2] * sub3 + m[1][3] * sub4) +
		   m[0][2] * (m[1][0] * sub1 - m[1][1] * sub3 + m[1][3] * sub5) -
		   m[0][3] * (m[1][0] * sub2 - m[1][1] * sub4 + m[1][2] * sub5);
}

DMat2 inverse_2x2(const DMat2& m) {
	/*
	 * d = M00 * M11 - M01 * M10
	 * M = [
	 *      M11 / d, -M01 / d,
	 *     -M10 / d,  M00 / d,
	 * ]
	 */
	double inv_det = 1 / (m[0][0] * m[1][1] - m[0][1] * m[1][0]);
	return {
		 inv_det * m[1][1],
		-inv_det * m[0][1],
		-inv_det * m[1][0],
		 inv_det * m[0][0],
	};
}

DMat3 inverse_3x3(const DMat3& m) {
	/*
	 * i0 = M11 * M22 - M21 * M12
	 * i1 = M12 * M20 - M10 * M22
	 * i2 = M10 * M21 - M20 * M11
	 * d = M00 * i0 + M01 * i1 + M02 * i2
	 * M = [
	 *     M11 * M22 - M21 * M12, M02 * M22 - M21 * M12, M01 * M12 - M02 * M11,
	 *     M12 * M20 - M10 * M22, M00 * M22 - M02 * M20, M10 * M02 - M00 * M12,
	 *     M10 * M21 - M20 * M11, M20 * M01 - M00 * M21, M00 * M11 - M10 * M01,
	 * ]
	 * M = M / d
	 */
	double inv0 = m[1][1] * m[2][2] - m[2][1] * m[1][2];
	double inv1 = m[1][2] * m[2][0] - m[1][0] * m[2][2];
	double inv2 = m[1][0] * m[2][1] - m[2][0] * m[1][1];
	double inv_det = 1 / (m[0][0] * inv0 + m[0][1] * inv1 + m[0][2] * inv2);
	return {
		inv_det * inv0,
		inv_det * (m[0][2] * m[2][1] - m[0][1] * m[2][2]),
		inv_det * (m[0][1] * m[1][2] - m[0][2] * m[1][1]),
		inv_det * inv1,
		inv_det * (m[0][0] * m[2][2] - m[0][2] * m[2][0]),
		inv_det * (m[1][0] * m[0][2] - m[0][0] * m[1][2]),
		inv_det * inv2,
		inv_det * (m[2][0] * m[0][1] - m[0][0] * m[2][1]),
		inv_det * (m[0][0] * m[1][1] - m[1][0] * m[0][1]),
	};
}

DMat4 inverse_4x4(const DMat4& m) {
	/*
	 * s00 = M22 * M33 - M23 * M32
	 * s01 = M21 * M33 - M23 * M31
	 * s02 = M21 * M32 - M22 * M31
	 * s03 = M20 * M33 - M23 * M30
	 * s04 = M20 * M32 - M22 * M30
	 * s05 = M20 * M31 - M21 * M30
	 * s06 = M12 * M33 - M13 * M32
	 * s07 = M11 * M33 - M13 * M31
	 * s08 = M11 * M32 - M12 * M31
	 * s09 = M12 * M23 - M13 * M22
	 * s10 = M11 * M23 - M13 * M21
	 * s11 = M11 * M22 - M12 * M21
	 * s12 = M10 * M33 - M13 * M30
	 * s13 = M10 * M32 - M12 * M30
	 * s14 = M10 * M23 - M13 * M20
	 * s15 = M10 * M22 - M12 * M20
	 * s16 = M10 * M31 - M11 * M30
	 * s17 = M10 * M21 - M11 * M20
	 * i00 = M11 * s00 - M12 * s01 + M13 * s02
	 * i01 = M10 * s00 - M12 * s03 + M13 * s04
	 * i02 = M10 * s01 - M11 * s03 + M13 * s05
	 * i03 = M10 * s02 - M11 * s04 + M12 * s05
	 * d = M00 * i00 - M01 * i01 + M02 * i02 - M03 * i03
	 * M = [
	 *      (M11 * s00 - M12 * s01 + M13 * s02),
	 *     -(M01 * s00 - M02 * s01 + M03 * s02),
	 *      (M01 * s06 - M02 * s07 + M03 * s08),
	 *     -(M01 * s09 - M02 * s10 + M03 * s11),
	 *     -(M10 * s00 - M12 * s03 + M13 * s04),
	 *      (M00 * s00 - M02 * s03 + M03 * s04),
	 *     -(M00 * s06 - M02 * s12 + M03 * s13),
	 *      (M00 * s09 - M02 * s14 + M03 * s15),
	 *      (M10 * s01 - M11 * s03 + M13 * s05),
	 *     -(M00 * s01 - M01 * s03 + M03 * s05),
	 *      (M00 * s07 - M01 * s12 + M03 * s16),
	 *     -(M00 * s10 - M01 * s14 + M03 * s17),
	 *     -(M10 * s02 - M11 * s04 + M12 * s05),
	 *      (M00 * s02 - M01 * s04 + M02 * s05),
	 *     -(M00 * s08 - M01 * s13 + M02 * s16),
	 *      (M00 * s11 - M01 * s15 + M02 * s17),
	 * ]
	 * M = M / d
	 */
	double sub00 = m[2][2] * m[3][3] - m[2][3] * m[3][2];
	double sub01 = m[2][1] * m[3][3] - m[2][3] * m[3][1];
	double sub02 = m[2][1] * m[3][2] - m[2][2] * m[3][1];
	double sub03 = m[2][0] * m[3][3] - m[2][3] * m[3][0];
	double sub04 = m[2][0] * m[3][2] - m[2][2] * m[3][0];
	double sub05 = m[2][0] * m[3][1] - m[2][1] * m[3][0];
	double sub06 = m[1][2] * m[3][3] - m[1][3] * m[3][2];
	double sub07 = m[1][1] * m[3][3] - m[1][3] * m[3][1];
	double sub08 = m[1][1] * m[3][2] - m[1][2] * m[3][1];
	double sub09 = m[1][2] * m[2][3] - m[1][3] * m[2][2];
	double sub10 = m[1][1] * m[2][3] - m[1][3] * m[2][1];
	double sub11 = m[1][1] * m[2][2] - m[1][2] * m[2][1];
	double sub12 = m[1][0] * m[3][3] - m[1][3] * m[3][0];
	double sub13 = m[1][0] * m[3][2] - m[1][2] * m[3][0];
	double sub14 = m[1][0] * m[2][3] - m[1][3] * m[2][0];
	double sub15 = m[1][0] * m[2][2] - m[1][2] * m[2][0];
	double sub16 = m[1][0] * m[3][1] - m[1][1] * m[3][0];
	double sub17 = m[1][0] * m[2][1] - m[1][1] * m[2][0];
	double inv00 = m[1][1] * sub00 - m[1][2] * sub01 + m[1][3] * sub02;
	double inv01 = m[1][0] * sub00 - m[1][2] * sub03 + m[1][3] * sub04;
	double inv02 = m[1][0] * sub01 - m[1][1] * sub03 + m[1][3] * sub05;
	double inv03 = m[1][0] * sub02 - m[1][1] * sub04 + m[1][2] * sub05;
	double inv_det = 1 / (m[0][0] * inv00 - m[0][1] * inv01 + m[0][2] * inv02 - m[0][3] * inv03);
	return {
		 inv_det * inv00,
		-inv_det * (m[0][1] * sub00 - m[0][2] * sub01 + m[0][3] * sub02),
		 inv_det * (m[0][1] * sub06 - m[0][2] * sub07 + m[0][3] * sub08),
		-inv_det * (m[0][1] * sub09 - m[0][2] * sub10 + m[0][3] * sub11),
		-inv_det * inv01,
		 inv_det * (m[0][0] * sub00 - m[0][2] * sub03 + m[0][3] * sub04),
		-inv_det * (m[0][0] * sub06 - m[0][2] * sub12 + m[0][3] * sub13),
		 inv_det * (m[0][0] * sub09 - m[0][2] * sub14 + m[0][3] * sub15),
		 inv_det * inv02,
		-inv_det * (m[0][0] * sub01 - m[0][1] * sub03 + m[0][3] * sub05),
		 inv_det * (m[0][0] * sub07 - m[0][1] * sub12 + m[0][3] * sub16),
		-inv_det * (m[0][0] * sub10 - m[0][1] * sub14 + m[0][3] * sub17),
		-inv_det * inv03,
		 inv_det * (m[0][0] * sub02 - m[0][1] * sub04 + m[0][2] * sub05),
		-inv_det * (m[0][0] * sub08 - m[0][1] * sub13 + m[0][2] * sub16),
		 inv_det * (m[0][0] * sub11 - m[0][1] * sub15 + m[0][2] * sub17),
	};
}

}

/* -------------------------------------------------------------------------- */
/* ---- ink/math/Ray.cxx ---------------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

Ray::Ray(const Vec3& o, const Vec3& d) : origin(o), direction(d) {}

float Ray::intersect_box(const Vec3& l, const Vec3& u) const {
	Vec3 inv_dir = 1 / direction;
	Vec3 tmin = (l - origin) * inv_dir;
	Vec3 tmax = (u - origin) * inv_dir;
	if (tmin.x > tmax.x) std::swap(tmin.x, tmax.x);
	if (tmin.y > tmax.y) std::swap(tmin.y, tmax.y);
	if (tmin.z > tmax.z) std::swap(tmin.z, tmax.z);
	float t0 = fmax(fmax(tmin.x, tmin.y), tmin.z);
	float t1 = fmin(fmin(tmax.x, tmax.y), tmax.z);
	return t0 > t1 ? -1 : t0 > 0 ? t0 : t1 > 0 ? t1 : -1;
}

float Ray::intersect_plane(const Vec3& n, float d) const {
	float t = n.dot(n * d - origin) / n.dot(direction);
	return t < 0 ? -1 : t;
}

float Ray::intersect_shpere(const Vec3& c, float r) const {
	Vec3 oc = c - origin;
	float t = direction.dot(oc);
	float d2 = oc.magnitude() - t * t;
	float r2 = r * r;
	if (d2 > r2) return -1;
	float dt = sqrtf(r2 - d2);
	float t0 = t - dt;
	float t1 = t + dt;
	return t1 < 0 ? -1 : t0 < 0 ? t1 : t0;
}

float Ray::intersect_triangle(const Vec3& a, const Vec3& b, const Vec3& c) const {
	Vec3 ab = b - a;
	Vec3 ac = c - a;
	Vec3 ao = origin - a;
	Vec3 p = direction.cross(ac);
	Vec3 q = ao.cross(ab);
	float inv = 1 / ab.dot(p);
	float u = direction.dot(q) * inv;
	float v = ao.dot(p) * inv;
	float t = ac.dot(q) * inv;
	return t < 0 || u < 0 || u > 1 || v < 0 || u + v > 1 ? -1 : t;
}

}

/* -------------------------------------------------------------------------- */
/* ---- ink/math/Euler.cxx -------------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

Euler::Euler(float x, float y, float z, EulerOrder o) :
x(x), y(y), z(z), order(o) {}

Euler::Euler(Vec3 r, EulerOrder o) :
x(r.x), y(r.y), z(r.z), order(o) {}

Mat3 Euler::to_rotation_matrix() const {
	Mat3 rotation_x = {
		1       , 0       , 0       ,
		0       , cosf(x) , -sinf(x),
		0       , sinf(x) , cosf(x) ,
	};
	Mat3 rotation_y = {
		cosf(y) , 0       , sinf(y) ,
		0       , 1       , 0       ,
		-sinf(y), 0       , cosf(y) ,
	};
	Mat3 rotation_z = {
		cosf(z) , -sinf(z), 0       ,
		sinf(z) , cosf(z) , 0       ,
		0       , 0       , 1       ,
	};
	if (order == EULER_XYZ) {
		return rotation_x * rotation_y * rotation_z;
	}
	if (order == EULER_XZY) {
		return rotation_x * rotation_z * rotation_y;
	}
	if (order == EULER_YXZ) {
		return rotation_y * rotation_x * rotation_z;
	}
	if (order == EULER_YZX) {
		return rotation_y * rotation_z * rotation_x;
	}
	if (order == EULER_ZXY) {
		return rotation_z * rotation_x * rotation_y;
	}
	/*       ... EULER_ZYX */ {
		return rotation_z * rotation_y * rotation_x;
	}
}

}

/* -------------------------------------------------------------------------- */
/* ---- ink/math/Color.cxx -------------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

const Mat3 RGB_TO_XYZ = {
	0.4124564, 0.3575761, 0.1804375,
	0.2126729, 0.7151522, 0.0721750,
	0.0193339, 0.1191920, 0.9503041
};

const Mat3 XYZ_TO_RGB = {
	 3.2404542, -1.5371385, -0.4985314,
	-0.9692660,  1.8760108,  0.0415560,
	 0.0556434, -0.2040259,  1.0572252
};

const Vec3 HCY = {0.299, 0.587, 0.114};

Vec3 Color::rgb_to_srgb(const Vec3& c) {
	return {rgb_to_srgb(c.x), rgb_to_srgb(c.y), rgb_to_srgb(c.z)};
}

Vec3 Color::srgb_to_rgb(const Vec3& c) {
	return {srgb_to_rgb(c.x), srgb_to_rgb(c.y), srgb_to_rgb(c.z)};
}

Vec3 Color::rgb_to_xyz(const Vec3& c) {
	return RGB_TO_XYZ * c;
}

Vec3 Color::xyz_to_rgb(const Vec3& c) {
	return XYZ_TO_RGB * c;
}

Vec3 Color::rgb_to_hsv(const Vec3& c) {
	Vec3 hcv = rgb_to_hcv(c);
	return {hcv.x, hcv.y / hcv.z, hcv.z};
}

Vec3 Color::hsv_to_rgb(const Vec3& c) {
	return ((hue_to_rgb(c.x) - 1.f) * c.y + 1.f) * c.z;
}

Vec3 Color::rgb_to_hsl(const Vec3& c) {
	Vec3 hcv = rgb_to_hcv(c);
	float lum = hcv.z - hcv.y * 0.5f;
	float sat = hcv.y / (1.f - fabsf(lum * 2.f - 1.f));
	return {hcv.x, sat, lum};
}

Vec3 Color::hsl_to_rgb(const Vec3& c) {
	return (hue_to_rgb(c.x) - 0.5f) * (1.f - fabsf(2.f * c.z - 1.f)) * c.y + c.z;
}

Vec3 Color::rgb_to_hcy(const Vec3& c) {
	Vec3 hcv = rgb_to_hcv(c);
	float y = HCY.dot(c);
	float z = HCY.dot(hue_to_rgb(hcv.x));
	hcv.y *= y < z ? z / y : (1.f - z) / (1.f - y);
	return {hcv.x, hcv.y, y};
}

Vec3 Color::hcy_to_rgb(const Vec3& c) {
	Vec3 rgb = hue_to_rgb(c.x);
	float d = HCY.dot(rgb);
	float y = c.y * (c.z < d ? c.z / d : d < 1.f ? (1.f - c.z) / (1.f - d) : 1.f);
	return (rgb - d) * y + c.z;
}

float Color::saturate(float v) {
	return v < 0 ? 0 : v > 1 ? 1 : v;
}

float Color::rgb_to_srgb(float v) {
	return v <= 0.0031308f ? v * 12.92f : powf(v, 1.f / 2.4f) * 1.055f - 0.055f;
}

float Color::srgb_to_rgb(float v) {
	return v <= 0.04045f ? v / 12.92f : powf((v + 0.055f) / 1.055f, 2.4f);
}

Vec3 Color::hue_to_rgb(float h) {
	float r = fabsf(h * 6.f - 3.f) - 1.f;
	float g = 2.f - fabsf(h * 6.f - 2.f);
	float b = 2.f - fabsf(h * 6.f - 4.f);
	return {saturate(r), saturate(g), saturate(b)};
}

Vec3 Color::rgb_to_hcv(const Vec3& c) {
	Vec4 p = c.y < c.z ? Vec4(c.z, c.y, -1.f, 2.f / 3.f) : Vec4(c.y, c.z, 0.f, -1.f / 3.f);
	Vec4 q = c.x < p.x ? Vec4(p.x, p.y, p.w, c.x) : Vec4(c.x, p.y, p.z, p.x);
	float chr = q.x - fminf(q.w, q.y);
	float hue = fabsf((q.w - q.y) / (6.f * chr) + q.z);
	return {hue, chr, q.x};
}

}

/* -------------------------------------------------------------------------- */
/* ---- ink/objects/Uniforms.cxx -------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

size_t Uniforms::get_count() const {
	return unifroms.size();
}

std::string Uniforms::get_name(int i) const {
	return std::get<0>(unifroms[i]);
}

int Uniforms::get_type(int i) const {
	return std::get<1>(unifroms[i]);
}

int Uniforms::get_location(int i) const {
	return std::get<2>(unifroms[i]);
}

float* Uniforms::get_data() {
	return data.data();
}

const float* Uniforms::get_data() const {
	return data.data();
}

void Uniforms::set_i(const std::string& n, int v) {
	size_t size = data.size();
	unifroms.emplace_back(std::make_tuple(n, 0, size));
	data.resize(size + 1);
	std::copy_n(&v, 1, data.data() + size);
}

void Uniforms::set_u(const std::string& n, unsigned int v) {
	size_t size = data.size();
	unifroms.emplace_back(std::make_tuple(n, 1, size));
	data.resize(size + 1);
	std::copy_n(&v, 1, data.data() + size);
}

void Uniforms::set_f(const std::string& n, float v) {
	size_t size = data.size();
	unifroms.emplace_back(std::make_tuple(n, 2, size));
	data.resize(size + 1);
	std::copy_n(&v, 1, data.data() + size);
}

void Uniforms::set_v2(const std::string& n, const Vec2& v) {
	size_t size = data.size();
	unifroms.emplace_back(std::make_tuple(n, 3, size));
	data.resize(size + 2);
	std::copy_n(&v.x, 2, data.data() + size);
}

void Uniforms::set_v3(const std::string& n, const Vec3& v) {
	size_t size = data.size();
	unifroms.emplace_back(std::make_tuple(n, 4, size));
	data.resize(size + 3);
	std::copy_n(&v.x, 3, data.data() + size);
}

void Uniforms::set_v4(const std::string& n, const Vec4& v) {
	size_t size = data.size();
	unifroms.emplace_back(std::make_tuple(n, 5, size));
	data.resize(size + 4);
	std::copy_n(&v.x, 4, data.data() + size);
}

void Uniforms::set_m2(const std::string& n, const Mat2& v) {
	size_t size = data.size();
	unifroms.emplace_back(std::make_tuple(n, 6, size));
	data.resize(size + 4);
	std::copy_n(&v[0][0], 4, data.data() + size);
}

void Uniforms::set_m3(const std::string& n, const Mat3& v) {
	size_t size = data.size();
	unifroms.emplace_back(std::make_tuple(n, 7, size));
	data.resize(size + 9);
	std::copy_n(&v[0][0], 9, data.data() + size);
}

void Uniforms::set_m4(const std::string& n, const Mat4& v) {
	size_t size = data.size();
	unifroms.emplace_back(std::make_tuple(n, 8, size));
	data.resize(size + 16);
	std::copy_n(&v[0][0], 16, data.data() + size);
}

void Uniforms::clear() {
	unifroms.clear();
	data.clear();
}

}

/* -------------------------------------------------------------------------- */
/* ---- ink/objects/Defines.cxx --------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

std::string Defines::get() const {
	return defines;
}

void Defines::set(const Defines& d) {
	defines += d.get();
}

void Defines::set(const std::string& n) {
	defines += "#define " + n + "\n";
}

void Defines::set(const std::string& n, const std::string& v) {
	defines += "#define " + n + " " + v + "\n";
}

void Defines::set_i(const std::string& n, int v) {
	defines += "#define " + n + " " + std::to_string(v) + "\n";
}

void Defines::set_l(const std::string& n, long v) {
	defines += "#define " + n + " " + std::to_string(v) + "\n";
}

void Defines::set_ll(const std::string& n, long long v) {
	defines += "#define " + n + " " + std::to_string(v) + "\n";
}

void Defines::set_if(const std::string& n, bool f) {
	if (f) defines += "#define " + n + "\n";
}

void Defines::clear() {
	defines.clear();
}

}

/* -------------------------------------------------------------------------- */
/* ---- ink/objects/Mesh.cxx ------------------------------------------------ */
/* -------------------------------------------------------------------------- */

namespace Ink {

Mesh::Mesh(const std::string& n) : name(n) {}

void Mesh::translate(float x, float y, float z) {
	for (auto& v : vertex) {
		v.x += x;
		v.y += y;
		v.z += z;
	}
}

void Mesh::translate(const Vec3& t) {
	for (auto& v : vertex) {
		v += t;
	}
}

void Mesh::rotate_x(float a) {
	for (auto& v : vertex) {
		float v_y = v.y;
		float v_z = v.z;
		v.y = cosf(a) * v_y - sinf(a) * v_z;
		v.z = sinf(a) * v_y + cosf(a) * v_z;
	}
	for (auto& n : normal) {
		float n_y = n.y;
		float n_z = n.z;
		n.y = cosf(a) * n_y - sinf(a) * n_z;
		n.z = sinf(a) * n_y + cosf(a) * n_z;
		n = n.normalize();
	}
	for (auto& t : tangent) {
		Vec3 t_xyz = {t.x, t.y, t.z};
		t_xyz.y = cosf(a) * t.y - sinf(a) * t.z;
		t_xyz.z = sinf(a) * t.y + cosf(a) * t.z;
		t = {t_xyz.normalize(), t.w};
	}
}

void Mesh::rotate_y(float a) {
	for (auto& v : vertex) {
		float v_x = v.y;
		float v_z = v.z;
		v.x = cosf(a) * v_x + sinf(a) * v_z;
		v.z = -sinf(a) * v_x + cosf(a) * v_z;
	}
	for (auto& n : normal) {
		float n_x = n.y;
		float n_z = n.z;
		n.x = cosf(a) * n_x + sinf(a) * n_z;
		n.z = -sinf(a) * n_x + cosf(a) * n_z;
		n = n.normalize();
	}
	for (auto& t : tangent) {
		Vec3 t_xyz = {t.x, t.y, t.z};
		t_xyz.x = cosf(a) * t.x + sinf(a) * t.z;
		t_xyz.z = -sinf(a) * t.x + cosf(a) * t.z;
		t = {t_xyz.normalize(), t.w};
	}
}

void Mesh::rotate_z(float a) {
	for (auto& v : vertex) {
		float v_x = v.x;
		float v_y = v.y;
		v.x = cosf(a) * v_x - sinf(a) * v_y;
		v.y = sinf(a) * v_x + cosf(a) * v_y;
	}
	for (auto& n : normal) {
		float n_x = n.x;
		float n_y = n.y;
		n.x = cosf(a) * n_x - sinf(a) * n_y;
		n.y = sinf(a) * n_x + cosf(a) * n_y;
		n = n.normalize();
	}
	for (auto& t : tangent) {
		Vec3 t_xyz = {t.x, t.y, t.z};
		t_xyz.x = cosf(a) * t.x - sinf(a) * t.y;
		t_xyz.y = sinf(a) * t.x + cosf(a) * t.y;
		t = {t_xyz.normalize(), t.w};
	}
}

void Mesh::rotate(const Euler& e) {
	Mat3 rotation_matrix = e.to_rotation_matrix();
	for (auto& v : vertex) {
		v = rotation_matrix * v;
	}
	for (auto& n : normal) {
		n = Vec3(rotation_matrix * n).normalize();
	}
	for (auto& t : tangent) {
		Vec3 t_xyz = rotation_matrix * Vec3(t.x, t.y, t.z);
		t = {t_xyz.normalize(), t.w};
	}
}

void Mesh::scale(float x, float y, float z) {
	for (auto& v : vertex) {
		v.x *= x;
		v.y *= y;
		v.z *= z;
	}
	for (auto& n : normal) {
		n.x /= x;
		n.y /= y;
		n.z /= z;
		n = n.normalize();
	}
	for (auto& t : tangent) {
		Vec3 t_xyz = {t.x * x, t.y * y, t.z * z};
		t = {t_xyz.normalize(), t.w};
	}
}

void Mesh::scale(const Vec3& s) {
	for (auto& v : vertex) {
		v *= s;
	}
	for (auto& n : normal) {
		n = (n / s).normalize();
	}
	for (auto& t : tangent) {
		Vec3 t_xyz = {t.x * s.x, t.y * s.y, t.z * s.z};
		t = {t_xyz.normalize(), t.w};
	}
}

void Mesh::normalize() {
	size_t size = normal.size();
	for (int i = 0; i < size; ++i) {
		normal[i] = normal[i].normalize();
	}
}

void Mesh::create_normals() {
	if (vertex.empty()) {
		return Error::set("Mesh", "Vertex information is missing");
	}
	size_t size = vertex.size();
	normal.resize(size);
	std::unordered_map<std::string, Vec3> normals;
	for (int i = 0; i < size; i += 3) {
		Vec3 v1 = vertex[i + 1] - vertex[i];
		Vec3 v2 = vertex[i + 2] - vertex[i];
		Vec3 face_normal = v1.cross(v2);
		for (int j = i; j < i + 3; ++j) {
			std::string hash = (vertex[j] + Vec3(0.005)).to_string(2);
			if (normals.count(hash) == 0) normals.insert({hash, {}});
			normals[hash] += face_normal.normalize();
		}
	}
	for (auto& [k, v] : normals) {
		v = v.normalize();
	}
	for (int i = 0; i < size; ++i) {
		std::string hash = (vertex[i] + Vec3(0.005)).to_string(2);
		normal[i] = normals[hash];
	}
}

void Mesh::create_tangents() {
	if (vertex.empty()) {
		return Error::set("Mesh", "Vertex information is missing");
	}
	if (uv.empty()) {
		return Error::set("Mesh", "UV information is missing");
	}
	if (normal.empty()) {
		return Error::set("Mesh", "Normal information is missing");
	}
	size_t size = vertex.size();
	tangent.resize(size);
	for (int i = 0; i < size; i += 3) {
		Vec3 v1 = vertex[i + 1] - vertex[i];
		Vec3 v2 = vertex[i + 2] - vertex[i];
		Vec2 uv1 = uv[i + 1] - uv[i];
		Vec2 uv2 = uv[i + 2] - uv[i];
		float r = 1 / (uv1.x * uv2.y - uv2.x * uv1.y);
		Vec3 t = (v1 * uv2.y - v2 * uv1.y) * r;
		Vec3 b = (v2 * uv1.x - v1 * uv2.x) * r;
		for (int j = i; j < i + 3; ++j) {
			Vec3& n = normal[j];
			Vec3 ortho_t = (t - n * n.dot(t)).normalize();
			float m = n.cross(ortho_t).dot(b) < 0 ? -1 : 1;
			tangent[j] = Vec4(ortho_t, m);
		}
	}
}

}

/* -------------------------------------------------------------------------- */
/* ---- ink/objects/Image.cxx ----------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

Image::Image(int w, int h, int c, int b) :
width(w), height(h), channel(c), bytes(b) {
	data.resize(w * h * c * b);
}

Image Image::subimage(int x1, int y1, int x2, int y2) {
	if (x1 > x2) std::swap(x1, x2);
	if (y1 > y2) std::swap(y1, y2);
	
	/* check whether the sub-image region is legal */
	if (x1 < 0 || x2 > width || y1 < 0 || y2 > height) {
		Error::set("Image", "Illegal slicing region");
	}
	
	/* create a new image */
	Image image = Image(x2 - x1, y2 - y1, channel, bytes);
	
	/* copy image's data to subimage */
	int bpp = channel * bytes;
	int row_bytes = image.width * bpp;
	uint8_t* data_ptr = data.data();
	uint8_t* image_ptr = image.data.data();
	for (int r = 0; r < image.height; ++r) {
		uint8_t* ptr_1 = data_ptr + (x1 + (r + y1) * width) * bpp;
		uint8_t* ptr_2 = image_ptr + r * row_bytes;
		std::copy_n(ptr_1, row_bytes, ptr_2);
	}
	
	return image; /* return the sub-image */
}

void Image::flip_vertical() {
	int bpp = channel * bytes;
	int row_bytes = width * bpp;
	std::vector<uint8_t> temp(row_bytes);
	uint8_t* temp_ptr = temp.data();
	uint8_t* data_ptr = data.data();
	int median = height / 2;
	for (int y = 0; y < median; ++y) {
		uint8_t* ptr_1 = data_ptr + y * row_bytes;
		uint8_t* ptr_2 = data_ptr + (height - y - 1) * row_bytes;
		std::copy_n(ptr_1, row_bytes, temp_ptr);
		std::copy_n(ptr_2, row_bytes, ptr_1);
		std::copy_n(temp_ptr, row_bytes, ptr_2);
	}
}

void Image::flip_horizontal() {
	int bpp = channel * bytes;
	std::vector<uint8_t> temp(bpp);
	uint8_t* temp_ptr = temp.data();
	uint8_t* data_ptr = data.data();
	int median = width / 2;
	for (int x = 0; x < median; ++x) {
		for (int y = 0; y < height; ++y) {
			uint8_t* ptr_1 = data_ptr + (x + y * width) * bpp;
			uint8_t* ptr_2 = data_ptr + (width - x - 1 + y * width) * bpp;
			std::copy_n(ptr_1, bpp, temp_ptr);
			std::copy_n(ptr_2, bpp, ptr_1);
			std::copy_n(temp_ptr, bpp, ptr_2);
		}
	}
}

std::vector<Image> Image::split() const {
	int bpp = channel * bytes;
	std::vector<Image> images(channel);
	std::vector<uint8_t*> image_ptrs(channel);
	for (int i = 0; i < channel; ++i) {
		images[i] = Image(width, height, 1, bytes);
		image_ptrs[i] = images[i].data.data();
	}
	const uint8_t* data_ptr = data.data();
	int pixel = width * height;
	while (pixel --> 0) {
		const uint8_t* ptr_1 = data_ptr + pixel * bpp;
		for (int i = 0; i < channel; ++i) {
			uint8_t* ptr_2 = image_ptrs[i] + pixel * bytes;
			std::copy_n(ptr_1 + i * bytes, bytes, ptr_2);
		}
	}
	return images;
}

void Image::convert(ColorConversion c) {
	/* check the number of channels */
	if (channel != 3 && channel != 4) {
		return Error::set("Image", "Image's channel must be 3 or 4");
	}
	
	/* convert from RGB color space to BGR color space */
	if (c == COLOR_RGB_TO_BGR) {
		if (bytes == 1) {
			convert_rgb_to_bgr<uint8_t>();
		} else {
			convert_rgb_to_bgr<float_t>();
		}
	}
	
	/* convert from BGR color space to RGB color space */
	else if (c == COLOR_BGR_TO_RGB) {
		if (bytes == 1) {
			convert_bgr_to_rgb<uint8_t>();
		} else {
			convert_bgr_to_rgb<float_t>();
		}
	}
	
	/* convert from RGB color space to SRGB color space */
	else if (c == COLOR_RGB_TO_SRGB) {
		if (bytes == 1) {
			convert_rgb_to_srgb<uint8_t>();
		} else {
			convert_rgb_to_srgb<float_t>();
		}
	}
	
	/* convert from SRGB color space to RGB color space */
	else if (c == COLOR_SRGB_TO_RGB) {
		if (bytes == 1) {
			convert_srgb_to_rgb<uint8_t>();
		} else {
			convert_srgb_to_rgb<float_t>();
		}
	}
	
	/* convert from RGB color space to XYZ color space */
	else if (c == COLOR_RGB_TO_XYZ) {
		if (bytes == 1) {
			convert_rgb_to_xyz<uint8_t>();
		} else {
			convert_rgb_to_xyz<float_t>();
		}
	}
	
	/* convert from XYZ color space to RGB color space */
	else if (c == COLOR_XYZ_TO_RGB) {
		if (bytes == 1) {
			convert_xyz_to_rgb<uint8_t>();
		} else {
			convert_xyz_to_rgb<float_t>();
		}
	}
	
	/* convert from RGB color space to HSV color space */
	else if (c == COLOR_RGB_TO_HSV) {
		if (bytes == 1) {
			convert_rgb_to_hsv<uint8_t>();
		} else {
			convert_rgb_to_hsv<float_t>();
		}
	}
	
	/* convert from HSV color space to RGB color space */
	else if (c == COLOR_HSV_TO_RGB) {
		if (bytes == 1) {
			convert_hsv_to_rgb<uint8_t>();
		} else {
			convert_hsv_to_rgb<float_t>();
		}
	}
	
	/* convert from RGB color space to HSL color space */
	else if (c == COLOR_RGB_TO_HSL) {
		if (bytes == 1) {
			convert_rgb_to_hsl<uint8_t>();
		} else {
			convert_rgb_to_hsl<float_t>();
		}
	}
	
	/* convert from HSL color space to RGB color space */
	else if (c == COLOR_HSL_TO_RGB) {
		if (bytes == 1) {
			convert_hsl_to_rgb<uint8_t>();
		} else {
			convert_hsl_to_rgb<float_t>();
		}
	}
	
	/* convert from RGB color space to HCY color space */
	else if (c == COLOR_RGB_TO_HCY) {
		if (bytes == 1) {
			convert_rgb_to_hcy<uint8_t>();
		} else {
			convert_rgb_to_hcy<float_t>();
		}
	}
	
	/* convert from HCY color space to RGB color space */
	else if (c == COLOR_HCY_TO_RGB) {
		if (bytes == 1) {
			convert_hcy_to_rgb<uint8_t>();
		} else {
			convert_hcy_to_rgb<float_t>();
		}
	}
}

template <typename Type>
void Image::convert_rgb_to_bgr() {
	/* get pointers of the specified data type */
	Type* ptr_r = reinterpret_cast<Type*>(data.data());
	Type* ptr_b = ptr_r + 2;
	
	/* swap the red and blue channel */
	int i = width * height;
	while (i --> 0) {
		std::swap(ptr_r[channel * i], ptr_b[channel * i]);
	}
}

template <typename Type>
void Image::convert_bgr_to_rgb() {
	/* get pointers of the specified data type */
	Type* ptr_r = reinterpret_cast<Type*>(data.data());
	Type* ptr_b = ptr_r + 2;
	
	/* swap the red and blue channel */
	int i = width * height;
	while (i --> 0) {
		std::swap(ptr_r[channel * i], ptr_b[channel * i]);
	}
}

template <typename Type>
void Image::convert_rgb_to_srgb() {
	/* get pointer of the specified data type */
	Type* ptr_r = reinterpret_cast<Type*>(data.data());
	Type* ptr_g = ptr_r + 1;
	Type* ptr_b = ptr_g + 1;
	
	/* convert colorspace */
	Vec3 color;
	int i = width * height;
	while (i --> 0) {
		/* unpack RGB information from data */
		color.x = unpack<Type>(ptr_r[channel * i]);
		color.y = unpack<Type>(ptr_g[channel * i]);
		color.z = unpack<Type>(ptr_b[channel * i]);
		
		/* convert to SRGB color space */
		color = Color::rgb_to_srgb(color);
		
		/* pack SRGB color to data */
		ptr_r[channel * i] = pack<Type>(color.x);
		ptr_g[channel * i] = pack<Type>(color.y);
		ptr_b[channel * i] = pack<Type>(color.z);
	}
}

template <typename Type>
void Image::convert_srgb_to_rgb() {
	/* get pointer of the specified data type */
	Type* ptr_r = reinterpret_cast<Type*>(data.data());
	Type* ptr_g = ptr_r + 1;
	Type* ptr_b = ptr_g + 1;
	
	/* convert colorspace */
	Vec3 color;
	int i = width * height;
	while (i --> 0) {
		/* unpack SRGB information from data */
		color.x = unpack<Type>(ptr_r[channel * i]);
		color.y = unpack<Type>(ptr_g[channel * i]);
		color.z = unpack<Type>(ptr_b[channel * i]);
		
		/* convert to RGB color space */
		color = Color::srgb_to_rgb(color);
		
		/* pack RGB color to data */
		ptr_r[channel * i] = pack<Type>(color.x);
		ptr_g[channel * i] = pack<Type>(color.y);
		ptr_b[channel * i] = pack<Type>(color.z);
	}
}

template <typename Type>
void Image::convert_rgb_to_xyz() {
	/* get pointers of the specified data type */
	Type* ptr_r = reinterpret_cast<Type*>(data.data());
	Type* ptr_g = ptr_r + 1;
	Type* ptr_b = ptr_g + 1;
	
	/* convert colorspace */
	Vec3 color;
	int i = width * height;
	while (i --> 0) {
		/* unpack RGB information from data */
		color.x = unpack<Type>(ptr_r[channel * i]);
		color.y = unpack<Type>(ptr_g[channel * i]);
		color.z = unpack<Type>(ptr_b[channel * i]);
		
		/* convert to XYZ color space */
		color = Color::rgb_to_xyz(color);
		
		/* pack XYZ color to data */
		ptr_r[channel * i] = pack<Type>(color.x);
		ptr_g[channel * i] = pack<Type>(color.y);
		ptr_b[channel * i] = pack<Type>(color.z);
	}
}

template <typename Type>
void Image::convert_xyz_to_rgb() {
	/* get pointers of the specified data type */
	Type* ptr_r = reinterpret_cast<Type*>(data.data());
	Type* ptr_g = ptr_r + 1;
	Type* ptr_b = ptr_g + 1;
	
	/* convert colorspace */
	Vec3 color;
	int i = width * height;
	while (i --> 0) {
		/* unpack XYZ information from data */
		color.x = unpack<Type>(ptr_r[channel * i]);
		color.y = unpack<Type>(ptr_g[channel * i]);
		color.z = unpack<Type>(ptr_b[channel * i]);
		
		/* convert to RGB color space */
		color = Color::xyz_to_rgb(color);
		
		/* pack RGB color to data */
		ptr_r[channel * i] = pack<Type>(color.x);
		ptr_g[channel * i] = pack<Type>(color.y);
		ptr_b[channel * i] = pack<Type>(color.z);
	}
}

template <typename Type>
void Image::convert_rgb_to_hsv() {
	/* get pointers of the specified data type */
	Type* ptr_r = reinterpret_cast<Type*>(data.data());
	Type* ptr_g = ptr_r + 1;
	Type* ptr_b = ptr_g + 1;
	
	/* convert colorspace */
	Vec3 color;
	int i = width * height;
	while (i --> 0) {
		/* unpack RGB information from data */
		color.x = unpack<Type>(ptr_r[channel * i]);
		color.y = unpack<Type>(ptr_g[channel * i]);
		color.z = unpack<Type>(ptr_b[channel * i]);
		
		/* convert to HSV color space */
		color = Color::rgb_to_hsv(color);
		
		/* pack HSV color to data */
		ptr_r[channel * i] = pack<Type>(color.x);
		ptr_g[channel * i] = pack<Type>(color.y);
		ptr_b[channel * i] = pack<Type>(color.z);
	}
}

template <typename Type>
void Image::convert_hsv_to_rgb() {
	/* get pointers of the specified data type */
	Type* ptr_r = reinterpret_cast<Type*>(data.data());
	Type* ptr_g = ptr_r + 1;
	Type* ptr_b = ptr_g + 1;
	
	/* convert colorspace */
	Vec3 color;
	int i = width * height;
	while (i --> 0) {
		/* unpack HSV information from data */
		color.x = unpack<Type>(ptr_r[channel * i]);
		color.y = unpack<Type>(ptr_g[channel * i]);
		color.z = unpack<Type>(ptr_b[channel * i]);
		
		/* convert to RGB color space */
		color = Color::hsv_to_rgb(color);
		
		/* pack RGB color to data */
		ptr_r[channel * i] = pack<Type>(color.x);
		ptr_g[channel * i] = pack<Type>(color.y);
		ptr_b[channel * i] = pack<Type>(color.z);
	}
}

template <typename Type>
void Image::convert_rgb_to_hsl() {
	/* get pointers of the specified data type */
	Type* ptr_r = reinterpret_cast<Type*>(data.data());
	Type* ptr_g = ptr_r + 1;
	Type* ptr_b = ptr_g + 1;
	
	/* convert colorspace */
	Vec3 color;
	int i = width * height;
	while (i --> 0) {
		/* unpack RGB information from data */
		color.x = unpack<Type>(ptr_r[channel * i]);
		color.y = unpack<Type>(ptr_g[channel * i]);
		color.z = unpack<Type>(ptr_b[channel * i]);
		
		/* convert to HSL color space */
		color = Color::rgb_to_hsl(color);
		
		/* pack HSL color to data */
		ptr_r[channel * i] = pack<Type>(color.x);
		ptr_g[channel * i] = pack<Type>(color.y);
		ptr_b[channel * i] = pack<Type>(color.z);
	}
}

template <typename Type>
void Image::convert_hsl_to_rgb() {
	/* get pointers of the specified data type */
	Type* ptr_r = reinterpret_cast<Type*>(data.data());
	Type* ptr_g = ptr_r + 1;
	Type* ptr_b = ptr_g + 1;
	
	/* convert colorspace */
	Vec3 color;
	int i = width * height;
	while (i --> 0) {
		/* unpack HSL information from data */
		color.x = unpack<Type>(ptr_r[channel * i]);
		color.y = unpack<Type>(ptr_g[channel * i]);
		color.z = unpack<Type>(ptr_b[channel * i]);
		
		/* convert to RGB color space */
		color = Color::hsl_to_rgb(color);
		
		/* pack RGB color to data */
		ptr_r[channel * i] = pack<Type>(color.x);
		ptr_g[channel * i] = pack<Type>(color.y);
		ptr_b[channel * i] = pack<Type>(color.z);
	}
}

template <typename Type>
void Image::convert_rgb_to_hcy() {
	/* get pointers of the specified data type */
	Type* ptr_r = reinterpret_cast<Type*>(data.data());
	Type* ptr_g = ptr_r + 1;
	Type* ptr_b = ptr_g + 1;
	
	/* convert colorspace */
	Vec3 color;
	int i = width * height;
	while (i --> 0) {
		/* unpack RGB information from data */
		color.x = unpack<Type>(ptr_r[channel * i]);
		color.y = unpack<Type>(ptr_g[channel * i]);
		color.z = unpack<Type>(ptr_b[channel * i]);
		
		/* convert to HCY color space */
		color = Color::rgb_to_hcy(color);
		
		/* pack HCY color to data */
		ptr_r[channel * i] = pack<Type>(color.x);
		ptr_g[channel * i] = pack<Type>(color.y);
		ptr_b[channel * i] = pack<Type>(color.z);
	}
}

template <typename Type>
void Image::convert_hcy_to_rgb() {
	/* get pointers of the specified data type */
	Type* ptr_r = reinterpret_cast<Type*>(data.data());
	Type* ptr_g = ptr_r + 1;
	Type* ptr_b = ptr_g + 1;
	
	/* convert colorspace */
	Vec3 color;
	int i = width * height;
	while (i --> 0) {
		/* unpack HCY information from data */
		color.x = unpack<Type>(ptr_r[channel * i]);
		color.y = unpack<Type>(ptr_g[channel * i]);
		color.z = unpack<Type>(ptr_b[channel * i]);
		
		/* convert to RGB color space */
		color = Color::hcy_to_rgb(color);
		
		/* pack RGB color to data */
		ptr_r[channel * i] = pack<Type>(color.x);
		ptr_g[channel * i] = pack<Type>(color.y);
		ptr_b[channel * i] = pack<Type>(color.z);
	}
}

template <typename Type>
float Image::unpack(Type v) {
	if constexpr (std::is_same_v<Type, float>) return v;
	return v / 255.f;
}

template <typename Type>
Type Image::pack(float v) {
	if constexpr (std::is_same_v<Type, float>) return v;
	return v < 0 ? 0 : v > 1 ? 255 : roundf(v * 255.f);
}

}

/* -------------------------------------------------------------------------- */
/* ---- ink/objects/Material.cxx -------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

Material::Material(const std::string& n) : name(n) {
	custom_maps[0]  = nullptr;
	custom_maps[1]  = nullptr;
	custom_maps[2]  = nullptr;
	custom_maps[3]  = nullptr;
	custom_maps[4]  = nullptr;
	custom_maps[5]  = nullptr;
	custom_maps[6]  = nullptr;
	custom_maps[7]  = nullptr;
	custom_maps[8]  = nullptr;
	custom_maps[9]  = nullptr;
	custom_maps[10] = nullptr;
	custom_maps[11] = nullptr;
	custom_maps[12] = nullptr;
	custom_maps[13] = nullptr;
	custom_maps[14] = nullptr;
	custom_maps[15] = nullptr;
	side            = FRONT_SIDE;
	shadow_side     = BACK_SIDE;
	depth_func      = FUNC_LEQUAL;
	stencil_func    = FUNC_ALWAYS;
	stencil_fail    = STENCIL_KEEP;
	stencil_zfail   = STENCIL_KEEP;
	stencil_zpass   = STENCIL_KEEP;
	blend_op_rgb    = BLEND_ADD;
	blend_op_alpha  = BLEND_ADD;
	blend_src_rgb   = FACTOR_SRC_ALPHA;
	blend_src_alpha = FACTOR_SRC_ALPHA;
	blend_dst_rgb   = FACTOR_ONE_MINUS_SRC_ALPHA;
	blend_dst_alpha = FACTOR_ONE_MINUS_SRC_ALPHA;
}

}

/* -------------------------------------------------------------------------- */
/* ---- ink/objects/Instance.cxx -------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

Instance::Instance(const std::string& n) : name(n) {}

void Instance::add(Instance* i) {
	i->parent = this;
	children.emplace_back(i);
}

void Instance::add(const std::initializer_list<Instance*>& l) {
	for (auto& instance : l) {
		instance->parent = this;
	}
	children.insert(children.end(), l);
}

void Instance::remove(Instance* i) {
	i->parent = nullptr;
	size_t size = children.size();
	for (int c = 0; c < size; ++c) {
		if (children[c] == i) {
			children.erase(children.begin() + c);
		}
	}
}

void Instance::remove(const std::initializer_list<Instance*>& l) {
	size_t size = children.size();
	for (auto& instance : l) {
		instance->parent = nullptr;
		for (int c = 0; c < size; ++c) {
			if (children[c] == instance) {
				children.erase(children.begin() + c);
			}
		}
	}
}

void Instance::clear() {
	children.clear();
}

Instance* Instance::get_child(int i) const {
	return children[i];
}

Instance* Instance::get_child(const std::string& n) const {
	size_t size = children.size();
	for (int c = 0; c < size; ++c) {
		if (children[c]->name == n) return children[c];
	}
	return nullptr;
}

size_t Instance::get_child_count() const {
	return children.size();
}

Instance* Instance::get_parent() const {
	return parent;
}

void Instance::set_transform(const Vec3& p, const Euler& r, const Vec3& s) {
	position = p;
	rotation = r;
	scale = s;
}

void Instance::update_matrix_local() {
	matrix_local = transform();
}

void Instance::update_matrix_global() {
	matrix_global = transform_global();
}

Vec3 Instance::global_to_local(const Vec3& v) const {
	return inverse_4x4(matrix_global) * Vec4(v, 1);
}

Vec3 Instance::local_to_global(const Vec3& v) const {
	return matrix_global * Vec4(v, 1);
}

Mat4 Instance::transform() const {
	return transform(position, rotation, scale);
}

Mat4 Instance::transform_global() const {
	const Instance* instance = this;
	Mat4 matrix = Mat4::identity();
	while (instance->parent != nullptr) {
		matrix = instance->transform() * matrix;
		instance = instance->parent;
	}
	return matrix;
}

Mat4 Instance::transform(const Vec3& p, const Euler& r, const Vec3& s) {
	Mat3 m = r.to_rotation_matrix();
	return {
		m[0][0] * s.x, m[0][1] * s.y, m[0][2] * s.z, p.x          ,
		m[1][0] * s.x, m[1][1] * s.y, m[1][2] * s.z, p.y          ,
		m[2][0] * s.x, m[2][1] * s.y, m[2][2] * s.z, p.z          ,
		0            , 0            , 0            , 1            ,
	};
}

}

/* -------------------------------------------------------------------------- */
/* ---- ink/loader/Loader.cxx ----------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

Image Loader::load_image(const std::string& p) {
	/* create a new image */
	Image image = Image(0, 0, 0, 1);
	
	/* get image data from file */
	auto* image_ptr = stbi_load(p.c_str(), &image.width, &image.height, &image.channel, 0);
	if (image_ptr == nullptr) {
		Error::set("Loader", "Failed to read from image");
		return Image();
	}
	
	/* copy image data to new image */
	image.data.resize(image.width * image.height * image.channel * image.bytes);
	memcpy(image.data.data(), image_ptr, image.data.size());
	
	/* release image data */
	stbi_image_free(image_ptr);
	return image;
}

Image Loader::load_image_hdr(const std::string& p) {
	/* create a new image */
	Image image = Image(0, 0, 0, 4);
	
	/* get image data from file */
	auto* image_ptr = stbi_loadf(p.c_str(), &image.width, &image.height, &image.channel, 0);
	if (image_ptr == nullptr) {
		Error::set("Loader", "Failed to read from image");
		return Image();
	}
	
	/* copy image data to new image */
	image.data.resize(image.width * image.height * image.channel * image.bytes);
	memcpy(image.data.data(), image_ptr, image.data.size());
	
	/* release image data */
	stbi_image_free(image_ptr);
	return image;
}

LoadObject Loader::load_mtl(const std::string& p) {
	/* prepare the file stream */
	std::ifstream stream;
	stream.open(p, std::ifstream::in);
	if (stream.fail()) {
		Error::set("Loader", "Failed to read from mtl file");
		return LoadObject();
	}
	size_t streamsize_max = std::numeric_limits<std::streamsize>::max();
	
	/* initialize load object */
	LoadObject object;
	Material* current_material = nullptr;
	
	/* read data by line */
	while (!stream.eof()) {
		std::string keyword;
		stream >> keyword;
		
		/* create a new material */
		if (keyword == "newmtl") {
			std::string name;
			stream >> name;
			current_material = &object.materials.emplace_back(Material(name));
		}
		
		/* material is not declared */
		else if (current_material == nullptr) {}
		
		/* Kd: diffuse color */
		else if (keyword == "Kd") {
			Vec3 kd;
			stream >> kd.x >> kd.y >> kd.z;
			current_material->color = kd;
		}
		
		/* Ke: emissive color */
		else if (keyword == "Ke") {
			Vec3 ke;
			stream >> ke.x >> ke.y >> ke.z;
			current_material->emissive = ke;
		}
		
		/* d: dissolve factor */
		else if (keyword == "d") {
			float d;
			stream >> d;
			current_material->alpha = d;
		}
		
		/* tr: transparency factor */
		else if (keyword == "tr") {
			float tr;
			stream >> tr;
			current_material->alpha = 1 - tr;
		}
		
		/* ignore unknown keyword */
		else {
			stream.ignore(streamsize_max, '\n');
		}
	}
	
	/* close file stream */
	stream.close();
	
	/* return the load object */
	return object;
}

LoadObject Loader::load_obj(const std::string& p, const ObjOptions& o) {
	/* prepare the file stream */
	std::ifstream stream;
	stream.open(p, std::ifstream::in);
	if (stream.fail()) {
		Error::set("Loader", "Failed to read from obj file");
		return LoadObject();
	}
	size_t streamsize_max = std::numeric_limits<std::streamsize>::max();
	
	/* temporary data */
	std::vector<Vec3> vertex;
	std::vector<Vec3> normal;
	std::vector<Vec2> uv;
	std::vector<Vec3> color;
	
	/* initialize load object */
	LoadObject object;
	Mesh* current_mesh = &object.meshes.emplace_back(Mesh("default"));
	
	/* initialize mesh group pointer */
	current_mesh->groups.emplace_back<MeshGroup>({"default", 0, 0});
	MeshGroup* current_group = &current_mesh->groups.back();
	
	/* initialize total length */
	int total_length = 0;
	
	/* read data by line */
	while (!stream.eof()) {
		std::string keyword;
		stream >> keyword;
		
		/* add vertex to temporary array */
		if (keyword == "v") {
			Vec3 v;
			stream >> v.x >> v.y >> v.z;
			vertex.emplace_back(v);
			if (o.vertex_color) {
				stream >> v.x >> v.y >> v.z;
				color.emplace_back(v);
			}
		}
		
		/* add normal to temporary array */
		else if (keyword == "vn") {
			Vec3 vn;
			stream >> vn.x >> vn.y >> vn.z;
			normal.emplace_back(vn);
		}
		
		/* add uv to temporary array */
		else if (keyword == "vt") {
			Vec2 vt;
			stream >> vt.x >> vt.y;
			uv.emplace_back(vt);
		}
		
		/* search for data by index and add it to current mesh */
		else if (keyword == "f") {
			int index = 0;
			for (int i = 0; i < 3; ++i) {
				stream >> index;
				current_mesh->vertex.emplace_back(vertex[index - 1]);
				if (o.vertex_color) {
					current_mesh->color.emplace_back(color[index - 1]);
				}
				
				/* check whether uv is omitted */
				if (stream.peek() != '/') continue;
				stream.get();
				if (std::isdigit(stream.peek())) {
					stream >> index;
					current_mesh->uv.emplace_back(uv[index - 1]);
				}
				
				/* check whether normal is omitted */
				if (stream.peek() != '/') continue;
				stream.get();
				stream >> index;
				current_mesh->normal.emplace_back(normal[index - 1]);
			}
			
			/* increase the length of current_material */
			current_group->length += 3;
			total_length += 3;
		}
		
		/* create new mesh object and initialize everything */
		else if (keyword == o.group) {
			std::string name;
			stream >> name;
			
			/* if current mesh has no data, replace its name */
			if (current_mesh->vertex.empty()) {
				current_mesh->name = name;
				continue;
			}
			
			/* initialize total length */
			total_length = 0;
			
			/* create new mesh object */
			current_mesh = &object.meshes.emplace_back(Mesh(name));
			
			/* create new mesh group */
			current_group = &current_mesh->groups.emplace_back(MeshGroup{name, total_length, 0});
		}
		
		/* create new mesh group */
		else if (keyword == "usemtl") {
			std::string name;
			stream >> name;
			
			/* if current mesh group has no data, replace its name */
			if (current_group->length == 0) {
				current_group->name = name;
				continue;
			}
			
			/* create new mesh group */
			current_group = &current_mesh->groups.emplace_back(MeshGroup{name, total_length, 0});
		}
		
		/* ignore unknown keyword */
		else {
			stream.ignore(streamsize_max, '\n');
		}
	}
	
	/* close file stream */
	stream.close();
	
	/* return the load object */
	return object;
}

}

/* -------------------------------------------------------------------------- */
/* ---- ink/camera/Camera.cxx ----------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

bool Camera::is_perspective() const {
	return projection[3][2] == -1;
}

void Camera::lookat(const Vec3& p, const Vec3& d, const Vec3& u) {
	position = p;
	direction = d.normalize();
	up = u.normalize();
	Vec3& dn = direction;
	Vec3& un = up;
	Vec3 rn = -dn.cross(un).normalize();
	viewing = {
		rn.x      , rn.y      , rn.z      , -p.dot(rn),
		un.x      , un.y      , un.z      , -p.dot(un),
		dn.x      , dn.y      , dn.z      , -p.dot(dn),
		0         , 0         , 0         , 1         ,
	};
}

}

/* -------------------------------------------------------------------------- */
/* ---- ink/camera/PerspCamera.cxx ------------------------------------------ */
/* -------------------------------------------------------------------------- */

namespace Ink {

PerspCamera::PerspCamera(float fov, float a, float n, float f) {
	set(fov, a, n, f);
}

void PerspCamera::set(float fov, float a, float n, float f) {
	fov_y = fov;
	aspect = a;
	near = n;
	far = f;
	projection = {
		1 / tanf(fov / 2) / a, 0                    , 0                    , 0                    ,
		0                    , 1 / tanf(fov / 2)    , 0                    , 0                    ,
		0                    , 0                    , (n + f) / (n - f)    , 2 * f * n / (n - f)  ,
		0                    , 0                    , -1                   , 0                    ,
	};
}

}

/* -------------------------------------------------------------------------- */
/* ---- ink/camera/OrthoCamera.cxx ------------------------------------------ */
/* -------------------------------------------------------------------------- */

namespace Ink {

OrthoCamera::OrthoCamera(float lv, float rv, float lh, float uh, float n, float f) {
	set(lv, rv, lh, uh, n, f);
}

void OrthoCamera::set(float lv, float rv, float lh, float uh, float n, float f) {
	left = lv;
	right = rv;
	lower = lh;
	upper = uh;
	far = f;
	near = n;
	projection = {
		2 / (rv - lv)        , 0                    , 0                    , (rv + lv) / (lv - rv),
		0                    , 2 / (uh - lh)        , 0                    , (uh + lh) / (lh - uh),
		0                    , 0                    , 2 / (n - f)          , (f + n) / (n - f)    ,
		0                    , 0                    , 0                    , 1                    ,
	};
}

}

/* -------------------------------------------------------------------------- */
/* ---- ink/meshes/BoxMesh.cxx ---------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

Mesh BoxMesh::create() {
	Mesh mesh = Mesh("Box");
	mesh.groups = {{"default", 0, 36}};
	mesh.vertex = vertex;
	mesh.uv = uv;
	mesh.normal = normal;
	return mesh;
}

std::vector<Vec3> BoxMesh::vertex = {
{ 0.500000000,  0.500000000,  0.500000000},
{ 0.500000000, -0.500000000,  0.500000000},
{ 0.500000000,  0.500000000, -0.500000000},
{ 0.500000000, -0.500000000,  0.500000000},
{ 0.500000000, -0.500000000, -0.500000000},
{ 0.500000000,  0.500000000, -0.500000000},
{-0.500000000,  0.500000000, -0.500000000},
{-0.500000000, -0.500000000, -0.500000000},
{-0.500000000,  0.500000000,  0.500000000},
{-0.500000000, -0.500000000, -0.500000000},
{-0.500000000, -0.500000000,  0.500000000},
{-0.500000000,  0.500000000,  0.500000000},
{-0.500000000,  0.500000000, -0.500000000},
{-0.500000000,  0.500000000,  0.500000000},
{ 0.500000000,  0.500000000, -0.500000000},
{-0.500000000,  0.500000000,  0.500000000},
{ 0.500000000,  0.500000000,  0.500000000},
{ 0.500000000,  0.500000000, -0.500000000},
{-0.500000000, -0.500000000,  0.500000000},
{-0.500000000, -0.500000000, -0.500000000},
{ 0.500000000, -0.500000000,  0.500000000},
{-0.500000000, -0.500000000, -0.500000000},
{ 0.500000000, -0.500000000, -0.500000000},
{ 0.500000000, -0.500000000,  0.500000000},
{-0.500000000,  0.500000000,  0.500000000},
{-0.500000000, -0.500000000,  0.500000000},
{ 0.500000000,  0.500000000,  0.500000000},
{-0.500000000, -0.500000000,  0.500000000},
{ 0.500000000, -0.500000000,  0.500000000},
{ 0.500000000,  0.500000000,  0.500000000},
{ 0.500000000,  0.500000000, -0.500000000},
{ 0.500000000, -0.500000000, -0.500000000},
{-0.500000000,  0.500000000, -0.500000000},
{ 0.500000000, -0.500000000, -0.500000000},
{-0.500000000, -0.500000000, -0.500000000},
{-0.500000000,  0.500000000, -0.500000000},
};

std::vector<Vec2> BoxMesh::uv = {
{ 0.000000000,  1.000000000},
{ 0.000000000,  0.000000000},
{ 1.000000000,  1.000000000},
{ 0.000000000,  0.000000000},
{ 1.000000000,  0.000000000},
{ 1.000000000,  1.000000000},
{ 0.000000000,  1.000000000},
{ 0.000000000,  0.000000000},
{ 1.000000000,  1.000000000},
{ 0.000000000,  0.000000000},
{ 1.000000000,  0.000000000},
{ 1.000000000,  1.000000000},
{ 0.000000000,  1.000000000},
{ 0.000000000,  0.000000000},
{ 1.000000000,  1.000000000},
{ 0.000000000,  0.000000000},
{ 1.000000000,  0.000000000},
{ 1.000000000,  1.000000000},
{ 0.000000000,  1.000000000},
{ 0.000000000,  0.000000000},
{ 1.000000000,  1.000000000},
{ 0.000000000,  0.000000000},
{ 1.000000000,  0.000000000},
{ 1.000000000,  1.000000000},
{ 0.000000000,  1.000000000},
{ 0.000000000,  0.000000000},
{ 1.000000000,  1.000000000},
{ 0.000000000,  0.000000000},
{ 1.000000000,  0.000000000},
{ 1.000000000,  1.000000000},
{ 0.000000000,  1.000000000},
{ 0.000000000,  0.000000000},
{ 1.000000000,  1.000000000},
{ 0.000000000,  0.000000000},
{ 1.000000000,  0.000000000},
{ 1.000000000,  1.000000000},
};

std::vector<Vec3> BoxMesh::normal = {
{ 1.000000000,  0.000000000,  0.000000000},
{ 1.000000000,  0.000000000,  0.000000000},
{ 1.000000000,  0.000000000,  0.000000000},
{ 1.000000000,  0.000000000,  0.000000000},
{ 1.000000000,  0.000000000,  0.000000000},
{ 1.000000000,  0.000000000,  0.000000000},
{-1.000000000,  0.000000000,  0.000000000},
{-1.000000000,  0.000000000,  0.000000000},
{-1.000000000,  0.000000000,  0.000000000},
{-1.000000000,  0.000000000,  0.000000000},
{-1.000000000,  0.000000000,  0.000000000},
{-1.000000000,  0.000000000,  0.000000000},
{ 0.000000000,  1.000000000,  0.000000000},
{ 0.000000000,  1.000000000,  0.000000000},
{ 0.000000000,  1.000000000,  0.000000000},
{ 0.000000000,  1.000000000,  0.000000000},
{ 0.000000000,  1.000000000,  0.000000000},
{ 0.000000000,  1.000000000,  0.000000000},
{ 0.000000000, -1.000000000,  0.000000000},
{ 0.000000000, -1.000000000,  0.000000000},
{ 0.000000000, -1.000000000,  0.000000000},
{ 0.000000000, -1.000000000,  0.000000000},
{ 0.000000000, -1.000000000,  0.000000000},
{ 0.000000000, -1.000000000,  0.000000000},
{ 0.000000000,  0.000000000,  1.000000000},
{ 0.000000000,  0.000000000,  1.000000000},
{ 0.000000000,  0.000000000,  1.000000000},
{ 0.000000000,  0.000000000,  1.000000000},
{ 0.000000000,  0.000000000,  1.000000000},
{ 0.000000000,  0.000000000,  1.000000000},
{ 0.000000000,  0.000000000, -1.000000000},
{ 0.000000000,  0.000000000, -1.000000000},
{ 0.000000000,  0.000000000, -1.000000000},
{ 0.000000000,  0.000000000, -1.000000000},
{ 0.000000000,  0.000000000, -1.000000000},
{ 0.000000000,  0.000000000, -1.000000000},
};

}

/* -------------------------------------------------------------------------- */
/* ---- ink/meshes/PlaneMesh.cxx -------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

Mesh PlaneMesh::create() {
	Mesh mesh = Mesh("Plane");
	mesh.groups = {{"default", 0, 6}};
	mesh.vertex = vertex;
	mesh.uv = uv;
	mesh.normal = normal;
	return mesh;
}

std::vector<Vec3> PlaneMesh::vertex = {
{-0.500000000,  0.500000000,  0.000000000},
{-0.500000000, -0.500000000,  0.000000000},
{ 0.500000000,  0.500000000,  0.000000000},
{-0.500000000, -0.500000000,  0.000000000},
{ 0.500000000, -0.500000000,  0.000000000},
{ 0.500000000,  0.500000000,  0.000000000},
};

std::vector<Vec2> PlaneMesh::uv = {
{ 0.000000000,  1.000000000},
{ 0.000000000,  0.000000000},
{ 1.000000000,  1.000000000},
{ 0.000000000,  0.000000000},
{ 1.000000000,  0.000000000},
{ 1.000000000,  1.000000000},
};

std::vector<Vec3> PlaneMesh::normal = {
{ 0.000000000,  0.000000000,  1.000000000},
{ 0.000000000,  0.000000000,  1.000000000},
{ 0.000000000,  0.000000000,  1.000000000},
{ 0.000000000,  0.000000000,  1.000000000},
{ 0.000000000,  0.000000000,  1.000000000},
{ 0.000000000,  0.000000000,  1.000000000},
};

}

/* -------------------------------------------------------------------------- */
/* ---- ink/meshes/CylinderMesh.cxx ----------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

Mesh CylinderMesh::create() {
	Mesh mesh = Mesh("Cylinder");
	mesh.groups = {{"default", 0, 192}};
	mesh.vertex = vertex;
	mesh.uv = uv;
	mesh.normal = normal;
	return mesh;
}

std::vector<Vec3> CylinderMesh::vertex = {
{ 0.000000000,  0.500000000,  0.500000000},
{ 0.000000000, -0.500000000,  0.500000000},
{ 0.191341713,  0.500000000,  0.461939752},
{ 0.000000000, -0.500000000,  0.500000000},
{ 0.191341713, -0.500000000,  0.461939752},
{ 0.191341713,  0.500000000,  0.461939752},
{ 0.191341713,  0.500000000,  0.461939752},
{ 0.191341713, -0.500000000,  0.461939752},
{ 0.353553385,  0.500000000,  0.353553385},
{ 0.191341713, -0.500000000,  0.461939752},
{ 0.353553385, -0.500000000,  0.353553385},
{ 0.353553385,  0.500000000,  0.353553385},
{ 0.353553385,  0.500000000,  0.353553385},
{ 0.353553385, -0.500000000,  0.353553385},
{ 0.461939752,  0.500000000,  0.191341713},
{ 0.353553385, -0.500000000,  0.353553385},
{ 0.461939752, -0.500000000,  0.191341713},
{ 0.461939752,  0.500000000,  0.191341713},
{ 0.461939752,  0.500000000,  0.191341713},
{ 0.461939752, -0.500000000,  0.191341713},
{ 0.500000000,  0.500000000,  0.000000000},
{ 0.461939752, -0.500000000,  0.191341713},
{ 0.500000000, -0.500000000,  0.000000000},
{ 0.500000000,  0.500000000,  0.000000000},
{ 0.500000000,  0.500000000,  0.000000000},
{ 0.500000000, -0.500000000,  0.000000000},
{ 0.461939752,  0.500000000, -0.191341713},
{ 0.500000000, -0.500000000,  0.000000000},
{ 0.461939752, -0.500000000, -0.191341713},
{ 0.461939752,  0.500000000, -0.191341713},
{ 0.461939752,  0.500000000, -0.191341713},
{ 0.461939752, -0.500000000, -0.191341713},
{ 0.353553385,  0.500000000, -0.353553385},
{ 0.461939752, -0.500000000, -0.191341713},
{ 0.353553385, -0.500000000, -0.353553385},
{ 0.353553385,  0.500000000, -0.353553385},
{ 0.353553385,  0.500000000, -0.353553385},
{ 0.353553385, -0.500000000, -0.353553385},
{ 0.191341713,  0.500000000, -0.461939752},
{ 0.353553385, -0.500000000, -0.353553385},
{ 0.191341713, -0.500000000, -0.461939752},
{ 0.191341713,  0.500000000, -0.461939752},
{ 0.191341713,  0.500000000, -0.461939752},
{ 0.191341713, -0.500000000, -0.461939752},
{ 0.000000000,  0.500000000, -0.500000000},
{ 0.191341713, -0.500000000, -0.461939752},
{ 0.000000000, -0.500000000, -0.500000000},
{ 0.000000000,  0.500000000, -0.500000000},
{ 0.000000000,  0.500000000, -0.500000000},
{ 0.000000000, -0.500000000, -0.500000000},
{-0.191341713,  0.500000000, -0.461939752},
{ 0.000000000, -0.500000000, -0.500000000},
{-0.191341713, -0.500000000, -0.461939752},
{-0.191341713,  0.500000000, -0.461939752},
{-0.191341713,  0.500000000, -0.461939752},
{-0.191341713, -0.500000000, -0.461939752},
{-0.353553385,  0.500000000, -0.353553385},
{-0.191341713, -0.500000000, -0.461939752},
{-0.353553385, -0.500000000, -0.353553385},
{-0.353553385,  0.500000000, -0.353553385},
{-0.353553385,  0.500000000, -0.353553385},
{-0.353553385, -0.500000000, -0.353553385},
{-0.461939752,  0.500000000, -0.191341713},
{-0.353553385, -0.500000000, -0.353553385},
{-0.461939752, -0.500000000, -0.191341713},
{-0.461939752,  0.500000000, -0.191341713},
{-0.461939752,  0.500000000, -0.191341713},
{-0.461939752, -0.500000000, -0.191341713},
{-0.500000000,  0.500000000, -0.000000000},
{-0.461939752, -0.500000000, -0.191341713},
{-0.500000000, -0.500000000, -0.000000000},
{-0.500000000,  0.500000000, -0.000000000},
{-0.500000000,  0.500000000, -0.000000000},
{-0.500000000, -0.500000000, -0.000000000},
{-0.461939752,  0.500000000,  0.191341713},
{-0.500000000, -0.500000000, -0.000000000},
{-0.461939752, -0.500000000,  0.191341713},
{-0.461939752,  0.500000000,  0.191341713},
{-0.461939752,  0.500000000,  0.191341713},
{-0.461939752, -0.500000000,  0.191341713},
{-0.353553385,  0.500000000,  0.353553385},
{-0.461939752, -0.500000000,  0.191341713},
{-0.353553385, -0.500000000,  0.353553385},
{-0.353553385,  0.500000000,  0.353553385},
{-0.353553385,  0.500000000,  0.353553385},
{-0.353553385, -0.500000000,  0.353553385},
{-0.191341713,  0.500000000,  0.461939752},
{-0.353553385, -0.500000000,  0.353553385},
{-0.191341713, -0.500000000,  0.461939752},
{-0.191341713,  0.500000000,  0.461939752},
{-0.191341713,  0.500000000,  0.461939752},
{-0.191341713, -0.500000000,  0.461939752},
{-0.000000000,  0.500000000,  0.500000000},
{-0.191341713, -0.500000000,  0.461939752},
{-0.000000000, -0.500000000,  0.500000000},
{-0.000000000,  0.500000000,  0.500000000},
{ 0.000000000,  0.500000000,  0.500000000},
{ 0.191341713,  0.500000000,  0.461939752},
{ 0.000000000,  0.500000000,  0.000000000},
{ 0.191341713,  0.500000000,  0.461939752},
{ 0.353553385,  0.500000000,  0.353553385},
{ 0.000000000,  0.500000000,  0.000000000},
{ 0.353553385,  0.500000000,  0.353553385},
{ 0.461939752,  0.500000000,  0.191341713},
{ 0.000000000,  0.500000000,  0.000000000},
{ 0.461939752,  0.500000000,  0.191341713},
{ 0.500000000,  0.500000000,  0.000000000},
{ 0.000000000,  0.500000000,  0.000000000},
{ 0.500000000,  0.500000000,  0.000000000},
{ 0.461939752,  0.500000000, -0.191341713},
{ 0.000000000,  0.500000000,  0.000000000},
{ 0.461939752,  0.500000000, -0.191341713},
{ 0.353553385,  0.500000000, -0.353553385},
{ 0.000000000,  0.500000000,  0.000000000},
{ 0.353553385,  0.500000000, -0.353553385},
{ 0.191341713,  0.500000000, -0.461939752},
{ 0.000000000,  0.500000000,  0.000000000},
{ 0.191341713,  0.500000000, -0.461939752},
{ 0.000000000,  0.500000000, -0.500000000},
{ 0.000000000,  0.500000000,  0.000000000},
{ 0.000000000,  0.500000000, -0.500000000},
{-0.191341713,  0.500000000, -0.461939752},
{ 0.000000000,  0.500000000,  0.000000000},
{-0.191341713,  0.500000000, -0.461939752},
{-0.353553385,  0.500000000, -0.353553385},
{ 0.000000000,  0.500000000,  0.000000000},
{-0.353553385,  0.500000000, -0.353553385},
{-0.461939752,  0.500000000, -0.191341713},
{ 0.000000000,  0.500000000,  0.000000000},
{-0.461939752,  0.500000000, -0.191341713},
{-0.500000000,  0.500000000, -0.000000000},
{ 0.000000000,  0.500000000,  0.000000000},
{-0.500000000,  0.500000000, -0.000000000},
{-0.461939752,  0.500000000,  0.191341713},
{ 0.000000000,  0.500000000,  0.000000000},
{-0.461939752,  0.500000000,  0.191341713},
{-0.353553385,  0.500000000,  0.353553385},
{ 0.000000000,  0.500000000,  0.000000000},
{-0.353553385,  0.500000000,  0.353553385},
{-0.191341713,  0.500000000,  0.461939752},
{ 0.000000000,  0.500000000,  0.000000000},
{-0.191341713,  0.500000000,  0.461939752},
{-0.000000000,  0.500000000,  0.500000000},
{ 0.000000000,  0.500000000,  0.000000000},
{ 0.191341713, -0.500000000,  0.461939752},
{ 0.000000000, -0.500000000,  0.500000000},
{ 0.000000000, -0.500000000,  0.000000000},
{ 0.353553385, -0.500000000,  0.353553385},
{ 0.191341713, -0.500000000,  0.461939752},
{ 0.000000000, -0.500000000,  0.000000000},
{ 0.461939752, -0.500000000,  0.191341713},
{ 0.353553385, -0.500000000,  0.353553385},
{ 0.000000000, -0.500000000,  0.000000000},
{ 0.500000000, -0.500000000,  0.000000000},
{ 0.461939752, -0.500000000,  0.191341713},
{ 0.000000000, -0.500000000,  0.000000000},
{ 0.461939752, -0.500000000, -0.191341713},
{ 0.500000000, -0.500000000,  0.000000000},
{ 0.000000000, -0.500000000,  0.000000000},
{ 0.353553385, -0.500000000, -0.353553385},
{ 0.461939752, -0.500000000, -0.191341713},
{ 0.000000000, -0.500000000,  0.000000000},
{ 0.191341713, -0.500000000, -0.461939752},
{ 0.353553385, -0.500000000, -0.353553385},
{ 0.000000000, -0.500000000,  0.000000000},
{ 0.000000000, -0.500000000, -0.500000000},
{ 0.191341713, -0.500000000, -0.461939752},
{ 0.000000000, -0.500000000,  0.000000000},
{-0.191341713, -0.500000000, -0.461939752},
{ 0.000000000, -0.500000000, -0.500000000},
{ 0.000000000, -0.500000000,  0.000000000},
{-0.353553385, -0.500000000, -0.353553385},
{-0.191341713, -0.500000000, -0.461939752},
{ 0.000000000, -0.500000000,  0.000000000},
{-0.461939752, -0.500000000, -0.191341713},
{-0.353553385, -0.500000000, -0.353553385},
{ 0.000000000, -0.500000000,  0.000000000},
{-0.500000000, -0.500000000, -0.000000000},
{-0.461939752, -0.500000000, -0.191341713},
{ 0.000000000, -0.500000000,  0.000000000},
{-0.461939752, -0.500000000,  0.191341713},
{-0.500000000, -0.500000000, -0.000000000},
{ 0.000000000, -0.500000000,  0.000000000},
{-0.353553385, -0.500000000,  0.353553385},
{-0.461939752, -0.500000000,  0.191341713},
{ 0.000000000, -0.500000000,  0.000000000},
{-0.191341713, -0.500000000,  0.461939752},
{-0.353553385, -0.500000000,  0.353553385},
{ 0.000000000, -0.500000000,  0.000000000},
{-0.000000000, -0.500000000,  0.500000000},
{-0.191341713, -0.500000000,  0.461939752},
{ 0.000000000, -0.500000000,  0.000000000},
};

std::vector<Vec2> CylinderMesh::uv = {
{ 0.000000000,  1.000000000},
{ 0.000000000,  0.000000000},
{ 0.062500000,  1.000000000},
{ 0.000000000,  0.000000000},
{ 0.062500000,  0.000000000},
{ 0.062500000,  1.000000000},
{ 0.062500000,  1.000000000},
{ 0.062500000,  0.000000000},
{ 0.125000000,  1.000000000},
{ 0.062500000,  0.000000000},
{ 0.125000000,  0.000000000},
{ 0.125000000,  1.000000000},
{ 0.125000000,  1.000000000},
{ 0.125000000,  0.000000000},
{ 0.187500000,  1.000000000},
{ 0.125000000,  0.000000000},
{ 0.187500000,  0.000000000},
{ 0.187500000,  1.000000000},
{ 0.187500000,  1.000000000},
{ 0.187500000,  0.000000000},
{ 0.250000000,  1.000000000},
{ 0.187500000,  0.000000000},
{ 0.250000000,  0.000000000},
{ 0.250000000,  1.000000000},
{ 0.250000000,  1.000000000},
{ 0.250000000,  0.000000000},
{ 0.312500000,  1.000000000},
{ 0.250000000,  0.000000000},
{ 0.312500000,  0.000000000},
{ 0.312500000,  1.000000000},
{ 0.312500000,  1.000000000},
{ 0.312500000,  0.000000000},
{ 0.375000000,  1.000000000},
{ 0.312500000,  0.000000000},
{ 0.375000000,  0.000000000},
{ 0.375000000,  1.000000000},
{ 0.375000000,  1.000000000},
{ 0.375000000,  0.000000000},
{ 0.437500000,  1.000000000},
{ 0.375000000,  0.000000000},
{ 0.437500000,  0.000000000},
{ 0.437500000,  1.000000000},
{ 0.437500000,  1.000000000},
{ 0.437500000,  0.000000000},
{ 0.500000000,  1.000000000},
{ 0.437500000,  0.000000000},
{ 0.500000000,  0.000000000},
{ 0.500000000,  1.000000000},
{ 0.500000000,  1.000000000},
{ 0.500000000,  0.000000000},
{ 0.562500000,  1.000000000},
{ 0.500000000,  0.000000000},
{ 0.562500000,  0.000000000},
{ 0.562500000,  1.000000000},
{ 0.562500000,  1.000000000},
{ 0.562500000,  0.000000000},
{ 0.625000000,  1.000000000},
{ 0.562500000,  0.000000000},
{ 0.625000000,  0.000000000},
{ 0.625000000,  1.000000000},
{ 0.625000000,  1.000000000},
{ 0.625000000,  0.000000000},
{ 0.687500000,  1.000000000},
{ 0.625000000,  0.000000000},
{ 0.687500000,  0.000000000},
{ 0.687500000,  1.000000000},
{ 0.687500000,  1.000000000},
{ 0.687500000,  0.000000000},
{ 0.750000000,  1.000000000},
{ 0.687500000,  0.000000000},
{ 0.750000000,  0.000000000},
{ 0.750000000,  1.000000000},
{ 0.750000000,  1.000000000},
{ 0.750000000,  0.000000000},
{ 0.812500000,  1.000000000},
{ 0.750000000,  0.000000000},
{ 0.812500000,  0.000000000},
{ 0.812500000,  1.000000000},
{ 0.812500000,  1.000000000},
{ 0.812500000,  0.000000000},
{ 0.875000000,  1.000000000},
{ 0.812500000,  0.000000000},
{ 0.875000000,  0.000000000},
{ 0.875000000,  1.000000000},
{ 0.875000000,  1.000000000},
{ 0.875000000,  0.000000000},
{ 0.937500000,  1.000000000},
{ 0.875000000,  0.000000000},
{ 0.937500000,  0.000000000},
{ 0.937500000,  1.000000000},
{ 0.937500000,  1.000000000},
{ 0.937500000,  0.000000000},
{ 1.000000000,  1.000000000},
{ 0.937500000,  0.000000000},
{ 1.000000000,  0.000000000},
{ 1.000000000,  1.000000000},
{ 1.000000000,  0.500000000},
{ 0.961939752,  0.691341698},
{ 0.500000000,  0.500000000},
{ 0.961939752,  0.691341698},
{ 0.853553414,  0.853553414},
{ 0.500000000,  0.500000000},
{ 0.853553414,  0.853553414},
{ 0.691341698,  0.961939752},
{ 0.500000000,  0.500000000},
{ 0.691341698,  0.961939752},
{ 0.500000000,  1.000000000},
{ 0.500000000,  0.500000000},
{ 0.500000000,  1.000000000},
{ 0.308658272,  0.961939752},
{ 0.500000000,  0.500000000},
{ 0.308658272,  0.961939752},
{ 0.146446615,  0.853553414},
{ 0.500000000,  0.500000000},
{ 0.146446615,  0.853553414},
{ 0.038060233,  0.691341698},
{ 0.500000000,  0.500000000},
{ 0.038060233,  0.691341698},
{ 0.000000000,  0.500000000},
{ 0.500000000,  0.500000000},
{ 0.000000000,  0.500000000},
{ 0.038060233,  0.308658272},
{ 0.500000000,  0.500000000},
{ 0.038060233,  0.308658272},
{ 0.146446615,  0.146446615},
{ 0.500000000,  0.500000000},
{ 0.146446615,  0.146446615},
{ 0.308658272,  0.038060233},
{ 0.500000000,  0.500000000},
{ 0.308658272,  0.038060233},
{ 0.500000000,  0.000000000},
{ 0.500000000,  0.500000000},
{ 0.500000000,  0.000000000},
{ 0.691341698,  0.038060233},
{ 0.500000000,  0.500000000},
{ 0.691341698,  0.038060233},
{ 0.853553414,  0.146446615},
{ 0.500000000,  0.500000000},
{ 0.853553414,  0.146446615},
{ 0.961939752,  0.308658272},
{ 0.500000000,  0.500000000},
{ 0.961939752,  0.308658272},
{ 1.000000000,  0.500000000},
{ 0.500000000,  0.500000000},
{ 0.961939752,  0.308658272},
{ 1.000000000,  0.500000000},
{ 0.500000000,  0.500000000},
{ 0.853553414,  0.146446615},
{ 0.961939752,  0.308658272},
{ 0.500000000,  0.500000000},
{ 0.691341698,  0.038060233},
{ 0.853553414,  0.146446615},
{ 0.500000000,  0.500000000},
{ 0.500000000,  0.000000000},
{ 0.691341698,  0.038060233},
{ 0.500000000,  0.500000000},
{ 0.308658272,  0.038060233},
{ 0.500000000,  0.000000000},
{ 0.500000000,  0.500000000},
{ 0.146446615,  0.146446615},
{ 0.308658272,  0.038060233},
{ 0.500000000,  0.500000000},
{ 0.038060233,  0.308658272},
{ 0.146446615,  0.146446615},
{ 0.500000000,  0.500000000},
{ 0.000000000,  0.500000000},
{ 0.038060233,  0.308658272},
{ 0.500000000,  0.500000000},
{ 0.038060233,  0.691341698},
{ 0.000000000,  0.500000000},
{ 0.500000000,  0.500000000},
{ 0.146446615,  0.853553414},
{ 0.038060233,  0.691341698},
{ 0.500000000,  0.500000000},
{ 0.308658272,  0.961939752},
{ 0.146446615,  0.853553414},
{ 0.500000000,  0.500000000},
{ 0.500000000,  1.000000000},
{ 0.308658272,  0.961939752},
{ 0.500000000,  0.500000000},
{ 0.691341698,  0.961939752},
{ 0.500000000,  1.000000000},
{ 0.500000000,  0.500000000},
{ 0.853553414,  0.853553414},
{ 0.691341698,  0.961939752},
{ 0.500000000,  0.500000000},
{ 0.961939752,  0.691341698},
{ 0.853553414,  0.853553414},
{ 0.500000000,  0.500000000},
{ 1.000000000,  0.500000000},
{ 0.961939752,  0.691341698},
{ 0.500000000,  0.500000000},
};

std::vector<Vec3> CylinderMesh::normal = {
{ 0.000000000,  0.000000000,  1.000000000},
{ 0.000000000,  0.000000000,  1.000000000},
{ 0.382683426,  0.000000000,  0.923879504},
{ 0.000000000,  0.000000000,  1.000000000},
{ 0.382683426,  0.000000000,  0.923879504},
{ 0.382683426,  0.000000000,  0.923879504},
{ 0.382683426,  0.000000000,  0.923879504},
{ 0.382683426,  0.000000000,  0.923879504},
{ 0.707106769,  0.000000000,  0.707106769},
{ 0.382683426,  0.000000000,  0.923879504},
{ 0.707106769,  0.000000000,  0.707106769},
{ 0.707106769,  0.000000000,  0.707106769},
{ 0.707106769,  0.000000000,  0.707106769},
{ 0.707106769,  0.000000000,  0.707106769},
{ 0.923879504,  0.000000000,  0.382683426},
{ 0.707106769,  0.000000000,  0.707106769},
{ 0.923879504,  0.000000000,  0.382683426},
{ 0.923879504,  0.000000000,  0.382683426},
{ 0.923879504,  0.000000000,  0.382683426},
{ 0.923879504,  0.000000000,  0.382683426},
{ 1.000000000,  0.000000000,  0.000000000},
{ 0.923879504,  0.000000000,  0.382683426},
{ 1.000000000,  0.000000000,  0.000000000},
{ 1.000000000,  0.000000000,  0.000000000},
{ 1.000000000,  0.000000000,  0.000000000},
{ 1.000000000,  0.000000000,  0.000000000},
{ 0.923879504,  0.000000000, -0.382683426},
{ 1.000000000,  0.000000000,  0.000000000},
{ 0.923879504,  0.000000000, -0.382683426},
{ 0.923879504,  0.000000000, -0.382683426},
{ 0.923879504,  0.000000000, -0.382683426},
{ 0.923879504,  0.000000000, -0.382683426},
{ 0.707106769,  0.000000000, -0.707106769},
{ 0.923879504,  0.000000000, -0.382683426},
{ 0.707106769,  0.000000000, -0.707106769},
{ 0.707106769,  0.000000000, -0.707106769},
{ 0.707106769,  0.000000000, -0.707106769},
{ 0.707106769,  0.000000000, -0.707106769},
{ 0.382683426,  0.000000000, -0.923879504},
{ 0.707106769,  0.000000000, -0.707106769},
{ 0.382683426,  0.000000000, -0.923879504},
{ 0.382683426,  0.000000000, -0.923879504},
{ 0.382683426,  0.000000000, -0.923879504},
{ 0.382683426,  0.000000000, -0.923879504},
{ 0.000000000,  0.000000000, -1.000000000},
{ 0.382683426,  0.000000000, -0.923879504},
{ 0.000000000,  0.000000000, -1.000000000},
{ 0.000000000,  0.000000000, -1.000000000},
{ 0.000000000,  0.000000000, -1.000000000},
{ 0.000000000,  0.000000000, -1.000000000},
{-0.382683426,  0.000000000, -0.923879504},
{ 0.000000000,  0.000000000, -1.000000000},
{-0.382683426,  0.000000000, -0.923879504},
{-0.382683426,  0.000000000, -0.923879504},
{-0.382683426,  0.000000000, -0.923879504},
{-0.382683426,  0.000000000, -0.923879504},
{-0.707106769,  0.000000000, -0.707106769},
{-0.382683426,  0.000000000, -0.923879504},
{-0.707106769,  0.000000000, -0.707106769},
{-0.707106769,  0.000000000, -0.707106769},
{-0.707106769,  0.000000000, -0.707106769},
{-0.707106769,  0.000000000, -0.707106769},
{-0.923879504,  0.000000000, -0.382683426},
{-0.707106769,  0.000000000, -0.707106769},
{-0.923879504,  0.000000000, -0.382683426},
{-0.923879504,  0.000000000, -0.382683426},
{-0.923879504,  0.000000000, -0.382683426},
{-0.923879504,  0.000000000, -0.382683426},
{-1.000000000,  0.000000000, -0.000000000},
{-0.923879504,  0.000000000, -0.382683426},
{-1.000000000,  0.000000000, -0.000000000},
{-1.000000000,  0.000000000, -0.000000000},
{-1.000000000,  0.000000000, -0.000000000},
{-1.000000000,  0.000000000, -0.000000000},
{-0.923879504,  0.000000000,  0.382683426},
{-1.000000000,  0.000000000, -0.000000000},
{-0.923879504,  0.000000000,  0.382683426},
{-0.923879504,  0.000000000,  0.382683426},
{-0.923879504,  0.000000000,  0.382683426},
{-0.923879504,  0.000000000,  0.382683426},
{-0.707106769,  0.000000000,  0.707106769},
{-0.923879504,  0.000000000,  0.382683426},
{-0.707106769,  0.000000000,  0.707106769},
{-0.707106769,  0.000000000,  0.707106769},
{-0.707106769,  0.000000000,  0.707106769},
{-0.707106769,  0.000000000,  0.707106769},
{-0.382683426,  0.000000000,  0.923879504},
{-0.707106769,  0.000000000,  0.707106769},
{-0.382683426,  0.000000000,  0.923879504},
{-0.382683426,  0.000000000,  0.923879504},
{-0.382683426,  0.000000000,  0.923879504},
{-0.382683426,  0.000000000,  0.923879504},
{-0.000000000,  0.000000000,  1.000000000},
{-0.382683426,  0.000000000,  0.923879504},
{-0.000000000,  0.000000000,  1.000000000},
{-0.000000000,  0.000000000,  1.000000000},
{ 0.000000000,  1.000000000,  0.000000000},
{ 0.000000000,  1.000000000,  0.000000000},
{ 0.000000000,  1.000000000,  0.000000000},
{ 0.000000000,  1.000000000,  0.000000000},
{ 0.000000000,  1.000000000,  0.000000000},
{ 0.000000000,  1.000000000,  0.000000000},
{ 0.000000000,  1.000000000,  0.000000000},
{ 0.000000000,  1.000000000,  0.000000000},
{ 0.000000000,  1.000000000,  0.000000000},
{ 0.000000000,  1.000000000,  0.000000000},
{ 0.000000000,  1.000000000,  0.000000000},
{ 0.000000000,  1.000000000,  0.000000000},
{ 0.000000000,  1.000000000,  0.000000000},
{ 0.000000000,  1.000000000,  0.000000000},
{ 0.000000000,  1.000000000,  0.000000000},
{ 0.000000000,  1.000000000,  0.000000000},
{ 0.000000000,  1.000000000,  0.000000000},
{ 0.000000000,  1.000000000,  0.000000000},
{ 0.000000000,  1.000000000,  0.000000000},
{ 0.000000000,  1.000000000,  0.000000000},
{ 0.000000000,  1.000000000,  0.000000000},
{ 0.000000000,  1.000000000,  0.000000000},
{ 0.000000000,  1.000000000,  0.000000000},
{ 0.000000000,  1.000000000,  0.000000000},
{ 0.000000000,  1.000000000,  0.000000000},
{ 0.000000000,  1.000000000,  0.000000000},
{ 0.000000000,  1.000000000,  0.000000000},
{ 0.000000000,  1.000000000,  0.000000000},
{ 0.000000000,  1.000000000,  0.000000000},
{ 0.000000000,  1.000000000,  0.000000000},
{ 0.000000000,  1.000000000,  0.000000000},
{ 0.000000000,  1.000000000,  0.000000000},
{ 0.000000000,  1.000000000,  0.000000000},
{ 0.000000000,  1.000000000,  0.000000000},
{ 0.000000000,  1.000000000,  0.000000000},
{ 0.000000000,  1.000000000,  0.000000000},
{ 0.000000000,  1.000000000,  0.000000000},
{ 0.000000000,  1.000000000,  0.000000000},
{ 0.000000000,  1.000000000,  0.000000000},
{ 0.000000000,  1.000000000,  0.000000000},
{ 0.000000000,  1.000000000,  0.000000000},
{ 0.000000000,  1.000000000,  0.000000000},
{ 0.000000000,  1.000000000,  0.000000000},
{ 0.000000000,  1.000000000,  0.000000000},
{ 0.000000000,  1.000000000,  0.000000000},
{ 0.000000000,  1.000000000,  0.000000000},
{ 0.000000000,  1.000000000,  0.000000000},
{ 0.000000000,  1.000000000,  0.000000000},
{ 0.000000000, -1.000000000,  0.000000000},
{ 0.000000000, -1.000000000,  0.000000000},
{ 0.000000000, -1.000000000,  0.000000000},
{ 0.000000000, -1.000000000,  0.000000000},
{ 0.000000000, -1.000000000,  0.000000000},
{ 0.000000000, -1.000000000,  0.000000000},
{ 0.000000000, -1.000000000,  0.000000000},
{ 0.000000000, -1.000000000,  0.000000000},
{ 0.000000000, -1.000000000,  0.000000000},
{ 0.000000000, -1.000000000,  0.000000000},
{ 0.000000000, -1.000000000,  0.000000000},
{ 0.000000000, -1.000000000,  0.000000000},
{ 0.000000000, -1.000000000,  0.000000000},
{ 0.000000000, -1.000000000,  0.000000000},
{ 0.000000000, -1.000000000,  0.000000000},
{ 0.000000000, -1.000000000,  0.000000000},
{ 0.000000000, -1.000000000,  0.000000000},
{ 0.000000000, -1.000000000,  0.000000000},
{ 0.000000000, -1.000000000,  0.000000000},
{ 0.000000000, -1.000000000,  0.000000000},
{ 0.000000000, -1.000000000,  0.000000000},
{ 0.000000000, -1.000000000,  0.000000000},
{ 0.000000000, -1.000000000,  0.000000000},
{ 0.000000000, -1.000000000,  0.000000000},
{ 0.000000000, -1.000000000,  0.000000000},
{ 0.000000000, -1.000000000,  0.000000000},
{ 0.000000000, -1.000000000,  0.000000000},
{ 0.000000000, -1.000000000,  0.000000000},
{ 0.000000000, -1.000000000,  0.000000000},
{ 0.000000000, -1.000000000,  0.000000000},
{ 0.000000000, -1.000000000,  0.000000000},
{ 0.000000000, -1.000000000,  0.000000000},
{ 0.000000000, -1.000000000,  0.000000000},
{ 0.000000000, -1.000000000,  0.000000000},
{ 0.000000000, -1.000000000,  0.000000000},
{ 0.000000000, -1.000000000,  0.000000000},
{ 0.000000000, -1.000000000,  0.000000000},
{ 0.000000000, -1.000000000,  0.000000000},
{ 0.000000000, -1.000000000,  0.000000000},
{ 0.000000000, -1.000000000,  0.000000000},
{ 0.000000000, -1.000000000,  0.000000000},
{ 0.000000000, -1.000000000,  0.000000000},
{ 0.000000000, -1.000000000,  0.000000000},
{ 0.000000000, -1.000000000,  0.000000000},
{ 0.000000000, -1.000000000,  0.000000000},
{ 0.000000000, -1.000000000,  0.000000000},
{ 0.000000000, -1.000000000,  0.000000000},
{ 0.000000000, -1.000000000,  0.000000000},
};

}

/* -------------------------------------------------------------------------- */
/* ---- ink/meshes/SphereMesh.cxx ------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

Mesh SphereMesh::create() {
	Mesh mesh = Mesh("Sphere");
	mesh.groups = {{"default", 0, 672}};
	mesh.vertex = vertex;
	mesh.uv = uv;
	mesh.normal = normal;
	return mesh;
}

std::vector<Vec3> SphereMesh::vertex = {
{ 0.000000000,  0.500000000,  0.000000000},
{-0.191341713,  0.461939752,  0.000000000},
{-0.176776692,  0.461939752,  0.073223308},
{ 0.000000000,  0.500000000,  0.000000000},
{-0.176776692,  0.461939752,  0.073223308},
{-0.135299027,  0.461939752,  0.135299027},
{ 0.000000000,  0.500000000,  0.000000000},
{-0.135299027,  0.461939752,  0.135299027},
{-0.073223308,  0.461939752,  0.176776692},
{ 0.000000000,  0.500000000,  0.000000000},
{-0.073223308,  0.461939752,  0.176776692},
{-0.000000000,  0.461939752,  0.191341713},
{ 0.000000000,  0.500000000,  0.000000000},
{-0.000000000,  0.461939752,  0.191341713},
{ 0.073223308,  0.461939752,  0.176776692},
{ 0.000000000,  0.500000000,  0.000000000},
{ 0.073223308,  0.461939752,  0.176776692},
{ 0.135299027,  0.461939752,  0.135299027},
{ 0.000000000,  0.500000000,  0.000000000},
{ 0.135299027,  0.461939752,  0.135299027},
{ 0.176776692,  0.461939752,  0.073223308},
{ 0.000000000,  0.500000000,  0.000000000},
{ 0.176776692,  0.461939752,  0.073223308},
{ 0.191341713,  0.461939752,  0.000000000},
{ 0.000000000,  0.500000000,  0.000000000},
{ 0.191341713,  0.461939752,  0.000000000},
{ 0.176776692,  0.461939752, -0.073223308},
{ 0.000000000,  0.500000000,  0.000000000},
{ 0.176776692,  0.461939752, -0.073223308},
{ 0.135299027,  0.461939752, -0.135299027},
{ 0.000000000,  0.500000000,  0.000000000},
{ 0.135299027,  0.461939752, -0.135299027},
{ 0.073223308,  0.461939752, -0.176776692},
{ 0.000000000,  0.500000000,  0.000000000},
{ 0.073223308,  0.461939752, -0.176776692},
{ 0.000000000,  0.461939752, -0.191341713},
{ 0.000000000,  0.500000000,  0.000000000},
{ 0.000000000,  0.461939752, -0.191341713},
{-0.073223308,  0.461939752, -0.176776692},
{ 0.000000000,  0.500000000,  0.000000000},
{-0.073223308,  0.461939752, -0.176776692},
{-0.135299027,  0.461939752, -0.135299027},
{ 0.000000000,  0.500000000,  0.000000000},
{-0.135299027,  0.461939752, -0.135299027},
{-0.176776692,  0.461939752, -0.073223308},
{ 0.000000000,  0.500000000,  0.000000000},
{-0.176776692,  0.461939752, -0.073223308},
{-0.191341713,  0.461939752, -0.000000000},
{-0.176776692,  0.461939752,  0.073223308},
{-0.191341713,  0.461939752,  0.000000000},
{-0.326640755,  0.353553385,  0.135299027},
{-0.191341713,  0.461939752,  0.000000000},
{-0.353553385,  0.353553385,  0.000000000},
{-0.326640755,  0.353553385,  0.135299027},
{-0.135299027,  0.461939752,  0.135299027},
{-0.176776692,  0.461939752,  0.073223308},
{-0.250000000,  0.353553385,  0.250000000},
{-0.176776692,  0.461939752,  0.073223308},
{-0.326640755,  0.353553385,  0.135299027},
{-0.250000000,  0.353553385,  0.250000000},
{-0.073223308,  0.461939752,  0.176776692},
{-0.135299027,  0.461939752,  0.135299027},
{-0.135299027,  0.353553385,  0.326640755},
{-0.135299027,  0.461939752,  0.135299027},
{-0.250000000,  0.353553385,  0.250000000},
{-0.135299027,  0.353553385,  0.326640755},
{-0.000000000,  0.461939752,  0.191341713},
{-0.073223308,  0.461939752,  0.176776692},
{-0.000000000,  0.353553385,  0.353553385},
{-0.073223308,  0.461939752,  0.176776692},
{-0.135299027,  0.353553385,  0.326640755},
{-0.000000000,  0.353553385,  0.353553385},
{ 0.073223308,  0.461939752,  0.176776692},
{-0.000000000,  0.461939752,  0.191341713},
{ 0.135299027,  0.353553385,  0.326640755},
{-0.000000000,  0.461939752,  0.191341713},
{-0.000000000,  0.353553385,  0.353553385},
{ 0.135299027,  0.353553385,  0.326640755},
{ 0.135299027,  0.461939752,  0.135299027},
{ 0.073223308,  0.461939752,  0.176776692},
{ 0.250000000,  0.353553385,  0.250000000},
{ 0.073223308,  0.461939752,  0.176776692},
{ 0.135299027,  0.353553385,  0.326640755},
{ 0.250000000,  0.353553385,  0.250000000},
{ 0.176776692,  0.461939752,  0.073223308},
{ 0.135299027,  0.461939752,  0.135299027},
{ 0.326640755,  0.353553385,  0.135299027},
{ 0.135299027,  0.461939752,  0.135299027},
{ 0.250000000,  0.353553385,  0.250000000},
{ 0.326640755,  0.353553385,  0.135299027},
{ 0.191341713,  0.461939752,  0.000000000},
{ 0.176776692,  0.461939752,  0.073223308},
{ 0.353553385,  0.353553385,  0.000000000},
{ 0.176776692,  0.461939752,  0.073223308},
{ 0.326640755,  0.353553385,  0.135299027},
{ 0.353553385,  0.353553385,  0.000000000},
{ 0.176776692,  0.461939752, -0.073223308},
{ 0.191341713,  0.461939752,  0.000000000},
{ 0.326640755,  0.353553385, -0.135299027},
{ 0.191341713,  0.461939752,  0.000000000},
{ 0.353553385,  0.353553385,  0.000000000},
{ 0.326640755,  0.353553385, -0.135299027},
{ 0.135299027,  0.461939752, -0.135299027},
{ 0.176776692,  0.461939752, -0.073223308},
{ 0.250000000,  0.353553385, -0.250000000},
{ 0.176776692,  0.461939752, -0.073223308},
{ 0.326640755,  0.353553385, -0.135299027},
{ 0.250000000,  0.353553385, -0.250000000},
{ 0.073223308,  0.461939752, -0.176776692},
{ 0.135299027,  0.461939752, -0.135299027},
{ 0.135299027,  0.353553385, -0.326640755},
{ 0.135299027,  0.461939752, -0.135299027},
{ 0.250000000,  0.353553385, -0.250000000},
{ 0.135299027,  0.353553385, -0.326640755},
{ 0.000000000,  0.461939752, -0.191341713},
{ 0.073223308,  0.461939752, -0.176776692},
{ 0.000000000,  0.353553385, -0.353553385},
{ 0.073223308,  0.461939752, -0.176776692},
{ 0.135299027,  0.353553385, -0.326640755},
{ 0.000000000,  0.353553385, -0.353553385},
{-0.073223308,  0.461939752, -0.176776692},
{ 0.000000000,  0.461939752, -0.191341713},
{-0.135299027,  0.353553385, -0.326640755},
{ 0.000000000,  0.461939752, -0.191341713},
{ 0.000000000,  0.353553385, -0.353553385},
{-0.135299027,  0.353553385, -0.326640755},
{-0.135299027,  0.461939752, -0.135299027},
{-0.073223308,  0.461939752, -0.176776692},
{-0.250000000,  0.353553385, -0.250000000},
{-0.073223308,  0.461939752, -0.176776692},
{-0.135299027,  0.353553385, -0.326640755},
{-0.250000000,  0.353553385, -0.250000000},
{-0.176776692,  0.461939752, -0.073223308},
{-0.135299027,  0.461939752, -0.135299027},
{-0.326640755,  0.353553385, -0.135299027},
{-0.135299027,  0.461939752, -0.135299027},
{-0.250000000,  0.353553385, -0.250000000},
{-0.326640755,  0.353553385, -0.135299027},
{-0.191341713,  0.461939752, -0.000000000},
{-0.176776692,  0.461939752, -0.073223308},
{-0.353553385,  0.353553385, -0.000000000},
{-0.176776692,  0.461939752, -0.073223308},
{-0.326640755,  0.353553385, -0.135299027},
{-0.353553385,  0.353553385, -0.000000000},
{-0.326640755,  0.353553385,  0.135299027},
{-0.353553385,  0.353553385,  0.000000000},
{-0.426776707,  0.191341713,  0.176776692},
{-0.353553385,  0.353553385,  0.000000000},
{-0.461939752,  0.191341713,  0.000000000},
{-0.426776707,  0.191341713,  0.176776692},
{-0.250000000,  0.353553385,  0.250000000},
{-0.326640755,  0.353553385,  0.135299027},
{-0.326640755,  0.191341713,  0.326640755},
{-0.326640755,  0.353553385,  0.135299027},
{-0.426776707,  0.191341713,  0.176776692},
{-0.326640755,  0.191341713,  0.326640755},
{-0.135299027,  0.353553385,  0.326640755},
{-0.250000000,  0.353553385,  0.250000000},
{-0.176776692,  0.191341713,  0.426776707},
{-0.250000000,  0.353553385,  0.250000000},
{-0.326640755,  0.191341713,  0.326640755},
{-0.176776692,  0.191341713,  0.426776707},
{-0.000000000,  0.353553385,  0.353553385},
{-0.135299027,  0.353553385,  0.326640755},
{-0.000000000,  0.191341713,  0.461939752},
{-0.135299027,  0.353553385,  0.326640755},
{-0.176776692,  0.191341713,  0.426776707},
{-0.000000000,  0.191341713,  0.461939752},
{ 0.135299027,  0.353553385,  0.326640755},
{-0.000000000,  0.353553385,  0.353553385},
{ 0.176776692,  0.191341713,  0.426776707},
{-0.000000000,  0.353553385,  0.353553385},
{-0.000000000,  0.191341713,  0.461939752},
{ 0.176776692,  0.191341713,  0.426776707},
{ 0.250000000,  0.353553385,  0.250000000},
{ 0.135299027,  0.353553385,  0.326640755},
{ 0.326640755,  0.191341713,  0.326640755},
{ 0.135299027,  0.353553385,  0.326640755},
{ 0.176776692,  0.191341713,  0.426776707},
{ 0.326640755,  0.191341713,  0.326640755},
{ 0.326640755,  0.353553385,  0.135299027},
{ 0.250000000,  0.353553385,  0.250000000},
{ 0.426776707,  0.191341713,  0.176776692},
{ 0.250000000,  0.353553385,  0.250000000},
{ 0.326640755,  0.191341713,  0.326640755},
{ 0.426776707,  0.191341713,  0.176776692},
{ 0.353553385,  0.353553385,  0.000000000},
{ 0.326640755,  0.353553385,  0.135299027},
{ 0.461939752,  0.191341713,  0.000000000},
{ 0.326640755,  0.353553385,  0.135299027},
{ 0.426776707,  0.191341713,  0.176776692},
{ 0.461939752,  0.191341713,  0.000000000},
{ 0.326640755,  0.353553385, -0.135299027},
{ 0.353553385,  0.353553385,  0.000000000},
{ 0.426776707,  0.191341713, -0.176776692},
{ 0.353553385,  0.353553385,  0.000000000},
{ 0.461939752,  0.191341713,  0.000000000},
{ 0.426776707,  0.191341713, -0.176776692},
{ 0.250000000,  0.353553385, -0.250000000},
{ 0.326640755,  0.353553385, -0.135299027},
{ 0.326640755,  0.191341713, -0.326640755},
{ 0.326640755,  0.353553385, -0.135299027},
{ 0.426776707,  0.191341713, -0.176776692},
{ 0.326640755,  0.191341713, -0.326640755},
{ 0.135299027,  0.353553385, -0.326640755},
{ 0.250000000,  0.353553385, -0.250000000},
{ 0.176776692,  0.191341713, -0.426776707},
{ 0.250000000,  0.353553385, -0.250000000},
{ 0.326640755,  0.191341713, -0.326640755},
{ 0.176776692,  0.191341713, -0.426776707},
{ 0.000000000,  0.353553385, -0.353553385},
{ 0.135299027,  0.353553385, -0.326640755},
{ 0.000000000,  0.191341713, -0.461939752},
{ 0.135299027,  0.353553385, -0.326640755},
{ 0.176776692,  0.191341713, -0.426776707},
{ 0.000000000,  0.191341713, -0.461939752},
{-0.135299027,  0.353553385, -0.326640755},
{ 0.000000000,  0.353553385, -0.353553385},
{-0.176776692,  0.191341713, -0.426776707},
{ 0.000000000,  0.353553385, -0.353553385},
{ 0.000000000,  0.191341713, -0.461939752},
{-0.176776692,  0.191341713, -0.426776707},
{-0.250000000,  0.353553385, -0.250000000},
{-0.135299027,  0.353553385, -0.326640755},
{-0.326640755,  0.191341713, -0.326640755},
{-0.135299027,  0.353553385, -0.326640755},
{-0.176776692,  0.191341713, -0.426776707},
{-0.326640755,  0.191341713, -0.326640755},
{-0.326640755,  0.353553385, -0.135299027},
{-0.250000000,  0.353553385, -0.250000000},
{-0.426776707,  0.191341713, -0.176776692},
{-0.250000000,  0.353553385, -0.250000000},
{-0.326640755,  0.191341713, -0.326640755},
{-0.426776707,  0.191341713, -0.176776692},
{-0.353553385,  0.353553385, -0.000000000},
{-0.326640755,  0.353553385, -0.135299027},
{-0.461939752,  0.191341713, -0.000000000},
{-0.326640755,  0.353553385, -0.135299027},
{-0.426776707,  0.191341713, -0.176776692},
{-0.461939752,  0.191341713, -0.000000000},
{-0.426776707,  0.191341713,  0.176776692},
{-0.461939752,  0.191341713,  0.000000000},
{-0.461939752,  0.000000000,  0.191341713},
{-0.461939752,  0.191341713,  0.000000000},
{-0.500000000,  0.000000000,  0.000000000},
{-0.461939752,  0.000000000,  0.191341713},
{-0.326640755,  0.191341713,  0.326640755},
{-0.426776707,  0.191341713,  0.176776692},
{-0.353553385,  0.000000000,  0.353553385},
{-0.426776707,  0.191341713,  0.176776692},
{-0.461939752,  0.000000000,  0.191341713},
{-0.353553385,  0.000000000,  0.353553385},
{-0.176776692,  0.191341713,  0.426776707},
{-0.326640755,  0.191341713,  0.326640755},
{-0.191341713,  0.000000000,  0.461939752},
{-0.326640755,  0.191341713,  0.326640755},
{-0.353553385,  0.000000000,  0.353553385},
{-0.191341713,  0.000000000,  0.461939752},
{-0.000000000,  0.191341713,  0.461939752},
{-0.176776692,  0.191341713,  0.426776707},
{-0.000000000,  0.000000000,  0.500000000},
{-0.176776692,  0.191341713,  0.426776707},
{-0.191341713,  0.000000000,  0.461939752},
{-0.000000000,  0.000000000,  0.500000000},
{ 0.176776692,  0.191341713,  0.426776707},
{-0.000000000,  0.191341713,  0.461939752},
{ 0.191341713,  0.000000000,  0.461939752},
{-0.000000000,  0.191341713,  0.461939752},
{-0.000000000,  0.000000000,  0.500000000},
{ 0.191341713,  0.000000000,  0.461939752},
{ 0.326640755,  0.191341713,  0.326640755},
{ 0.176776692,  0.191341713,  0.426776707},
{ 0.353553385,  0.000000000,  0.353553385},
{ 0.176776692,  0.191341713,  0.426776707},
{ 0.191341713,  0.000000000,  0.461939752},
{ 0.353553385,  0.000000000,  0.353553385},
{ 0.426776707,  0.191341713,  0.176776692},
{ 0.326640755,  0.191341713,  0.326640755},
{ 0.461939752,  0.000000000,  0.191341713},
{ 0.326640755,  0.191341713,  0.326640755},
{ 0.353553385,  0.000000000,  0.353553385},
{ 0.461939752,  0.000000000,  0.191341713},
{ 0.461939752,  0.191341713,  0.000000000},
{ 0.426776707,  0.191341713,  0.176776692},
{ 0.500000000,  0.000000000,  0.000000000},
{ 0.426776707,  0.191341713,  0.176776692},
{ 0.461939752,  0.000000000,  0.191341713},
{ 0.500000000,  0.000000000,  0.000000000},
{ 0.426776707,  0.191341713, -0.176776692},
{ 0.461939752,  0.191341713,  0.000000000},
{ 0.461939752,  0.000000000, -0.191341713},
{ 0.461939752,  0.191341713,  0.000000000},
{ 0.500000000,  0.000000000,  0.000000000},
{ 0.461939752,  0.000000000, -0.191341713},
{ 0.326640755,  0.191341713, -0.326640755},
{ 0.426776707,  0.191341713, -0.176776692},
{ 0.353553385,  0.000000000, -0.353553385},
{ 0.426776707,  0.191341713, -0.176776692},
{ 0.461939752,  0.000000000, -0.191341713},
{ 0.353553385,  0.000000000, -0.353553385},
{ 0.176776692,  0.191341713, -0.426776707},
{ 0.326640755,  0.191341713, -0.326640755},
{ 0.191341713,  0.000000000, -0.461939752},
{ 0.326640755,  0.191341713, -0.326640755},
{ 0.353553385,  0.000000000, -0.353553385},
{ 0.191341713,  0.000000000, -0.461939752},
{ 0.000000000,  0.191341713, -0.461939752},
{ 0.176776692,  0.191341713, -0.426776707},
{ 0.000000000,  0.000000000, -0.500000000},
{ 0.176776692,  0.191341713, -0.426776707},
{ 0.191341713,  0.000000000, -0.461939752},
{ 0.000000000,  0.000000000, -0.500000000},
{-0.176776692,  0.191341713, -0.426776707},
{ 0.000000000,  0.191341713, -0.461939752},
{-0.191341713,  0.000000000, -0.461939752},
{ 0.000000000,  0.191341713, -0.461939752},
{ 0.000000000,  0.000000000, -0.500000000},
{-0.191341713,  0.000000000, -0.461939752},
{-0.326640755,  0.191341713, -0.326640755},
{-0.176776692,  0.191341713, -0.426776707},
{-0.353553385,  0.000000000, -0.353553385},
{-0.176776692,  0.191341713, -0.426776707},
{-0.191341713,  0.000000000, -0.461939752},
{-0.353553385,  0.000000000, -0.353553385},
{-0.426776707,  0.191341713, -0.176776692},
{-0.326640755,  0.191341713, -0.326640755},
{-0.461939752,  0.000000000, -0.191341713},
{-0.326640755,  0.191341713, -0.326640755},
{-0.353553385,  0.000000000, -0.353553385},
{-0.461939752,  0.000000000, -0.191341713},
{-0.461939752,  0.191341713, -0.000000000},
{-0.426776707,  0.191341713, -0.176776692},
{-0.500000000,  0.000000000, -0.000000000},
{-0.426776707,  0.191341713, -0.176776692},
{-0.461939752,  0.000000000, -0.191341713},
{-0.500000000,  0.000000000, -0.000000000},
{-0.461939752,  0.000000000,  0.191341713},
{-0.500000000,  0.000000000,  0.000000000},
{-0.426776707, -0.191341713,  0.176776692},
{-0.500000000,  0.000000000,  0.000000000},
{-0.461939752, -0.191341713,  0.000000000},
{-0.426776707, -0.191341713,  0.176776692},
{-0.353553385,  0.000000000,  0.353553385},
{-0.461939752,  0.000000000,  0.191341713},
{-0.326640755, -0.191341713,  0.326640755},
{-0.461939752,  0.000000000,  0.191341713},
{-0.426776707, -0.191341713,  0.176776692},
{-0.326640755, -0.191341713,  0.326640755},
{-0.191341713,  0.000000000,  0.461939752},
{-0.353553385,  0.000000000,  0.353553385},
{-0.176776692, -0.191341713,  0.426776707},
{-0.353553385,  0.000000000,  0.353553385},
{-0.326640755, -0.191341713,  0.326640755},
{-0.176776692, -0.191341713,  0.426776707},
{-0.000000000,  0.000000000,  0.500000000},
{-0.191341713,  0.000000000,  0.461939752},
{-0.000000000, -0.191341713,  0.461939752},
{-0.191341713,  0.000000000,  0.461939752},
{-0.176776692, -0.191341713,  0.426776707},
{-0.000000000, -0.191341713,  0.461939752},
{ 0.191341713,  0.000000000,  0.461939752},
{-0.000000000,  0.000000000,  0.500000000},
{ 0.176776692, -0.191341713,  0.426776707},
{-0.000000000,  0.000000000,  0.500000000},
{-0.000000000, -0.191341713,  0.461939752},
{ 0.176776692, -0.191341713,  0.426776707},
{ 0.353553385,  0.000000000,  0.353553385},
{ 0.191341713,  0.000000000,  0.461939752},
{ 0.326640755, -0.191341713,  0.326640755},
{ 0.191341713,  0.000000000,  0.461939752},
{ 0.176776692, -0.191341713,  0.426776707},
{ 0.326640755, -0.191341713,  0.326640755},
{ 0.461939752,  0.000000000,  0.191341713},
{ 0.353553385,  0.000000000,  0.353553385},
{ 0.426776707, -0.191341713,  0.176776692},
{ 0.353553385,  0.000000000,  0.353553385},
{ 0.326640755, -0.191341713,  0.326640755},
{ 0.426776707, -0.191341713,  0.176776692},
{ 0.500000000,  0.000000000,  0.000000000},
{ 0.461939752,  0.000000000,  0.191341713},
{ 0.461939752, -0.191341713,  0.000000000},
{ 0.461939752,  0.000000000,  0.191341713},
{ 0.426776707, -0.191341713,  0.176776692},
{ 0.461939752, -0.191341713,  0.000000000},
{ 0.461939752,  0.000000000, -0.191341713},
{ 0.500000000,  0.000000000,  0.000000000},
{ 0.426776707, -0.191341713, -0.176776692},
{ 0.500000000,  0.000000000,  0.000000000},
{ 0.461939752, -0.191341713,  0.000000000},
{ 0.426776707, -0.191341713, -0.176776692},
{ 0.353553385,  0.000000000, -0.353553385},
{ 0.461939752,  0.000000000, -0.191341713},
{ 0.326640755, -0.191341713, -0.326640755},
{ 0.461939752,  0.000000000, -0.191341713},
{ 0.426776707, -0.191341713, -0.176776692},
{ 0.326640755, -0.191341713, -0.326640755},
{ 0.191341713,  0.000000000, -0.461939752},
{ 0.353553385,  0.000000000, -0.353553385},
{ 0.176776692, -0.191341713, -0.426776707},
{ 0.353553385,  0.000000000, -0.353553385},
{ 0.326640755, -0.191341713, -0.326640755},
{ 0.176776692, -0.191341713, -0.426776707},
{ 0.000000000,  0.000000000, -0.500000000},
{ 0.191341713,  0.000000000, -0.461939752},
{ 0.000000000, -0.191341713, -0.461939752},
{ 0.191341713,  0.000000000, -0.461939752},
{ 0.176776692, -0.191341713, -0.426776707},
{ 0.000000000, -0.191341713, -0.461939752},
{-0.191341713,  0.000000000, -0.461939752},
{ 0.000000000,  0.000000000, -0.500000000},
{-0.176776692, -0.191341713, -0.426776707},
{ 0.000000000,  0.000000000, -0.500000000},
{ 0.000000000, -0.191341713, -0.461939752},
{-0.176776692, -0.191341713, -0.426776707},
{-0.353553385,  0.000000000, -0.353553385},
{-0.191341713,  0.000000000, -0.461939752},
{-0.326640755, -0.191341713, -0.326640755},
{-0.191341713,  0.000000000, -0.461939752},
{-0.176776692, -0.191341713, -0.426776707},
{-0.326640755, -0.191341713, -0.326640755},
{-0.461939752,  0.000000000, -0.191341713},
{-0.353553385,  0.000000000, -0.353553385},
{-0.426776707, -0.191341713, -0.176776692},
{-0.353553385,  0.000000000, -0.353553385},
{-0.326640755, -0.191341713, -0.326640755},
{-0.426776707, -0.191341713, -0.176776692},
{-0.500000000,  0.000000000, -0.000000000},
{-0.461939752,  0.000000000, -0.191341713},
{-0.461939752, -0.191341713, -0.000000000},
{-0.461939752,  0.000000000, -0.191341713},
{-0.426776707, -0.191341713, -0.176776692},
{-0.461939752, -0.191341713, -0.000000000},
{-0.426776707, -0.191341713,  0.176776692},
{-0.461939752, -0.191341713,  0.000000000},
{-0.326640755, -0.353553385,  0.135299027},
{-0.461939752, -0.191341713,  0.000000000},
{-0.353553385, -0.353553385,  0.000000000},
{-0.326640755, -0.353553385,  0.135299027},
{-0.326640755, -0.191341713,  0.326640755},
{-0.426776707, -0.191341713,  0.176776692},
{-0.250000000, -0.353553385,  0.250000000},
{-0.426776707, -0.191341713,  0.176776692},
{-0.326640755, -0.353553385,  0.135299027},
{-0.250000000, -0.353553385,  0.250000000},
{-0.176776692, -0.191341713,  0.426776707},
{-0.326640755, -0.191341713,  0.326640755},
{-0.135299027, -0.353553385,  0.326640755},
{-0.326640755, -0.191341713,  0.326640755},
{-0.250000000, -0.353553385,  0.250000000},
{-0.135299027, -0.353553385,  0.326640755},
{-0.000000000, -0.191341713,  0.461939752},
{-0.176776692, -0.191341713,  0.426776707},
{-0.000000000, -0.353553385,  0.353553385},
{-0.176776692, -0.191341713,  0.426776707},
{-0.135299027, -0.353553385,  0.326640755},
{-0.000000000, -0.353553385,  0.353553385},
{ 0.176776692, -0.191341713,  0.426776707},
{-0.000000000, -0.191341713,  0.461939752},
{ 0.135299027, -0.353553385,  0.326640755},
{-0.000000000, -0.191341713,  0.461939752},
{-0.000000000, -0.353553385,  0.353553385},
{ 0.135299027, -0.353553385,  0.326640755},
{ 0.326640755, -0.191341713,  0.326640755},
{ 0.176776692, -0.191341713,  0.426776707},
{ 0.250000000, -0.353553385,  0.250000000},
{ 0.176776692, -0.191341713,  0.426776707},
{ 0.135299027, -0.353553385,  0.326640755},
{ 0.250000000, -0.353553385,  0.250000000},
{ 0.426776707, -0.191341713,  0.176776692},
{ 0.326640755, -0.191341713,  0.326640755},
{ 0.326640755, -0.353553385,  0.135299027},
{ 0.326640755, -0.191341713,  0.326640755},
{ 0.250000000, -0.353553385,  0.250000000},
{ 0.326640755, -0.353553385,  0.135299027},
{ 0.461939752, -0.191341713,  0.000000000},
{ 0.426776707, -0.191341713,  0.176776692},
{ 0.353553385, -0.353553385,  0.000000000},
{ 0.426776707, -0.191341713,  0.176776692},
{ 0.326640755, -0.353553385,  0.135299027},
{ 0.353553385, -0.353553385,  0.000000000},
{ 0.426776707, -0.191341713, -0.176776692},
{ 0.461939752, -0.191341713,  0.000000000},
{ 0.326640755, -0.353553385, -0.135299027},
{ 0.461939752, -0.191341713,  0.000000000},
{ 0.353553385, -0.353553385,  0.000000000},
{ 0.326640755, -0.353553385, -0.135299027},
{ 0.326640755, -0.191341713, -0.326640755},
{ 0.426776707, -0.191341713, -0.176776692},
{ 0.250000000, -0.353553385, -0.250000000},
{ 0.426776707, -0.191341713, -0.176776692},
{ 0.326640755, -0.353553385, -0.135299027},
{ 0.250000000, -0.353553385, -0.250000000},
{ 0.176776692, -0.191341713, -0.426776707},
{ 0.326640755, -0.191341713, -0.326640755},
{ 0.135299027, -0.353553385, -0.326640755},
{ 0.326640755, -0.191341713, -0.326640755},
{ 0.250000000, -0.353553385, -0.250000000},
{ 0.135299027, -0.353553385, -0.326640755},
{ 0.000000000, -0.191341713, -0.461939752},
{ 0.176776692, -0.191341713, -0.426776707},
{ 0.000000000, -0.353553385, -0.353553385},
{ 0.176776692, -0.191341713, -0.426776707},
{ 0.135299027, -0.353553385, -0.326640755},
{ 0.000000000, -0.353553385, -0.353553385},
{-0.176776692, -0.191341713, -0.426776707},
{ 0.000000000, -0.191341713, -0.461939752},
{-0.135299027, -0.353553385, -0.326640755},
{ 0.000000000, -0.191341713, -0.461939752},
{ 0.000000000, -0.353553385, -0.353553385},
{-0.135299027, -0.353553385, -0.326640755},
{-0.326640755, -0.191341713, -0.326640755},
{-0.176776692, -0.191341713, -0.426776707},
{-0.250000000, -0.353553385, -0.250000000},
{-0.176776692, -0.191341713, -0.426776707},
{-0.135299027, -0.353553385, -0.326640755},
{-0.250000000, -0.353553385, -0.250000000},
{-0.426776707, -0.191341713, -0.176776692},
{-0.326640755, -0.191341713, -0.326640755},
{-0.326640755, -0.353553385, -0.135299027},
{-0.326640755, -0.191341713, -0.326640755},
{-0.250000000, -0.353553385, -0.250000000},
{-0.326640755, -0.353553385, -0.135299027},
{-0.461939752, -0.191341713, -0.000000000},
{-0.426776707, -0.191341713, -0.176776692},
{-0.353553385, -0.353553385, -0.000000000},
{-0.426776707, -0.191341713, -0.176776692},
{-0.326640755, -0.353553385, -0.135299027},
{-0.353553385, -0.353553385, -0.000000000},
{-0.326640755, -0.353553385,  0.135299027},
{-0.353553385, -0.353553385,  0.000000000},
{-0.176776692, -0.461939752,  0.073223308},
{-0.353553385, -0.353553385,  0.000000000},
{-0.191341713, -0.461939752,  0.000000000},
{-0.176776692, -0.461939752,  0.073223308},
{-0.250000000, -0.353553385,  0.250000000},
{-0.326640755, -0.353553385,  0.135299027},
{-0.135299027, -0.461939752,  0.135299027},
{-0.326640755, -0.353553385,  0.135299027},
{-0.176776692, -0.461939752,  0.073223308},
{-0.135299027, -0.461939752,  0.135299027},
{-0.135299027, -0.353553385,  0.326640755},
{-0.250000000, -0.353553385,  0.250000000},
{-0.073223308, -0.461939752,  0.176776692},
{-0.250000000, -0.353553385,  0.250000000},
{-0.135299027, -0.461939752,  0.135299027},
{-0.073223308, -0.461939752,  0.176776692},
{-0.000000000, -0.353553385,  0.353553385},
{-0.135299027, -0.353553385,  0.326640755},
{-0.000000000, -0.461939752,  0.191341713},
{-0.135299027, -0.353553385,  0.326640755},
{-0.073223308, -0.461939752,  0.176776692},
{-0.000000000, -0.461939752,  0.191341713},
{ 0.135299027, -0.353553385,  0.326640755},
{-0.000000000, -0.353553385,  0.353553385},
{ 0.073223308, -0.461939752,  0.176776692},
{-0.000000000, -0.353553385,  0.353553385},
{-0.000000000, -0.461939752,  0.191341713},
{ 0.073223308, -0.461939752,  0.176776692},
{ 0.250000000, -0.353553385,  0.250000000},
{ 0.135299027, -0.353553385,  0.326640755},
{ 0.135299027, -0.461939752,  0.135299027},
{ 0.135299027, -0.353553385,  0.326640755},
{ 0.073223308, -0.461939752,  0.176776692},
{ 0.135299027, -0.461939752,  0.135299027},
{ 0.326640755, -0.353553385,  0.135299027},
{ 0.250000000, -0.353553385,  0.250000000},
{ 0.176776692, -0.461939752,  0.073223308},
{ 0.250000000, -0.353553385,  0.250000000},
{ 0.135299027, -0.461939752,  0.135299027},
{ 0.176776692, -0.461939752,  0.073223308},
{ 0.353553385, -0.353553385,  0.000000000},
{ 0.326640755, -0.353553385,  0.135299027},
{ 0.191341713, -0.461939752,  0.000000000},
{ 0.326640755, -0.353553385,  0.135299027},
{ 0.176776692, -0.461939752,  0.073223308},
{ 0.191341713, -0.461939752,  0.000000000},
{ 0.326640755, -0.353553385, -0.135299027},
{ 0.353553385, -0.353553385,  0.000000000},
{ 0.176776692, -0.461939752, -0.073223308},
{ 0.353553385, -0.353553385,  0.000000000},
{ 0.191341713, -0.461939752,  0.000000000},
{ 0.176776692, -0.461939752, -0.073223308},
{ 0.250000000, -0.353553385, -0.250000000},
{ 0.326640755, -0.353553385, -0.135299027},
{ 0.135299027, -0.461939752, -0.135299027},
{ 0.326640755, -0.353553385, -0.135299027},
{ 0.176776692, -0.461939752, -0.073223308},
{ 0.135299027, -0.461939752, -0.135299027},
{ 0.135299027, -0.353553385, -0.326640755},
{ 0.250000000, -0.353553385, -0.250000000},
{ 0.073223308, -0.461939752, -0.176776692},
{ 0.250000000, -0.353553385, -0.250000000},
{ 0.135299027, -0.461939752, -0.135299027},
{ 0.073223308, -0.461939752, -0.176776692},
{ 0.000000000, -0.353553385, -0.353553385},
{ 0.135299027, -0.353553385, -0.326640755},
{ 0.000000000, -0.461939752, -0.191341713},
{ 0.135299027, -0.353553385, -0.326640755},
{ 0.073223308, -0.461939752, -0.176776692},
{ 0.000000000, -0.461939752, -0.191341713},
{-0.135299027, -0.353553385, -0.326640755},
{ 0.000000000, -0.353553385, -0.353553385},
{-0.073223308, -0.461939752, -0.176776692},
{ 0.000000000, -0.353553385, -0.353553385},
{ 0.000000000, -0.461939752, -0.191341713},
{-0.073223308, -0.461939752, -0.176776692},
{-0.250000000, -0.353553385, -0.250000000},
{-0.135299027, -0.353553385, -0.326640755},
{-0.135299027, -0.461939752, -0.135299027},
{-0.135299027, -0.353553385, -0.326640755},
{-0.073223308, -0.461939752, -0.176776692},
{-0.135299027, -0.461939752, -0.135299027},
{-0.326640755, -0.353553385, -0.135299027},
{-0.250000000, -0.353553385, -0.250000000},
{-0.176776692, -0.461939752, -0.073223308},
{-0.250000000, -0.353553385, -0.250000000},
{-0.135299027, -0.461939752, -0.135299027},
{-0.176776692, -0.461939752, -0.073223308},
{-0.353553385, -0.353553385, -0.000000000},
{-0.326640755, -0.353553385, -0.135299027},
{-0.191341713, -0.461939752, -0.000000000},
{-0.326640755, -0.353553385, -0.135299027},
{-0.176776692, -0.461939752, -0.073223308},
{-0.191341713, -0.461939752, -0.000000000},
{-0.176776692, -0.461939752,  0.073223308},
{-0.191341713, -0.461939752,  0.000000000},
{-0.000000000, -0.500000000,  0.000000000},
{-0.135299027, -0.461939752,  0.135299027},
{-0.176776692, -0.461939752,  0.073223308},
{-0.000000000, -0.500000000,  0.000000000},
{-0.073223308, -0.461939752,  0.176776692},
{-0.135299027, -0.461939752,  0.135299027},
{-0.000000000, -0.500000000,  0.000000000},
{-0.000000000, -0.461939752,  0.191341713},
{-0.073223308, -0.461939752,  0.176776692},
{-0.000000000, -0.500000000,  0.000000000},
{ 0.073223308, -0.461939752,  0.176776692},
{-0.000000000, -0.461939752,  0.191341713},
{ 0.000000000, -0.500000000,  0.000000000},
{ 0.135299027, -0.461939752,  0.135299027},
{ 0.073223308, -0.461939752,  0.176776692},
{ 0.000000000, -0.500000000,  0.000000000},
{ 0.176776692, -0.461939752,  0.073223308},
{ 0.135299027, -0.461939752,  0.135299027},
{ 0.000000000, -0.500000000,  0.000000000},
{ 0.191341713, -0.461939752,  0.000000000},
{ 0.176776692, -0.461939752,  0.073223308},
{ 0.000000000, -0.500000000,  0.000000000},
{ 0.176776692, -0.461939752, -0.073223308},
{ 0.191341713, -0.461939752,  0.000000000},
{ 0.000000000, -0.500000000, -0.000000000},
{ 0.135299027, -0.461939752, -0.135299027},
{ 0.176776692, -0.461939752, -0.073223308},
{ 0.000000000, -0.500000000, -0.000000000},
{ 0.073223308, -0.461939752, -0.176776692},
{ 0.135299027, -0.461939752, -0.135299027},
{ 0.000000000, -0.500000000, -0.000000000},
{ 0.000000000, -0.461939752, -0.191341713},
{ 0.073223308, -0.461939752, -0.176776692},
{ 0.000000000, -0.500000000, -0.000000000},
{-0.073223308, -0.461939752, -0.176776692},
{ 0.000000000, -0.461939752, -0.191341713},
{-0.000000000, -0.500000000, -0.000000000},
{-0.135299027, -0.461939752, -0.135299027},
{-0.073223308, -0.461939752, -0.176776692},
{-0.000000000, -0.500000000, -0.000000000},
{-0.176776692, -0.461939752, -0.073223308},
{-0.135299027, -0.461939752, -0.135299027},
{-0.000000000, -0.500000000, -0.000000000},
{-0.191341713, -0.461939752, -0.000000000},
{-0.176776692, -0.461939752, -0.073223308},
{-0.000000000, -0.500000000, -0.000000000},
};

std::vector<Vec2> SphereMesh::uv = {
{ 0.031250000,  1.000000000},
{ 0.000000000,  0.875000000},
{ 0.062500000,  0.875000000},
{ 0.093750000,  1.000000000},
{ 0.062500000,  0.875000000},
{ 0.125000000,  0.875000000},
{ 0.156250000,  1.000000000},
{ 0.125000000,  0.875000000},
{ 0.187500000,  0.875000000},
{ 0.218750000,  1.000000000},
{ 0.187500000,  0.875000000},
{ 0.250000000,  0.875000000},
{ 0.281250000,  1.000000000},
{ 0.250000000,  0.875000000},
{ 0.312500000,  0.875000000},
{ 0.343750000,  1.000000000},
{ 0.312500000,  0.875000000},
{ 0.375000000,  0.875000000},
{ 0.406250000,  1.000000000},
{ 0.375000000,  0.875000000},
{ 0.437500000,  0.875000000},
{ 0.468750000,  1.000000000},
{ 0.437500000,  0.875000000},
{ 0.500000000,  0.875000000},
{ 0.531250000,  1.000000000},
{ 0.500000000,  0.875000000},
{ 0.562500000,  0.875000000},
{ 0.593750000,  1.000000000},
{ 0.562500000,  0.875000000},
{ 0.625000000,  0.875000000},
{ 0.656250000,  1.000000000},
{ 0.625000000,  0.875000000},
{ 0.687500000,  0.875000000},
{ 0.718750000,  1.000000000},
{ 0.687500000,  0.875000000},
{ 0.750000000,  0.875000000},
{ 0.781250000,  1.000000000},
{ 0.750000000,  0.875000000},
{ 0.812500000,  0.875000000},
{ 0.843750000,  1.000000000},
{ 0.812500000,  0.875000000},
{ 0.875000000,  0.875000000},
{ 0.906250000,  1.000000000},
{ 0.875000000,  0.875000000},
{ 0.937500000,  0.875000000},
{ 0.968750000,  1.000000000},
{ 0.937500000,  0.875000000},
{ 1.000000000,  0.875000000},
{ 0.062500000,  0.875000000},
{ 0.000000000,  0.875000000},
{ 0.062500000,  0.750000000},
{ 0.000000000,  0.875000000},
{ 0.000000000,  0.750000000},
{ 0.062500000,  0.750000000},
{ 0.125000000,  0.875000000},
{ 0.062500000,  0.875000000},
{ 0.125000000,  0.750000000},
{ 0.062500000,  0.875000000},
{ 0.062500000,  0.750000000},
{ 0.125000000,  0.750000000},
{ 0.187500000,  0.875000000},
{ 0.125000000,  0.875000000},
{ 0.187500000,  0.750000000},
{ 0.125000000,  0.875000000},
{ 0.125000000,  0.750000000},
{ 0.187500000,  0.750000000},
{ 0.250000000,  0.875000000},
{ 0.187500000,  0.875000000},
{ 0.250000000,  0.750000000},
{ 0.187500000,  0.875000000},
{ 0.187500000,  0.750000000},
{ 0.250000000,  0.750000000},
{ 0.312500000,  0.875000000},
{ 0.250000000,  0.875000000},
{ 0.312500000,  0.750000000},
{ 0.250000000,  0.875000000},
{ 0.250000000,  0.750000000},
{ 0.312500000,  0.750000000},
{ 0.375000000,  0.875000000},
{ 0.312500000,  0.875000000},
{ 0.375000000,  0.750000000},
{ 0.312500000,  0.875000000},
{ 0.312500000,  0.750000000},
{ 0.375000000,  0.750000000},
{ 0.437500000,  0.875000000},
{ 0.375000000,  0.875000000},
{ 0.437500000,  0.750000000},
{ 0.375000000,  0.875000000},
{ 0.375000000,  0.750000000},
{ 0.437500000,  0.750000000},
{ 0.500000000,  0.875000000},
{ 0.437500000,  0.875000000},
{ 0.500000000,  0.750000000},
{ 0.437500000,  0.875000000},
{ 0.437500000,  0.750000000},
{ 0.500000000,  0.750000000},
{ 0.562500000,  0.875000000},
{ 0.500000000,  0.875000000},
{ 0.562500000,  0.750000000},
{ 0.500000000,  0.875000000},
{ 0.500000000,  0.750000000},
{ 0.562500000,  0.750000000},
{ 0.625000000,  0.875000000},
{ 0.562500000,  0.875000000},
{ 0.625000000,  0.750000000},
{ 0.562500000,  0.875000000},
{ 0.562500000,  0.750000000},
{ 0.625000000,  0.750000000},
{ 0.687500000,  0.875000000},
{ 0.625000000,  0.875000000},
{ 0.687500000,  0.750000000},
{ 0.625000000,  0.875000000},
{ 0.625000000,  0.750000000},
{ 0.687500000,  0.750000000},
{ 0.750000000,  0.875000000},
{ 0.687500000,  0.875000000},
{ 0.750000000,  0.750000000},
{ 0.687500000,  0.875000000},
{ 0.687500000,  0.750000000},
{ 0.750000000,  0.750000000},
{ 0.812500000,  0.875000000},
{ 0.750000000,  0.875000000},
{ 0.812500000,  0.750000000},
{ 0.750000000,  0.875000000},
{ 0.750000000,  0.750000000},
{ 0.812500000,  0.750000000},
{ 0.875000000,  0.875000000},
{ 0.812500000,  0.875000000},
{ 0.875000000,  0.750000000},
{ 0.812500000,  0.875000000},
{ 0.812500000,  0.750000000},
{ 0.875000000,  0.750000000},
{ 0.937500000,  0.875000000},
{ 0.875000000,  0.875000000},
{ 0.937500000,  0.750000000},
{ 0.875000000,  0.875000000},
{ 0.875000000,  0.750000000},
{ 0.937500000,  0.750000000},
{ 1.000000000,  0.875000000},
{ 0.937500000,  0.875000000},
{ 1.000000000,  0.750000000},
{ 0.937500000,  0.875000000},
{ 0.937500000,  0.750000000},
{ 1.000000000,  0.750000000},
{ 0.062500000,  0.750000000},
{ 0.000000000,  0.750000000},
{ 0.062500000,  0.625000000},
{ 0.000000000,  0.750000000},
{ 0.000000000,  0.625000000},
{ 0.062500000,  0.625000000},
{ 0.125000000,  0.750000000},
{ 0.062500000,  0.750000000},
{ 0.125000000,  0.625000000},
{ 0.062500000,  0.750000000},
{ 0.062500000,  0.625000000},
{ 0.125000000,  0.625000000},
{ 0.187500000,  0.750000000},
{ 0.125000000,  0.750000000},
{ 0.187500000,  0.625000000},
{ 0.125000000,  0.750000000},
{ 0.125000000,  0.625000000},
{ 0.187500000,  0.625000000},
{ 0.250000000,  0.750000000},
{ 0.187500000,  0.750000000},
{ 0.250000000,  0.625000000},
{ 0.187500000,  0.750000000},
{ 0.187500000,  0.625000000},
{ 0.250000000,  0.625000000},
{ 0.312500000,  0.750000000},
{ 0.250000000,  0.750000000},
{ 0.312500000,  0.625000000},
{ 0.250000000,  0.750000000},
{ 0.250000000,  0.625000000},
{ 0.312500000,  0.625000000},
{ 0.375000000,  0.750000000},
{ 0.312500000,  0.750000000},
{ 0.375000000,  0.625000000},
{ 0.312500000,  0.750000000},
{ 0.312500000,  0.625000000},
{ 0.375000000,  0.625000000},
{ 0.437500000,  0.750000000},
{ 0.375000000,  0.750000000},
{ 0.437500000,  0.625000000},
{ 0.375000000,  0.750000000},
{ 0.375000000,  0.625000000},
{ 0.437500000,  0.625000000},
{ 0.500000000,  0.750000000},
{ 0.437500000,  0.750000000},
{ 0.500000000,  0.625000000},
{ 0.437500000,  0.750000000},
{ 0.437500000,  0.625000000},
{ 0.500000000,  0.625000000},
{ 0.562500000,  0.750000000},
{ 0.500000000,  0.750000000},
{ 0.562500000,  0.625000000},
{ 0.500000000,  0.750000000},
{ 0.500000000,  0.625000000},
{ 0.562500000,  0.625000000},
{ 0.625000000,  0.750000000},
{ 0.562500000,  0.750000000},
{ 0.625000000,  0.625000000},
{ 0.562500000,  0.750000000},
{ 0.562500000,  0.625000000},
{ 0.625000000,  0.625000000},
{ 0.687500000,  0.750000000},
{ 0.625000000,  0.750000000},
{ 0.687500000,  0.625000000},
{ 0.625000000,  0.750000000},
{ 0.625000000,  0.625000000},
{ 0.687500000,  0.625000000},
{ 0.750000000,  0.750000000},
{ 0.687500000,  0.750000000},
{ 0.750000000,  0.625000000},
{ 0.687500000,  0.750000000},
{ 0.687500000,  0.625000000},
{ 0.750000000,  0.625000000},
{ 0.812500000,  0.750000000},
{ 0.750000000,  0.750000000},
{ 0.812500000,  0.625000000},
{ 0.750000000,  0.750000000},
{ 0.750000000,  0.625000000},
{ 0.812500000,  0.625000000},
{ 0.875000000,  0.750000000},
{ 0.812500000,  0.750000000},
{ 0.875000000,  0.625000000},
{ 0.812500000,  0.750000000},
{ 0.812500000,  0.625000000},
{ 0.875000000,  0.625000000},
{ 0.937500000,  0.750000000},
{ 0.875000000,  0.750000000},
{ 0.937500000,  0.625000000},
{ 0.875000000,  0.750000000},
{ 0.875000000,  0.625000000},
{ 0.937500000,  0.625000000},
{ 1.000000000,  0.750000000},
{ 0.937500000,  0.750000000},
{ 1.000000000,  0.625000000},
{ 0.937500000,  0.750000000},
{ 0.937500000,  0.625000000},
{ 1.000000000,  0.625000000},
{ 0.062500000,  0.625000000},
{ 0.000000000,  0.625000000},
{ 0.062500000,  0.500000000},
{ 0.000000000,  0.625000000},
{ 0.000000000,  0.500000000},
{ 0.062500000,  0.500000000},
{ 0.125000000,  0.625000000},
{ 0.062500000,  0.625000000},
{ 0.125000000,  0.500000000},
{ 0.062500000,  0.625000000},
{ 0.062500000,  0.500000000},
{ 0.125000000,  0.500000000},
{ 0.187500000,  0.625000000},
{ 0.125000000,  0.625000000},
{ 0.187500000,  0.500000000},
{ 0.125000000,  0.625000000},
{ 0.125000000,  0.500000000},
{ 0.187500000,  0.500000000},
{ 0.250000000,  0.625000000},
{ 0.187500000,  0.625000000},
{ 0.250000000,  0.500000000},
{ 0.187500000,  0.625000000},
{ 0.187500000,  0.500000000},
{ 0.250000000,  0.500000000},
{ 0.312500000,  0.625000000},
{ 0.250000000,  0.625000000},
{ 0.312500000,  0.500000000},
{ 0.250000000,  0.625000000},
{ 0.250000000,  0.500000000},
{ 0.312500000,  0.500000000},
{ 0.375000000,  0.625000000},
{ 0.312500000,  0.625000000},
{ 0.375000000,  0.500000000},
{ 0.312500000,  0.625000000},
{ 0.312500000,  0.500000000},
{ 0.375000000,  0.500000000},
{ 0.437500000,  0.625000000},
{ 0.375000000,  0.625000000},
{ 0.437500000,  0.500000000},
{ 0.375000000,  0.625000000},
{ 0.375000000,  0.500000000},
{ 0.437500000,  0.500000000},
{ 0.500000000,  0.625000000},
{ 0.437500000,  0.625000000},
{ 0.500000000,  0.500000000},
{ 0.437500000,  0.625000000},
{ 0.437500000,  0.500000000},
{ 0.500000000,  0.500000000},
{ 0.562500000,  0.625000000},
{ 0.500000000,  0.625000000},
{ 0.562500000,  0.500000000},
{ 0.500000000,  0.625000000},
{ 0.500000000,  0.500000000},
{ 0.562500000,  0.500000000},
{ 0.625000000,  0.625000000},
{ 0.562500000,  0.625000000},
{ 0.625000000,  0.500000000},
{ 0.562500000,  0.625000000},
{ 0.562500000,  0.500000000},
{ 0.625000000,  0.500000000},
{ 0.687500000,  0.625000000},
{ 0.625000000,  0.625000000},
{ 0.687500000,  0.500000000},
{ 0.625000000,  0.625000000},
{ 0.625000000,  0.500000000},
{ 0.687500000,  0.500000000},
{ 0.750000000,  0.625000000},
{ 0.687500000,  0.625000000},
{ 0.750000000,  0.500000000},
{ 0.687500000,  0.625000000},
{ 0.687500000,  0.500000000},
{ 0.750000000,  0.500000000},
{ 0.812500000,  0.625000000},
{ 0.750000000,  0.625000000},
{ 0.812500000,  0.500000000},
{ 0.750000000,  0.625000000},
{ 0.750000000,  0.500000000},
{ 0.812500000,  0.500000000},
{ 0.875000000,  0.625000000},
{ 0.812500000,  0.625000000},
{ 0.875000000,  0.500000000},
{ 0.812500000,  0.625000000},
{ 0.812500000,  0.500000000},
{ 0.875000000,  0.500000000},
{ 0.937500000,  0.625000000},
{ 0.875000000,  0.625000000},
{ 0.937500000,  0.500000000},
{ 0.875000000,  0.625000000},
{ 0.875000000,  0.500000000},
{ 0.937500000,  0.500000000},
{ 1.000000000,  0.625000000},
{ 0.937500000,  0.625000000},
{ 1.000000000,  0.500000000},
{ 0.937500000,  0.625000000},
{ 0.937500000,  0.500000000},
{ 1.000000000,  0.500000000},
{ 0.062500000,  0.500000000},
{ 0.000000000,  0.500000000},
{ 0.062500000,  0.375000000},
{ 0.000000000,  0.500000000},
{ 0.000000000,  0.375000000},
{ 0.062500000,  0.375000000},
{ 0.125000000,  0.500000000},
{ 0.062500000,  0.500000000},
{ 0.125000000,  0.375000000},
{ 0.062500000,  0.500000000},
{ 0.062500000,  0.375000000},
{ 0.125000000,  0.375000000},
{ 0.187500000,  0.500000000},
{ 0.125000000,  0.500000000},
{ 0.187500000,  0.375000000},
{ 0.125000000,  0.500000000},
{ 0.125000000,  0.375000000},
{ 0.187500000,  0.375000000},
{ 0.250000000,  0.500000000},
{ 0.187500000,  0.500000000},
{ 0.250000000,  0.375000000},
{ 0.187500000,  0.500000000},
{ 0.187500000,  0.375000000},
{ 0.250000000,  0.375000000},
{ 0.312500000,  0.500000000},
{ 0.250000000,  0.500000000},
{ 0.312500000,  0.375000000},
{ 0.250000000,  0.500000000},
{ 0.250000000,  0.375000000},
{ 0.312500000,  0.375000000},
{ 0.375000000,  0.500000000},
{ 0.312500000,  0.500000000},
{ 0.375000000,  0.375000000},
{ 0.312500000,  0.500000000},
{ 0.312500000,  0.375000000},
{ 0.375000000,  0.375000000},
{ 0.437500000,  0.500000000},
{ 0.375000000,  0.500000000},
{ 0.437500000,  0.375000000},
{ 0.375000000,  0.500000000},
{ 0.375000000,  0.375000000},
{ 0.437500000,  0.375000000},
{ 0.500000000,  0.500000000},
{ 0.437500000,  0.500000000},
{ 0.500000000,  0.375000000},
{ 0.437500000,  0.500000000},
{ 0.437500000,  0.375000000},
{ 0.500000000,  0.375000000},
{ 0.562500000,  0.500000000},
{ 0.500000000,  0.500000000},
{ 0.562500000,  0.375000000},
{ 0.500000000,  0.500000000},
{ 0.500000000,  0.375000000},
{ 0.562500000,  0.375000000},
{ 0.625000000,  0.500000000},
{ 0.562500000,  0.500000000},
{ 0.625000000,  0.375000000},
{ 0.562500000,  0.500000000},
{ 0.562500000,  0.375000000},
{ 0.625000000,  0.375000000},
{ 0.687500000,  0.500000000},
{ 0.625000000,  0.500000000},
{ 0.687500000,  0.375000000},
{ 0.625000000,  0.500000000},
{ 0.625000000,  0.375000000},
{ 0.687500000,  0.375000000},
{ 0.750000000,  0.500000000},
{ 0.687500000,  0.500000000},
{ 0.750000000,  0.375000000},
{ 0.687500000,  0.500000000},
{ 0.687500000,  0.375000000},
{ 0.750000000,  0.375000000},
{ 0.812500000,  0.500000000},
{ 0.750000000,  0.500000000},
{ 0.812500000,  0.375000000},
{ 0.750000000,  0.500000000},
{ 0.750000000,  0.375000000},
{ 0.812500000,  0.375000000},
{ 0.875000000,  0.500000000},
{ 0.812500000,  0.500000000},
{ 0.875000000,  0.375000000},
{ 0.812500000,  0.500000000},
{ 0.812500000,  0.375000000},
{ 0.875000000,  0.375000000},
{ 0.937500000,  0.500000000},
{ 0.875000000,  0.500000000},
{ 0.937500000,  0.375000000},
{ 0.875000000,  0.500000000},
{ 0.875000000,  0.375000000},
{ 0.937500000,  0.375000000},
{ 1.000000000,  0.500000000},
{ 0.937500000,  0.500000000},
{ 1.000000000,  0.375000000},
{ 0.937500000,  0.500000000},
{ 0.937500000,  0.375000000},
{ 1.000000000,  0.375000000},
{ 0.062500000,  0.375000000},
{ 0.000000000,  0.375000000},
{ 0.062500000,  0.250000000},
{ 0.000000000,  0.375000000},
{ 0.000000000,  0.250000000},
{ 0.062500000,  0.250000000},
{ 0.125000000,  0.375000000},
{ 0.062500000,  0.375000000},
{ 0.125000000,  0.250000000},
{ 0.062500000,  0.375000000},
{ 0.062500000,  0.250000000},
{ 0.125000000,  0.250000000},
{ 0.187500000,  0.375000000},
{ 0.125000000,  0.375000000},
{ 0.187500000,  0.250000000},
{ 0.125000000,  0.375000000},
{ 0.125000000,  0.250000000},
{ 0.187500000,  0.250000000},
{ 0.250000000,  0.375000000},
{ 0.187500000,  0.375000000},
{ 0.250000000,  0.250000000},
{ 0.187500000,  0.375000000},
{ 0.187500000,  0.250000000},
{ 0.250000000,  0.250000000},
{ 0.312500000,  0.375000000},
{ 0.250000000,  0.375000000},
{ 0.312500000,  0.250000000},
{ 0.250000000,  0.375000000},
{ 0.250000000,  0.250000000},
{ 0.312500000,  0.250000000},
{ 0.375000000,  0.375000000},
{ 0.312500000,  0.375000000},
{ 0.375000000,  0.250000000},
{ 0.312500000,  0.375000000},
{ 0.312500000,  0.250000000},
{ 0.375000000,  0.250000000},
{ 0.437500000,  0.375000000},
{ 0.375000000,  0.375000000},
{ 0.437500000,  0.250000000},
{ 0.375000000,  0.375000000},
{ 0.375000000,  0.250000000},
{ 0.437500000,  0.250000000},
{ 0.500000000,  0.375000000},
{ 0.437500000,  0.375000000},
{ 0.500000000,  0.250000000},
{ 0.437500000,  0.375000000},
{ 0.437500000,  0.250000000},
{ 0.500000000,  0.250000000},
{ 0.562500000,  0.375000000},
{ 0.500000000,  0.375000000},
{ 0.562500000,  0.250000000},
{ 0.500000000,  0.375000000},
{ 0.500000000,  0.250000000},
{ 0.562500000,  0.250000000},
{ 0.625000000,  0.375000000},
{ 0.562500000,  0.375000000},
{ 0.625000000,  0.250000000},
{ 0.562500000,  0.375000000},
{ 0.562500000,  0.250000000},
{ 0.625000000,  0.250000000},
{ 0.687500000,  0.375000000},
{ 0.625000000,  0.375000000},
{ 0.687500000,  0.250000000},
{ 0.625000000,  0.375000000},
{ 0.625000000,  0.250000000},
{ 0.687500000,  0.250000000},
{ 0.750000000,  0.375000000},
{ 0.687500000,  0.375000000},
{ 0.750000000,  0.250000000},
{ 0.687500000,  0.375000000},
{ 0.687500000,  0.250000000},
{ 0.750000000,  0.250000000},
{ 0.812500000,  0.375000000},
{ 0.750000000,  0.375000000},
{ 0.812500000,  0.250000000},
{ 0.750000000,  0.375000000},
{ 0.750000000,  0.250000000},
{ 0.812500000,  0.250000000},
{ 0.875000000,  0.375000000},
{ 0.812500000,  0.375000000},
{ 0.875000000,  0.250000000},
{ 0.812500000,  0.375000000},
{ 0.812500000,  0.250000000},
{ 0.875000000,  0.250000000},
{ 0.937500000,  0.375000000},
{ 0.875000000,  0.375000000},
{ 0.937500000,  0.250000000},
{ 0.875000000,  0.375000000},
{ 0.875000000,  0.250000000},
{ 0.937500000,  0.250000000},
{ 1.000000000,  0.375000000},
{ 0.937500000,  0.375000000},
{ 1.000000000,  0.250000000},
{ 0.937500000,  0.375000000},
{ 0.937500000,  0.250000000},
{ 1.000000000,  0.250000000},
{ 0.062500000,  0.250000000},
{ 0.000000000,  0.250000000},
{ 0.062500000,  0.125000000},
{ 0.000000000,  0.250000000},
{ 0.000000000,  0.125000000},
{ 0.062500000,  0.125000000},
{ 0.125000000,  0.250000000},
{ 0.062500000,  0.250000000},
{ 0.125000000,  0.125000000},
{ 0.062500000,  0.250000000},
{ 0.062500000,  0.125000000},
{ 0.125000000,  0.125000000},
{ 0.187500000,  0.250000000},
{ 0.125000000,  0.250000000},
{ 0.187500000,  0.125000000},
{ 0.125000000,  0.250000000},
{ 0.125000000,  0.125000000},
{ 0.187500000,  0.125000000},
{ 0.250000000,  0.250000000},
{ 0.187500000,  0.250000000},
{ 0.250000000,  0.125000000},
{ 0.187500000,  0.250000000},
{ 0.187500000,  0.125000000},
{ 0.250000000,  0.125000000},
{ 0.312500000,  0.250000000},
{ 0.250000000,  0.250000000},
{ 0.312500000,  0.125000000},
{ 0.250000000,  0.250000000},
{ 0.250000000,  0.125000000},
{ 0.312500000,  0.125000000},
{ 0.375000000,  0.250000000},
{ 0.312500000,  0.250000000},
{ 0.375000000,  0.125000000},
{ 0.312500000,  0.250000000},
{ 0.312500000,  0.125000000},
{ 0.375000000,  0.125000000},
{ 0.437500000,  0.250000000},
{ 0.375000000,  0.250000000},
{ 0.437500000,  0.125000000},
{ 0.375000000,  0.250000000},
{ 0.375000000,  0.125000000},
{ 0.437500000,  0.125000000},
{ 0.500000000,  0.250000000},
{ 0.437500000,  0.250000000},
{ 0.500000000,  0.125000000},
{ 0.437500000,  0.250000000},
{ 0.437500000,  0.125000000},
{ 0.500000000,  0.125000000},
{ 0.562500000,  0.250000000},
{ 0.500000000,  0.250000000},
{ 0.562500000,  0.125000000},
{ 0.500000000,  0.250000000},
{ 0.500000000,  0.125000000},
{ 0.562500000,  0.125000000},
{ 0.625000000,  0.250000000},
{ 0.562500000,  0.250000000},
{ 0.625000000,  0.125000000},
{ 0.562500000,  0.250000000},
{ 0.562500000,  0.125000000},
{ 0.625000000,  0.125000000},
{ 0.687500000,  0.250000000},
{ 0.625000000,  0.250000000},
{ 0.687500000,  0.125000000},
{ 0.625000000,  0.250000000},
{ 0.625000000,  0.125000000},
{ 0.687500000,  0.125000000},
{ 0.750000000,  0.250000000},
{ 0.687500000,  0.250000000},
{ 0.750000000,  0.125000000},
{ 0.687500000,  0.250000000},
{ 0.687500000,  0.125000000},
{ 0.750000000,  0.125000000},
{ 0.812500000,  0.250000000},
{ 0.750000000,  0.250000000},
{ 0.812500000,  0.125000000},
{ 0.750000000,  0.250000000},
{ 0.750000000,  0.125000000},
{ 0.812500000,  0.125000000},
{ 0.875000000,  0.250000000},
{ 0.812500000,  0.250000000},
{ 0.875000000,  0.125000000},
{ 0.812500000,  0.250000000},
{ 0.812500000,  0.125000000},
{ 0.875000000,  0.125000000},
{ 0.937500000,  0.250000000},
{ 0.875000000,  0.250000000},
{ 0.937500000,  0.125000000},
{ 0.875000000,  0.250000000},
{ 0.875000000,  0.125000000},
{ 0.937500000,  0.125000000},
{ 1.000000000,  0.250000000},
{ 0.937500000,  0.250000000},
{ 1.000000000,  0.125000000},
{ 0.937500000,  0.250000000},
{ 0.937500000,  0.125000000},
{ 1.000000000,  0.125000000},
{ 0.062500000,  0.125000000},
{ 0.000000000,  0.125000000},
{ 0.031250000,  0.000000000},
{ 0.125000000,  0.125000000},
{ 0.062500000,  0.125000000},
{ 0.093750000,  0.000000000},
{ 0.187500000,  0.125000000},
{ 0.125000000,  0.125000000},
{ 0.156250000,  0.000000000},
{ 0.250000000,  0.125000000},
{ 0.187500000,  0.125000000},
{ 0.218750000,  0.000000000},
{ 0.312500000,  0.125000000},
{ 0.250000000,  0.125000000},
{ 0.281250000,  0.000000000},
{ 0.375000000,  0.125000000},
{ 0.312500000,  0.125000000},
{ 0.343750000,  0.000000000},
{ 0.437500000,  0.125000000},
{ 0.375000000,  0.125000000},
{ 0.406250000,  0.000000000},
{ 0.500000000,  0.125000000},
{ 0.437500000,  0.125000000},
{ 0.468750000,  0.000000000},
{ 0.562500000,  0.125000000},
{ 0.500000000,  0.125000000},
{ 0.531250000,  0.000000000},
{ 0.625000000,  0.125000000},
{ 0.562500000,  0.125000000},
{ 0.593750000,  0.000000000},
{ 0.687500000,  0.125000000},
{ 0.625000000,  0.125000000},
{ 0.656250000,  0.000000000},
{ 0.750000000,  0.125000000},
{ 0.687500000,  0.125000000},
{ 0.718750000,  0.000000000},
{ 0.812500000,  0.125000000},
{ 0.750000000,  0.125000000},
{ 0.781250000,  0.000000000},
{ 0.875000000,  0.125000000},
{ 0.812500000,  0.125000000},
{ 0.843750000,  0.000000000},
{ 0.937500000,  0.125000000},
{ 0.875000000,  0.125000000},
{ 0.906250000,  0.000000000},
{ 1.000000000,  0.125000000},
{ 0.937500000,  0.125000000},
{ 0.968750000,  0.000000000},
};

std::vector<Vec3> SphereMesh::normal = {
{ 0.000000000,  1.000000000,  0.000000000},
{-0.382683426,  0.923879504,  0.000000000},
{-0.353553385,  0.923879504,  0.146446615},
{ 0.000000000,  1.000000000,  0.000000000},
{-0.353553385,  0.923879504,  0.146446615},
{-0.270598054,  0.923879504,  0.270598054},
{ 0.000000000,  1.000000000,  0.000000000},
{-0.270598054,  0.923879504,  0.270598054},
{-0.146446615,  0.923879504,  0.353553385},
{ 0.000000000,  1.000000000,  0.000000000},
{-0.146446615,  0.923879504,  0.353553385},
{-0.000000000,  0.923879504,  0.382683426},
{ 0.000000000,  1.000000000,  0.000000000},
{-0.000000000,  0.923879504,  0.382683426},
{ 0.146446615,  0.923879504,  0.353553385},
{ 0.000000000,  1.000000000,  0.000000000},
{ 0.146446615,  0.923879504,  0.353553385},
{ 0.270598054,  0.923879504,  0.270598054},
{ 0.000000000,  1.000000000,  0.000000000},
{ 0.270598054,  0.923879504,  0.270598054},
{ 0.353553385,  0.923879504,  0.146446615},
{ 0.000000000,  1.000000000,  0.000000000},
{ 0.353553385,  0.923879504,  0.146446615},
{ 0.382683426,  0.923879504,  0.000000000},
{ 0.000000000,  1.000000000,  0.000000000},
{ 0.382683426,  0.923879504,  0.000000000},
{ 0.353553385,  0.923879504, -0.146446615},
{ 0.000000000,  1.000000000,  0.000000000},
{ 0.353553385,  0.923879504, -0.146446615},
{ 0.270598054,  0.923879504, -0.270598054},
{ 0.000000000,  1.000000000,  0.000000000},
{ 0.270598054,  0.923879504, -0.270598054},
{ 0.146446615,  0.923879504, -0.353553385},
{ 0.000000000,  1.000000000,  0.000000000},
{ 0.146446615,  0.923879504, -0.353553385},
{ 0.000000000,  0.923879504, -0.382683426},
{ 0.000000000,  1.000000000,  0.000000000},
{ 0.000000000,  0.923879504, -0.382683426},
{-0.146446615,  0.923879504, -0.353553385},
{ 0.000000000,  1.000000000,  0.000000000},
{-0.146446615,  0.923879504, -0.353553385},
{-0.270598054,  0.923879504, -0.270598054},
{ 0.000000000,  1.000000000,  0.000000000},
{-0.270598054,  0.923879504, -0.270598054},
{-0.353553385,  0.923879504, -0.146446615},
{ 0.000000000,  1.000000000,  0.000000000},
{-0.353553385,  0.923879504, -0.146446615},
{-0.382683426,  0.923879504, -0.000000000},
{-0.353553385,  0.923879504,  0.146446615},
{-0.382683426,  0.923879504,  0.000000000},
{-0.653281510,  0.707106769,  0.270598054},
{-0.382683426,  0.923879504,  0.000000000},
{-0.707106769,  0.707106769,  0.000000000},
{-0.653281510,  0.707106769,  0.270598054},
{-0.270598054,  0.923879504,  0.270598054},
{-0.353553385,  0.923879504,  0.146446615},
{-0.500000000,  0.707106769,  0.500000000},
{-0.353553385,  0.923879504,  0.146446615},
{-0.653281510,  0.707106769,  0.270598054},
{-0.500000000,  0.707106769,  0.500000000},
{-0.146446615,  0.923879504,  0.353553385},
{-0.270598054,  0.923879504,  0.270598054},
{-0.270598054,  0.707106769,  0.653281510},
{-0.270598054,  0.923879504,  0.270598054},
{-0.500000000,  0.707106769,  0.500000000},
{-0.270598054,  0.707106769,  0.653281510},
{-0.000000000,  0.923879504,  0.382683426},
{-0.146446615,  0.923879504,  0.353553385},
{-0.000000000,  0.707106769,  0.707106769},
{-0.146446615,  0.923879504,  0.353553385},
{-0.270598054,  0.707106769,  0.653281510},
{-0.000000000,  0.707106769,  0.707106769},
{ 0.146446615,  0.923879504,  0.353553385},
{-0.000000000,  0.923879504,  0.382683426},
{ 0.270598054,  0.707106769,  0.653281510},
{-0.000000000,  0.923879504,  0.382683426},
{-0.000000000,  0.707106769,  0.707106769},
{ 0.270598054,  0.707106769,  0.653281510},
{ 0.270598054,  0.923879504,  0.270598054},
{ 0.146446615,  0.923879504,  0.353553385},
{ 0.500000000,  0.707106769,  0.500000000},
{ 0.146446615,  0.923879504,  0.353553385},
{ 0.270598054,  0.707106769,  0.653281510},
{ 0.500000000,  0.707106769,  0.500000000},
{ 0.353553385,  0.923879504,  0.146446615},
{ 0.270598054,  0.923879504,  0.270598054},
{ 0.653281510,  0.707106769,  0.270598054},
{ 0.270598054,  0.923879504,  0.270598054},
{ 0.500000000,  0.707106769,  0.500000000},
{ 0.653281510,  0.707106769,  0.270598054},
{ 0.382683426,  0.923879504,  0.000000000},
{ 0.353553385,  0.923879504,  0.146446615},
{ 0.707106769,  0.707106769,  0.000000000},
{ 0.353553385,  0.923879504,  0.146446615},
{ 0.653281510,  0.707106769,  0.270598054},
{ 0.707106769,  0.707106769,  0.000000000},
{ 0.353553385,  0.923879504, -0.146446615},
{ 0.382683426,  0.923879504,  0.000000000},
{ 0.653281510,  0.707106769, -0.270598054},
{ 0.382683426,  0.923879504,  0.000000000},
{ 0.707106769,  0.707106769,  0.000000000},
{ 0.653281510,  0.707106769, -0.270598054},
{ 0.270598054,  0.923879504, -0.270598054},
{ 0.353553385,  0.923879504, -0.146446615},
{ 0.500000000,  0.707106769, -0.500000000},
{ 0.353553385,  0.923879504, -0.146446615},
{ 0.653281510,  0.707106769, -0.270598054},
{ 0.500000000,  0.707106769, -0.500000000},
{ 0.146446615,  0.923879504, -0.353553385},
{ 0.270598054,  0.923879504, -0.270598054},
{ 0.270598054,  0.707106769, -0.653281510},
{ 0.270598054,  0.923879504, -0.270598054},
{ 0.500000000,  0.707106769, -0.500000000},
{ 0.270598054,  0.707106769, -0.653281510},
{ 0.000000000,  0.923879504, -0.382683426},
{ 0.146446615,  0.923879504, -0.353553385},
{ 0.000000000,  0.707106769, -0.707106769},
{ 0.146446615,  0.923879504, -0.353553385},
{ 0.270598054,  0.707106769, -0.653281510},
{ 0.000000000,  0.707106769, -0.707106769},
{-0.146446615,  0.923879504, -0.353553385},
{ 0.000000000,  0.923879504, -0.382683426},
{-0.270598054,  0.707106769, -0.653281510},
{ 0.000000000,  0.923879504, -0.382683426},
{ 0.000000000,  0.707106769, -0.707106769},
{-0.270598054,  0.707106769, -0.653281510},
{-0.270598054,  0.923879504, -0.270598054},
{-0.146446615,  0.923879504, -0.353553385},
{-0.500000000,  0.707106769, -0.500000000},
{-0.146446615,  0.923879504, -0.353553385},
{-0.270598054,  0.707106769, -0.653281510},
{-0.500000000,  0.707106769, -0.500000000},
{-0.353553385,  0.923879504, -0.146446615},
{-0.270598054,  0.923879504, -0.270598054},
{-0.653281510,  0.707106769, -0.270598054},
{-0.270598054,  0.923879504, -0.270598054},
{-0.500000000,  0.707106769, -0.500000000},
{-0.653281510,  0.707106769, -0.270598054},
{-0.382683426,  0.923879504, -0.000000000},
{-0.353553385,  0.923879504, -0.146446615},
{-0.707106769,  0.707106769, -0.000000000},
{-0.353553385,  0.923879504, -0.146446615},
{-0.653281510,  0.707106769, -0.270598054},
{-0.707106769,  0.707106769, -0.000000000},
{-0.653281510,  0.707106769,  0.270598054},
{-0.707106769,  0.707106769,  0.000000000},
{-0.853553414,  0.382683426,  0.353553385},
{-0.707106769,  0.707106769,  0.000000000},
{-0.923879504,  0.382683426,  0.000000000},
{-0.853553414,  0.382683426,  0.353553385},
{-0.500000000,  0.707106769,  0.500000000},
{-0.653281510,  0.707106769,  0.270598054},
{-0.653281510,  0.382683426,  0.653281510},
{-0.653281510,  0.707106769,  0.270598054},
{-0.853553414,  0.382683426,  0.353553385},
{-0.653281510,  0.382683426,  0.653281510},
{-0.270598054,  0.707106769,  0.653281510},
{-0.500000000,  0.707106769,  0.500000000},
{-0.353553385,  0.382683426,  0.853553414},
{-0.500000000,  0.707106769,  0.500000000},
{-0.653281510,  0.382683426,  0.653281510},
{-0.353553385,  0.382683426,  0.853553414},
{-0.000000000,  0.707106769,  0.707106769},
{-0.270598054,  0.707106769,  0.653281510},
{-0.000000000,  0.382683426,  0.923879504},
{-0.270598054,  0.707106769,  0.653281510},
{-0.353553385,  0.382683426,  0.853553414},
{-0.000000000,  0.382683426,  0.923879504},
{ 0.270598054,  0.707106769,  0.653281510},
{-0.000000000,  0.707106769,  0.707106769},
{ 0.353553385,  0.382683426,  0.853553414},
{-0.000000000,  0.707106769,  0.707106769},
{-0.000000000,  0.382683426,  0.923879504},
{ 0.353553385,  0.382683426,  0.853553414},
{ 0.500000000,  0.707106769,  0.500000000},
{ 0.270598054,  0.707106769,  0.653281510},
{ 0.653281510,  0.382683426,  0.653281510},
{ 0.270598054,  0.707106769,  0.653281510},
{ 0.353553385,  0.382683426,  0.853553414},
{ 0.653281510,  0.382683426,  0.653281510},
{ 0.653281510,  0.707106769,  0.270598054},
{ 0.500000000,  0.707106769,  0.500000000},
{ 0.853553414,  0.382683426,  0.353553385},
{ 0.500000000,  0.707106769,  0.500000000},
{ 0.653281510,  0.382683426,  0.653281510},
{ 0.853553414,  0.382683426,  0.353553385},
{ 0.707106769,  0.707106769,  0.000000000},
{ 0.653281510,  0.707106769,  0.270598054},
{ 0.923879504,  0.382683426,  0.000000000},
{ 0.653281510,  0.707106769,  0.270598054},
{ 0.853553414,  0.382683426,  0.353553385},
{ 0.923879504,  0.382683426,  0.000000000},
{ 0.653281510,  0.707106769, -0.270598054},
{ 0.707106769,  0.707106769,  0.000000000},
{ 0.853553414,  0.382683426, -0.353553385},
{ 0.707106769,  0.707106769,  0.000000000},
{ 0.923879504,  0.382683426,  0.000000000},
{ 0.853553414,  0.382683426, -0.353553385},
{ 0.500000000,  0.707106769, -0.500000000},
{ 0.653281510,  0.707106769, -0.270598054},
{ 0.653281510,  0.382683426, -0.653281510},
{ 0.653281510,  0.707106769, -0.270598054},
{ 0.853553414,  0.382683426, -0.353553385},
{ 0.653281510,  0.382683426, -0.653281510},
{ 0.270598054,  0.707106769, -0.653281510},
{ 0.500000000,  0.707106769, -0.500000000},
{ 0.353553385,  0.382683426, -0.853553414},
{ 0.500000000,  0.707106769, -0.500000000},
{ 0.653281510,  0.382683426, -0.653281510},
{ 0.353553385,  0.382683426, -0.853553414},
{ 0.000000000,  0.707106769, -0.707106769},
{ 0.270598054,  0.707106769, -0.653281510},
{ 0.000000000,  0.382683426, -0.923879504},
{ 0.270598054,  0.707106769, -0.653281510},
{ 0.353553385,  0.382683426, -0.853553414},
{ 0.000000000,  0.382683426, -0.923879504},
{-0.270598054,  0.707106769, -0.653281510},
{ 0.000000000,  0.707106769, -0.707106769},
{-0.353553385,  0.382683426, -0.853553414},
{ 0.000000000,  0.707106769, -0.707106769},
{ 0.000000000,  0.382683426, -0.923879504},
{-0.353553385,  0.382683426, -0.853553414},
{-0.500000000,  0.707106769, -0.500000000},
{-0.270598054,  0.707106769, -0.653281510},
{-0.653281510,  0.382683426, -0.653281510},
{-0.270598054,  0.707106769, -0.653281510},
{-0.353553385,  0.382683426, -0.853553414},
{-0.653281510,  0.382683426, -0.653281510},
{-0.653281510,  0.707106769, -0.270598054},
{-0.500000000,  0.707106769, -0.500000000},
{-0.853553414,  0.382683426, -0.353553385},
{-0.500000000,  0.707106769, -0.500000000},
{-0.653281510,  0.382683426, -0.653281510},
{-0.853553414,  0.382683426, -0.353553385},
{-0.707106769,  0.707106769, -0.000000000},
{-0.653281510,  0.707106769, -0.270598054},
{-0.923879504,  0.382683426, -0.000000000},
{-0.653281510,  0.707106769, -0.270598054},
{-0.853553414,  0.382683426, -0.353553385},
{-0.923879504,  0.382683426, -0.000000000},
{-0.853553414,  0.382683426,  0.353553385},
{-0.923879504,  0.382683426,  0.000000000},
{-0.923879504,  0.000000000,  0.382683426},
{-0.923879504,  0.382683426,  0.000000000},
{-1.000000000,  0.000000000,  0.000000000},
{-0.923879504,  0.000000000,  0.382683426},
{-0.653281510,  0.382683426,  0.653281510},
{-0.853553414,  0.382683426,  0.353553385},
{-0.707106769,  0.000000000,  0.707106769},
{-0.853553414,  0.382683426,  0.353553385},
{-0.923879504,  0.000000000,  0.382683426},
{-0.707106769,  0.000000000,  0.707106769},
{-0.353553385,  0.382683426,  0.853553414},
{-0.653281510,  0.382683426,  0.653281510},
{-0.382683426,  0.000000000,  0.923879504},
{-0.653281510,  0.382683426,  0.653281510},
{-0.707106769,  0.000000000,  0.707106769},
{-0.382683426,  0.000000000,  0.923879504},
{-0.000000000,  0.382683426,  0.923879504},
{-0.353553385,  0.382683426,  0.853553414},
{-0.000000000,  0.000000000,  1.000000000},
{-0.353553385,  0.382683426,  0.853553414},
{-0.382683426,  0.000000000,  0.923879504},
{-0.000000000,  0.000000000,  1.000000000},
{ 0.353553385,  0.382683426,  0.853553414},
{-0.000000000,  0.382683426,  0.923879504},
{ 0.382683426,  0.000000000,  0.923879504},
{-0.000000000,  0.382683426,  0.923879504},
{-0.000000000,  0.000000000,  1.000000000},
{ 0.382683426,  0.000000000,  0.923879504},
{ 0.653281510,  0.382683426,  0.653281510},
{ 0.353553385,  0.382683426,  0.853553414},
{ 0.707106769,  0.000000000,  0.707106769},
{ 0.353553385,  0.382683426,  0.853553414},
{ 0.382683426,  0.000000000,  0.923879504},
{ 0.707106769,  0.000000000,  0.707106769},
{ 0.853553414,  0.382683426,  0.353553385},
{ 0.653281510,  0.382683426,  0.653281510},
{ 0.923879504,  0.000000000,  0.382683426},
{ 0.653281510,  0.382683426,  0.653281510},
{ 0.707106769,  0.000000000,  0.707106769},
{ 0.923879504,  0.000000000,  0.382683426},
{ 0.923879504,  0.382683426,  0.000000000},
{ 0.853553414,  0.382683426,  0.353553385},
{ 1.000000000,  0.000000000,  0.000000000},
{ 0.853553414,  0.382683426,  0.353553385},
{ 0.923879504,  0.000000000,  0.382683426},
{ 1.000000000,  0.000000000,  0.000000000},
{ 0.853553414,  0.382683426, -0.353553385},
{ 0.923879504,  0.382683426,  0.000000000},
{ 0.923879504,  0.000000000, -0.382683426},
{ 0.923879504,  0.382683426,  0.000000000},
{ 1.000000000,  0.000000000,  0.000000000},
{ 0.923879504,  0.000000000, -0.382683426},
{ 0.653281510,  0.382683426, -0.653281510},
{ 0.853553414,  0.382683426, -0.353553385},
{ 0.707106769,  0.000000000, -0.707106769},
{ 0.853553414,  0.382683426, -0.353553385},
{ 0.923879504,  0.000000000, -0.382683426},
{ 0.707106769,  0.000000000, -0.707106769},
{ 0.353553385,  0.382683426, -0.853553414},
{ 0.653281510,  0.382683426, -0.653281510},
{ 0.382683426,  0.000000000, -0.923879504},
{ 0.653281510,  0.382683426, -0.653281510},
{ 0.707106769,  0.000000000, -0.707106769},
{ 0.382683426,  0.000000000, -0.923879504},
{ 0.000000000,  0.382683426, -0.923879504},
{ 0.353553385,  0.382683426, -0.853553414},
{ 0.000000000,  0.000000000, -1.000000000},
{ 0.353553385,  0.382683426, -0.853553414},
{ 0.382683426,  0.000000000, -0.923879504},
{ 0.000000000,  0.000000000, -1.000000000},
{-0.353553385,  0.382683426, -0.853553414},
{ 0.000000000,  0.382683426, -0.923879504},
{-0.382683426,  0.000000000, -0.923879504},
{ 0.000000000,  0.382683426, -0.923879504},
{ 0.000000000,  0.000000000, -1.000000000},
{-0.382683426,  0.000000000, -0.923879504},
{-0.653281510,  0.382683426, -0.653281510},
{-0.353553385,  0.382683426, -0.853553414},
{-0.707106769,  0.000000000, -0.707106769},
{-0.353553385,  0.382683426, -0.853553414},
{-0.382683426,  0.000000000, -0.923879504},
{-0.707106769,  0.000000000, -0.707106769},
{-0.853553414,  0.382683426, -0.353553385},
{-0.653281510,  0.382683426, -0.653281510},
{-0.923879504,  0.000000000, -0.382683426},
{-0.653281510,  0.382683426, -0.653281510},
{-0.707106769,  0.000000000, -0.707106769},
{-0.923879504,  0.000000000, -0.382683426},
{-0.923879504,  0.382683426, -0.000000000},
{-0.853553414,  0.382683426, -0.353553385},
{-1.000000000,  0.000000000, -0.000000000},
{-0.853553414,  0.382683426, -0.353553385},
{-0.923879504,  0.000000000, -0.382683426},
{-1.000000000,  0.000000000, -0.000000000},
{-0.923879504,  0.000000000,  0.382683426},
{-1.000000000,  0.000000000,  0.000000000},
{-0.853553414, -0.382683426,  0.353553385},
{-1.000000000,  0.000000000,  0.000000000},
{-0.923879504, -0.382683426,  0.000000000},
{-0.853553414, -0.382683426,  0.353553385},
{-0.707106769,  0.000000000,  0.707106769},
{-0.923879504,  0.000000000,  0.382683426},
{-0.653281510, -0.382683426,  0.653281510},
{-0.923879504,  0.000000000,  0.382683426},
{-0.853553414, -0.382683426,  0.353553385},
{-0.653281510, -0.382683426,  0.653281510},
{-0.382683426,  0.000000000,  0.923879504},
{-0.707106769,  0.000000000,  0.707106769},
{-0.353553385, -0.382683426,  0.853553414},
{-0.707106769,  0.000000000,  0.707106769},
{-0.653281510, -0.382683426,  0.653281510},
{-0.353553385, -0.382683426,  0.853553414},
{-0.000000000,  0.000000000,  1.000000000},
{-0.382683426,  0.000000000,  0.923879504},
{-0.000000000, -0.382683426,  0.923879504},
{-0.382683426,  0.000000000,  0.923879504},
{-0.353553385, -0.382683426,  0.853553414},
{-0.000000000, -0.382683426,  0.923879504},
{ 0.382683426,  0.000000000,  0.923879504},
{-0.000000000,  0.000000000,  1.000000000},
{ 0.353553385, -0.382683426,  0.853553414},
{-0.000000000,  0.000000000,  1.000000000},
{-0.000000000, -0.382683426,  0.923879504},
{ 0.353553385, -0.382683426,  0.853553414},
{ 0.707106769,  0.000000000,  0.707106769},
{ 0.382683426,  0.000000000,  0.923879504},
{ 0.653281510, -0.382683426,  0.653281510},
{ 0.382683426,  0.000000000,  0.923879504},
{ 0.353553385, -0.382683426,  0.853553414},
{ 0.653281510, -0.382683426,  0.653281510},
{ 0.923879504,  0.000000000,  0.382683426},
{ 0.707106769,  0.000000000,  0.707106769},
{ 0.853553414, -0.382683426,  0.353553385},
{ 0.707106769,  0.000000000,  0.707106769},
{ 0.653281510, -0.382683426,  0.653281510},
{ 0.853553414, -0.382683426,  0.353553385},
{ 1.000000000,  0.000000000,  0.000000000},
{ 0.923879504,  0.000000000,  0.382683426},
{ 0.923879504, -0.382683426,  0.000000000},
{ 0.923879504,  0.000000000,  0.382683426},
{ 0.853553414, -0.382683426,  0.353553385},
{ 0.923879504, -0.382683426,  0.000000000},
{ 0.923879504,  0.000000000, -0.382683426},
{ 1.000000000,  0.000000000,  0.000000000},
{ 0.853553414, -0.382683426, -0.353553385},
{ 1.000000000,  0.000000000,  0.000000000},
{ 0.923879504, -0.382683426,  0.000000000},
{ 0.853553414, -0.382683426, -0.353553385},
{ 0.707106769,  0.000000000, -0.707106769},
{ 0.923879504,  0.000000000, -0.382683426},
{ 0.653281510, -0.382683426, -0.653281510},
{ 0.923879504,  0.000000000, -0.382683426},
{ 0.853553414, -0.382683426, -0.353553385},
{ 0.653281510, -0.382683426, -0.653281510},
{ 0.382683426,  0.000000000, -0.923879504},
{ 0.707106769,  0.000000000, -0.707106769},
{ 0.353553385, -0.382683426, -0.853553414},
{ 0.707106769,  0.000000000, -0.707106769},
{ 0.653281510, -0.382683426, -0.653281510},
{ 0.353553385, -0.382683426, -0.853553414},
{ 0.000000000,  0.000000000, -1.000000000},
{ 0.382683426,  0.000000000, -0.923879504},
{ 0.000000000, -0.382683426, -0.923879504},
{ 0.382683426,  0.000000000, -0.923879504},
{ 0.353553385, -0.382683426, -0.853553414},
{ 0.000000000, -0.382683426, -0.923879504},
{-0.382683426,  0.000000000, -0.923879504},
{ 0.000000000,  0.000000000, -1.000000000},
{-0.353553385, -0.382683426, -0.853553414},
{ 0.000000000,  0.000000000, -1.000000000},
{ 0.000000000, -0.382683426, -0.923879504},
{-0.353553385, -0.382683426, -0.853553414},
{-0.707106769,  0.000000000, -0.707106769},
{-0.382683426,  0.000000000, -0.923879504},
{-0.653281510, -0.382683426, -0.653281510},
{-0.382683426,  0.000000000, -0.923879504},
{-0.353553385, -0.382683426, -0.853553414},
{-0.653281510, -0.382683426, -0.653281510},
{-0.923879504,  0.000000000, -0.382683426},
{-0.707106769,  0.000000000, -0.707106769},
{-0.853553414, -0.382683426, -0.353553385},
{-0.707106769,  0.000000000, -0.707106769},
{-0.653281510, -0.382683426, -0.653281510},
{-0.853553414, -0.382683426, -0.353553385},
{-1.000000000,  0.000000000, -0.000000000},
{-0.923879504,  0.000000000, -0.382683426},
{-0.923879504, -0.382683426, -0.000000000},
{-0.923879504,  0.000000000, -0.382683426},
{-0.853553414, -0.382683426, -0.353553385},
{-0.923879504, -0.382683426, -0.000000000},
{-0.853553414, -0.382683426,  0.353553385},
{-0.923879504, -0.382683426,  0.000000000},
{-0.653281510, -0.707106769,  0.270598054},
{-0.923879504, -0.382683426,  0.000000000},
{-0.707106769, -0.707106769,  0.000000000},
{-0.653281510, -0.707106769,  0.270598054},
{-0.653281510, -0.382683426,  0.653281510},
{-0.853553414, -0.382683426,  0.353553385},
{-0.500000000, -0.707106769,  0.500000000},
{-0.853553414, -0.382683426,  0.353553385},
{-0.653281510, -0.707106769,  0.270598054},
{-0.500000000, -0.707106769,  0.500000000},
{-0.353553385, -0.382683426,  0.853553414},
{-0.653281510, -0.382683426,  0.653281510},
{-0.270598054, -0.707106769,  0.653281510},
{-0.653281510, -0.382683426,  0.653281510},
{-0.500000000, -0.707106769,  0.500000000},
{-0.270598054, -0.707106769,  0.653281510},
{-0.000000000, -0.382683426,  0.923879504},
{-0.353553385, -0.382683426,  0.853553414},
{-0.000000000, -0.707106769,  0.707106769},
{-0.353553385, -0.382683426,  0.853553414},
{-0.270598054, -0.707106769,  0.653281510},
{-0.000000000, -0.707106769,  0.707106769},
{ 0.353553385, -0.382683426,  0.853553414},
{-0.000000000, -0.382683426,  0.923879504},
{ 0.270598054, -0.707106769,  0.653281510},
{-0.000000000, -0.382683426,  0.923879504},
{-0.000000000, -0.707106769,  0.707106769},
{ 0.270598054, -0.707106769,  0.653281510},
{ 0.653281510, -0.382683426,  0.653281510},
{ 0.353553385, -0.382683426,  0.853553414},
{ 0.500000000, -0.707106769,  0.500000000},
{ 0.353553385, -0.382683426,  0.853553414},
{ 0.270598054, -0.707106769,  0.653281510},
{ 0.500000000, -0.707106769,  0.500000000},
{ 0.853553414, -0.382683426,  0.353553385},
{ 0.653281510, -0.382683426,  0.653281510},
{ 0.653281510, -0.707106769,  0.270598054},
{ 0.653281510, -0.382683426,  0.653281510},
{ 0.500000000, -0.707106769,  0.500000000},
{ 0.653281510, -0.707106769,  0.270598054},
{ 0.923879504, -0.382683426,  0.000000000},
{ 0.853553414, -0.382683426,  0.353553385},
{ 0.707106769, -0.707106769,  0.000000000},
{ 0.853553414, -0.382683426,  0.353553385},
{ 0.653281510, -0.707106769,  0.270598054},
{ 0.707106769, -0.707106769,  0.000000000},
{ 0.853553414, -0.382683426, -0.353553385},
{ 0.923879504, -0.382683426,  0.000000000},
{ 0.653281510, -0.707106769, -0.270598054},
{ 0.923879504, -0.382683426,  0.000000000},
{ 0.707106769, -0.707106769,  0.000000000},
{ 0.653281510, -0.707106769, -0.270598054},
{ 0.653281510, -0.382683426, -0.653281510},
{ 0.853553414, -0.382683426, -0.353553385},
{ 0.500000000, -0.707106769, -0.500000000},
{ 0.853553414, -0.382683426, -0.353553385},
{ 0.653281510, -0.707106769, -0.270598054},
{ 0.500000000, -0.707106769, -0.500000000},
{ 0.353553385, -0.382683426, -0.853553414},
{ 0.653281510, -0.382683426, -0.653281510},
{ 0.270598054, -0.707106769, -0.653281510},
{ 0.653281510, -0.382683426, -0.653281510},
{ 0.500000000, -0.707106769, -0.500000000},
{ 0.270598054, -0.707106769, -0.653281510},
{ 0.000000000, -0.382683426, -0.923879504},
{ 0.353553385, -0.382683426, -0.853553414},
{ 0.000000000, -0.707106769, -0.707106769},
{ 0.353553385, -0.382683426, -0.853553414},
{ 0.270598054, -0.707106769, -0.653281510},
{ 0.000000000, -0.707106769, -0.707106769},
{-0.353553385, -0.382683426, -0.853553414},
{ 0.000000000, -0.382683426, -0.923879504},
{-0.270598054, -0.707106769, -0.653281510},
{ 0.000000000, -0.382683426, -0.923879504},
{ 0.000000000, -0.707106769, -0.707106769},
{-0.270598054, -0.707106769, -0.653281510},
{-0.653281510, -0.382683426, -0.653281510},
{-0.353553385, -0.382683426, -0.853553414},
{-0.500000000, -0.707106769, -0.500000000},
{-0.353553385, -0.382683426, -0.853553414},
{-0.270598054, -0.707106769, -0.653281510},
{-0.500000000, -0.707106769, -0.500000000},
{-0.853553414, -0.382683426, -0.353553385},
{-0.653281510, -0.382683426, -0.653281510},
{-0.653281510, -0.707106769, -0.270598054},
{-0.653281510, -0.382683426, -0.653281510},
{-0.500000000, -0.707106769, -0.500000000},
{-0.653281510, -0.707106769, -0.270598054},
{-0.923879504, -0.382683426, -0.000000000},
{-0.853553414, -0.382683426, -0.353553385},
{-0.707106769, -0.707106769, -0.000000000},
{-0.853553414, -0.382683426, -0.353553385},
{-0.653281510, -0.707106769, -0.270598054},
{-0.707106769, -0.707106769, -0.000000000},
{-0.653281510, -0.707106769,  0.270598054},
{-0.707106769, -0.707106769,  0.000000000},
{-0.353553385, -0.923879504,  0.146446615},
{-0.707106769, -0.707106769,  0.000000000},
{-0.382683426, -0.923879504,  0.000000000},
{-0.353553385, -0.923879504,  0.146446615},
{-0.500000000, -0.707106769,  0.500000000},
{-0.653281510, -0.707106769,  0.270598054},
{-0.270598054, -0.923879504,  0.270598054},
{-0.653281510, -0.707106769,  0.270598054},
{-0.353553385, -0.923879504,  0.146446615},
{-0.270598054, -0.923879504,  0.270598054},
{-0.270598054, -0.707106769,  0.653281510},
{-0.500000000, -0.707106769,  0.500000000},
{-0.146446615, -0.923879504,  0.353553385},
{-0.500000000, -0.707106769,  0.500000000},
{-0.270598054, -0.923879504,  0.270598054},
{-0.146446615, -0.923879504,  0.353553385},
{-0.000000000, -0.707106769,  0.707106769},
{-0.270598054, -0.707106769,  0.653281510},
{-0.000000000, -0.923879504,  0.382683426},
{-0.270598054, -0.707106769,  0.653281510},
{-0.146446615, -0.923879504,  0.353553385},
{-0.000000000, -0.923879504,  0.382683426},
{ 0.270598054, -0.707106769,  0.653281510},
{-0.000000000, -0.707106769,  0.707106769},
{ 0.146446615, -0.923879504,  0.353553385},
{-0.000000000, -0.707106769,  0.707106769},
{-0.000000000, -0.923879504,  0.382683426},
{ 0.146446615, -0.923879504,  0.353553385},
{ 0.500000000, -0.707106769,  0.500000000},
{ 0.270598054, -0.707106769,  0.653281510},
{ 0.270598054, -0.923879504,  0.270598054},
{ 0.270598054, -0.707106769,  0.653281510},
{ 0.146446615, -0.923879504,  0.353553385},
{ 0.270598054, -0.923879504,  0.270598054},
{ 0.653281510, -0.707106769,  0.270598054},
{ 0.500000000, -0.707106769,  0.500000000},
{ 0.353553385, -0.923879504,  0.146446615},
{ 0.500000000, -0.707106769,  0.500000000},
{ 0.270598054, -0.923879504,  0.270598054},
{ 0.353553385, -0.923879504,  0.146446615},
{ 0.707106769, -0.707106769,  0.000000000},
{ 0.653281510, -0.707106769,  0.270598054},
{ 0.382683426, -0.923879504,  0.000000000},
{ 0.653281510, -0.707106769,  0.270598054},
{ 0.353553385, -0.923879504,  0.146446615},
{ 0.382683426, -0.923879504,  0.000000000},
{ 0.653281510, -0.707106769, -0.270598054},
{ 0.707106769, -0.707106769,  0.000000000},
{ 0.353553385, -0.923879504, -0.146446615},
{ 0.707106769, -0.707106769,  0.000000000},
{ 0.382683426, -0.923879504,  0.000000000},
{ 0.353553385, -0.923879504, -0.146446615},
{ 0.500000000, -0.707106769, -0.500000000},
{ 0.653281510, -0.707106769, -0.270598054},
{ 0.270598054, -0.923879504, -0.270598054},
{ 0.653281510, -0.707106769, -0.270598054},
{ 0.353553385, -0.923879504, -0.146446615},
{ 0.270598054, -0.923879504, -0.270598054},
{ 0.270598054, -0.707106769, -0.653281510},
{ 0.500000000, -0.707106769, -0.500000000},
{ 0.146446615, -0.923879504, -0.353553385},
{ 0.500000000, -0.707106769, -0.500000000},
{ 0.270598054, -0.923879504, -0.270598054},
{ 0.146446615, -0.923879504, -0.353553385},
{ 0.000000000, -0.707106769, -0.707106769},
{ 0.270598054, -0.707106769, -0.653281510},
{ 0.000000000, -0.923879504, -0.382683426},
{ 0.270598054, -0.707106769, -0.653281510},
{ 0.146446615, -0.923879504, -0.353553385},
{ 0.000000000, -0.923879504, -0.382683426},
{-0.270598054, -0.707106769, -0.653281510},
{ 0.000000000, -0.707106769, -0.707106769},
{-0.146446615, -0.923879504, -0.353553385},
{ 0.000000000, -0.707106769, -0.707106769},
{ 0.000000000, -0.923879504, -0.382683426},
{-0.146446615, -0.923879504, -0.353553385},
{-0.500000000, -0.707106769, -0.500000000},
{-0.270598054, -0.707106769, -0.653281510},
{-0.270598054, -0.923879504, -0.270598054},
{-0.270598054, -0.707106769, -0.653281510},
{-0.146446615, -0.923879504, -0.353553385},
{-0.270598054, -0.923879504, -0.270598054},
{-0.653281510, -0.707106769, -0.270598054},
{-0.500000000, -0.707106769, -0.500000000},
{-0.353553385, -0.923879504, -0.146446615},
{-0.500000000, -0.707106769, -0.500000000},
{-0.270598054, -0.923879504, -0.270598054},
{-0.353553385, -0.923879504, -0.146446615},
{-0.707106769, -0.707106769, -0.000000000},
{-0.653281510, -0.707106769, -0.270598054},
{-0.382683426, -0.923879504, -0.000000000},
{-0.653281510, -0.707106769, -0.270598054},
{-0.353553385, -0.923879504, -0.146446615},
{-0.382683426, -0.923879504, -0.000000000},
{-0.353553385, -0.923879504,  0.146446615},
{-0.382683426, -0.923879504,  0.000000000},
{-0.000000000, -1.000000000,  0.000000000},
{-0.270598054, -0.923879504,  0.270598054},
{-0.353553385, -0.923879504,  0.146446615},
{-0.000000000, -1.000000000,  0.000000000},
{-0.146446615, -0.923879504,  0.353553385},
{-0.270598054, -0.923879504,  0.270598054},
{-0.000000000, -1.000000000,  0.000000000},
{-0.000000000, -0.923879504,  0.382683426},
{-0.146446615, -0.923879504,  0.353553385},
{-0.000000000, -1.000000000,  0.000000000},
{ 0.146446615, -0.923879504,  0.353553385},
{-0.000000000, -0.923879504,  0.382683426},
{ 0.000000000, -1.000000000,  0.000000000},
{ 0.270598054, -0.923879504,  0.270598054},
{ 0.146446615, -0.923879504,  0.353553385},
{ 0.000000000, -1.000000000,  0.000000000},
{ 0.353553385, -0.923879504,  0.146446615},
{ 0.270598054, -0.923879504,  0.270598054},
{ 0.000000000, -1.000000000,  0.000000000},
{ 0.382683426, -0.923879504,  0.000000000},
{ 0.353553385, -0.923879504,  0.146446615},
{ 0.000000000, -1.000000000,  0.000000000},
{ 0.353553385, -0.923879504, -0.146446615},
{ 0.382683426, -0.923879504,  0.000000000},
{ 0.000000000, -1.000000000, -0.000000000},
{ 0.270598054, -0.923879504, -0.270598054},
{ 0.353553385, -0.923879504, -0.146446615},
{ 0.000000000, -1.000000000, -0.000000000},
{ 0.146446615, -0.923879504, -0.353553385},
{ 0.270598054, -0.923879504, -0.270598054},
{ 0.000000000, -1.000000000, -0.000000000},
{ 0.000000000, -0.923879504, -0.382683426},
{ 0.146446615, -0.923879504, -0.353553385},
{ 0.000000000, -1.000000000, -0.000000000},
{-0.146446615, -0.923879504, -0.353553385},
{ 0.000000000, -0.923879504, -0.382683426},
{-0.000000000, -1.000000000, -0.000000000},
{-0.270598054, -0.923879504, -0.270598054},
{-0.146446615, -0.923879504, -0.353553385},
{-0.000000000, -1.000000000, -0.000000000},
{-0.353553385, -0.923879504, -0.146446615},
{-0.270598054, -0.923879504, -0.270598054},
{-0.000000000, -1.000000000, -0.000000000},
{-0.382683426, -0.923879504, -0.000000000},
{-0.353553385, -0.923879504, -0.146446615},
{-0.000000000, -1.000000000, -0.000000000},
};

}

/* -------------------------------------------------------------------------- */
/* ---- ink/graphics/Gpu.cxx ------------------------------------------------ */
/* -------------------------------------------------------------------------- */

namespace Ink::Gpu {

constexpr uint32_t GL_COMPARISON_FUNCTIONS[] = {
	GL_NEVER,                                                 /**< FUNC_NEVER */
	GL_LESS,                                                  /**< FUNC_LESS */
	GL_EQUAL,                                                 /**< FUNC_EQUAL */
	GL_LEQUAL,                                                /**< FUNC_LEQUAL */
	GL_GREATER,                                               /**< FUNC_GREATER */
	GL_NOTEQUAL,                                              /**< FUNC_NOTEQUAL */
	GL_GEQUAL,                                                /**< FUNC_GEQUAL */
	GL_ALWAYS,                                                /**< FUNC_ALWAYS */
};

constexpr uint32_t GL_STENCIL_OPERATIONS[] = {
	GL_ZERO,                                                  /**< STENCIL_ZERO */
	GL_KEEP,                                                  /**< STENCIL_KEEP */
	GL_REPLACE,                                               /**< STENCIL_REPLACE */
	GL_INCR,                                                  /**< STENCIL_INCR */
	GL_DECR,                                                  /**< STENCIL_DECR */
	GL_INCR_WRAP,                                             /**< STENCIL_INCR_WRAP */
	GL_DECR_WRAP,                                             /**< STENCIL_DECR_WRAP */
	GL_INVERT,                                                /**< STENCIL_INVERT */
};

constexpr uint32_t GL_BLEND_OPERATIONS[] = {
	GL_FUNC_ADD,                                              /**< BLEND_ADD */
	GL_FUNC_SUBTRACT,                                         /**< BLEND_SUBTRACT */
	GL_FUNC_REVERSE_SUBTRACT,                                 /**< BLEND_REVERSE_SUBTRACT */
	GL_MIN,                                                   /**< BLEND_MIN */
	GL_MAX,                                                   /**< BLEND_MAX */
};

constexpr uint32_t GL_BLEND_FACTORS[] = {
	GL_ZERO,                                                  /**< FACTOR_ZERO */
	GL_ONE,                                                   /**< FACTOR_ONE */
	GL_SRC_COLOR,                                             /**< FACTOR_SRC_COLOR */
	GL_ONE_MINUS_SRC_COLOR,                                   /**< FACTOR_ONE_MINUS_SRC_COLOR */
	GL_DST_COLOR,                                             /**< FACTOR_DST_COLOR */
	GL_ONE_MINUS_DST_COLOR,                                   /**< FACTOR_ONE_MINUS_DST_COLOR */
	GL_SRC_ALPHA,                                             /**< FACTOR_SRC_ALPHA */
	GL_ONE_MINUS_SRC_ALPHA,                                   /**< FACTOR_ONE_MINUS_SRC_ALPHA */
	GL_DST_ALPHA,                                             /**< FACTOR_DST_ALPHA */
	GL_ONE_MINUS_DST_ALPHA,                                   /**< FACTOR_ONE_MINUS_DST_ALPHA */
};

constexpr uint32_t GL_RENDER_SIDES[] = {
	GL_FRONT,                                                 /**< FRONT_SIDE */
	GL_BACK,                                                  /**< BACK_SIDE */
	GL_FRONT_AND_BACK,                                        /**< DOUBLE_SIDE */
};

constexpr uint32_t GL_IMAGE_TYPES[] = {
	GL_UNSIGNED_BYTE,                                         /**< IMAGE_UBYTE */
	GL_BYTE,                                                  /**< IMAGE_BYTE */
	GL_UNSIGNED_SHORT,                                        /**< IMAGE_USHORT */
	GL_SHORT,                                                 /**< IMAGE_SHORT */
	GL_UNSIGNED_INT,                                          /**< IMAGE_UINT */
	GL_INT,                                                   /**< IMAGE_INT */
	GL_HALF_FLOAT,                                            /**< IMAGE_HALF_FLOAT */
	GL_FLOAT,                                                 /**< IMAGE_FLOAT */
	GL_UNSIGNED_INT_24_8,                                     /**< IMAGE_UINT_24_8 */
};

constexpr uint32_t GL_IMAGE_COLORS[] = {
	GL_RED,                                                   /**< channel is 1 */
	GL_RG,                                                    /**< channel is 2 */
	GL_RGB,                                                   /**< channel is 3 */
	GL_RGBA,                                                  /**< channel is 4 */
};

constexpr uint32_t GL_IMAGE_COLOR_INTEGERS[] = {
	GL_RED_INTEGER,                                           /**< channel is 1 */
	GL_RG_INTEGER,                                            /**< channel is 2 */
	GL_RGB_INTEGER,                                           /**< channel is 3 */
	GL_RGBA_INTEGER,                                          /**< channel is 4 */
};

constexpr uint32_t GL_IMAGE_FORMATS[] = {
	GL_RGBA,                                                  /**< IMAGE_COLOR */
	GL_RGBA_INTEGER,                                          /**< IMAGE_COLOR_INTEGER */
	GL_DEPTH_COMPONENT,                                       /**< IMAGE_DEPTH */
	GL_STENCIL_INDEX,                                         /**< IMAGE_STENCIL */
	GL_DEPTH_STENCIL,                                         /**< IMAGE_DEPTH_STENCIL */
};

constexpr uint32_t GL_TEXTURE_TYPES[] = {
	GL_TEXTURE_1D,                                            /**< TEXTURE_1D */
	GL_TEXTURE_2D,                                            /**< TEXTURE_2D */
	GL_TEXTURE_3D,                                            /**< TEXTURE_3D */
	GL_TEXTURE_CUBE_MAP,                                      /**< TEXTURE_CUBE */
	GL_TEXTURE_1D_ARRAY,                                      /**< TEXTURE_1D_ARRAY */
	GL_TEXTURE_2D_ARRAY,                                      /**< TEXTURE_2D_ARRAY */
	GL_TEXTURE_CUBE_MAP_ARRAY,                                /**< TEXTURE_CUBE_ARRAY */
};

constexpr std::pair<int32_t, uint32_t> GL_TEXTURE_FORMATS[] = {
	{GL_R8                , GL_RED            },              /**< TEXTURE_R8_UNORM */
	{GL_R8_SNORM          , GL_RED            },              /**< TEXTURE_R8_SNORM */
	{GL_R16               , GL_RED            },              /**< TEXTURE_R16_UNORM */
	{GL_R16_SNORM         , GL_RED            },              /**< TEXTURE_R16_SNORM */
	{GL_RG8               , GL_RG             },              /**< TEXTURE_R8G8_UNORM */
	{GL_RG8_SNORM         , GL_RG             },              /**< TEXTURE_R8G8_SNORM */
	{GL_RG16              , GL_RG             },              /**< TEXTURE_R16G16_UNORM */
	{GL_RG16_SNORM        , GL_RG             },              /**< TEXTURE_R16G16_SNORM */
	{GL_R3_G3_B2          , GL_RGB            },              /**< TEXTURE_R3G3B2_UNORM */
	{GL_RGB4              , GL_RGB            },              /**< TEXTURE_R4G4B4_UNORM */
	{GL_RGB5              , GL_RGB            },              /**< TEXTURE_R5G5B5_UNORM */
	{GL_RGB8              , GL_RGB            },              /**< TEXTURE_R8G8B8_UNORM */
	{GL_RGB8_SNORM        , GL_RGB            },              /**< TEXTURE_R8G8B8_SNORM */
	{GL_RGB10             , GL_RGB            },              /**< TEXTURE_R10G10B10_UNORM */
	{GL_RGB12             , GL_RGB            },              /**< TEXTURE_R12G12B12_UNORM */
	{GL_RGB16_SNORM       , GL_RGB            },              /**< TEXTURE_R16G16B16_UNORM */
	{GL_RGBA2             , GL_RGB            },              /**< TEXTURE_R2G2B2A2_UNORM */
	{GL_RGBA4             , GL_RGB            },              /**< TEXTURE_R4G4B4A4_UNORM */
	{GL_RGB5_A1           , GL_RGBA           },              /**< TEXTURE_R5G5B5A1_UNORM */
	{GL_RGBA8             , GL_RGBA           },              /**< TEXTURE_R8G8B8A8_UNORM */
	{GL_RGBA8_SNORM       , GL_RGBA           },              /**< TEXTURE_R8G8B8A8_SNORM */
	{GL_RGB10_A2          , GL_RGBA           },              /**< TEXTURE_R10G10B10A2_UNORM */
	{GL_RGB10_A2UI        , GL_RGBA           },              /**< TEXTURE_R10G10B10A2_UINT */
	{GL_RGBA12            , GL_RGBA           },              /**< TEXTURE_R12G12B12A12_UNORM */
	{GL_RGBA16            , GL_RGBA           },              /**< TEXTURE_R16G16B16A16_UNORM */
	{GL_SRGB8             , GL_RGB            },              /**< TEXTURE_R8G8B8_SRGB */
	{GL_SRGB8_ALPHA8      , GL_RGBA           },              /**< TEXTURE_R8G8B8A8_SRGB */
	{GL_R16F              , GL_RED            },              /**< TEXTURE_R16_SFLOAT */
	{GL_RG16F             , GL_RG             },              /**< TEXTURE_R16G16_SFLOAT */
	{GL_RGB16F            , GL_RGB            },              /**< TEXTURE_R16G16B16_SFLOAT */
	{GL_RGBA16F           , GL_RGBA           },              /**< TEXTURE_R16G16B16A16_SFLOAT */
	{GL_R32F              , GL_RED            },              /**< TEXTURE_R32_SFLOAT */
	{GL_RG32F             , GL_RG             },              /**< TEXTURE_R32G32_SFLOAT */
	{GL_RGB32F            , GL_RGB            },              /**< TEXTURE_R32G32B32_SFLOAT */
	{GL_RGBA32F           , GL_RGBA           },              /**< TEXTURE_R32G32B32A32_SFLOAT */
	{GL_R11F_G11F_B10F    , GL_RGB            },              /**< TEXTURE_R11G11B10_SFLOAT */
	{GL_RGB9_E5           , GL_RGB            },              /**< TEXTURE_R9G9B9E5_UNORM */
	{GL_R8I               , GL_RED            },              /**< TEXTURE_R8_SINT */
	{GL_R8UI              , GL_RED            },              /**< TEXTURE_R8_UINT */
	{GL_R16I              , GL_RED            },              /**< TEXTURE_R16_SINT */
	{GL_R16UI             , GL_RED            },              /**< TEXTURE_R16_UINT */
	{GL_R32I              , GL_RED            },              /**< TEXTURE_R32_SINT */
	{GL_R32UI             , GL_RED            },              /**< TEXTURE_R32_UINT */
	{GL_RG8I              , GL_RG             },              /**< TEXTURE_R8G8_SINT */
	{GL_RG8UI             , GL_RG             },              /**< TEXTURE_R8G8_UINT */
	{GL_RG16I             , GL_RG             },              /**< TEXTURE_R16G16_SINT */
	{GL_RG16UI            , GL_RG             },              /**< TEXTURE_R16G16_UINT */
	{GL_RG32I             , GL_RG             },              /**< TEXTURE_R32G32_SINT */
	{GL_RG32UI            , GL_RG             },              /**< TEXTURE_R32G32_UINT */
	{GL_RGB8I             , GL_RGB            },              /**< TEXTURE_R8G8B8_SINT */
	{GL_RGB8UI            , GL_RGB            },              /**< TEXTURE_R8G8B8_UINT */
	{GL_RGB16I            , GL_RGB            },              /**< TEXTURE_R16G16B16_SINT */
	{GL_RGB16UI           , GL_RGB            },              /**< TEXTURE_R16G16B16_UINT */
	{GL_RGB32I            , GL_RGB            },              /**< TEXTURE_R32G32B32_SINT */
	{GL_RGB32UI           , GL_RGB            },              /**< TEXTURE_R32G32B32_UINT */
	{GL_RGBA8I            , GL_RGBA           },              /**< TEXTURE_R8G8B8A8_SINT */
	{GL_RGBA8UI           , GL_RGBA           },              /**< TEXTURE_R8G8B8A8_UINT */
	{GL_RGBA16I           , GL_RGBA           },              /**< TEXTURE_R16G16B16A16_SINT */
	{GL_RGBA16UI          , GL_RGBA           },              /**< TEXTURE_R16G16B16A16_UINT */
	{GL_RGBA32I           , GL_RGBA           },              /**< TEXTURE_R32G32B32A32_SINT */
	{GL_RGBA32UI          , GL_RGBA           },              /**< TEXTURE_R32G32B32A32_UINT */
	{GL_DEPTH_COMPONENT16 , GL_DEPTH_COMPONENT},              /**< TEXTURE_D16_UNORM */
	{GL_DEPTH_COMPONENT24 , GL_DEPTH_COMPONENT},              /**< TEXTURE_D24_UNORM */
	{GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT},              /**< TEXTURE_D32_SFLOAT */
	{GL_DEPTH24_STENCIL8  , GL_DEPTH_STENCIL  },              /**< TEXTURE_D24_UNORM_S8_UINT */
	{GL_DEPTH32F_STENCIL8 , GL_DEPTH_STENCIL  },              /**< TEXTURE_D32_SFLOAT_S8_UINT */
};

constexpr int32_t GL_TEXTURE_WRAPPINGS[] = {
	GL_REPEAT,                                                /**< TEXTURE_REPEAT */
	GL_MIRRORED_REPEAT,                                       /**< TEXTURE_MIRRORED_REPEAT */
	GL_CLAMP_TO_EDGE,                                         /**< TEXTURE_CLAMP_TO_EDGE */
	GL_CLAMP_TO_BORDER,                                       /**< TEXTURE_CLAMP_TO_BORDER */
};

constexpr int32_t GL_TEXTURE_FILTERS[] = {
	GL_NEAREST,                                               /**< TEXTURE_NEAREST */
	GL_LINEAR,                                                /**< TEXTURE_LINEAR */
	GL_NEAREST_MIPMAP_NEAREST,                                /**< TEXTURE_NEAREST_MIPMAP_NEAREST */
	GL_LINEAR_MIPMAP_NEAREST,                                 /**< TEXTURE_LINEAR_MIPMAP_NEAREST */
	GL_NEAREST_MIPMAP_LINEAR,                                 /**< TEXTURE_NEAREST_MIPMAP_LINEAR */
	GL_LINEAR_MIPMAP_LINEAR,                                  /**< TEXTURE_LINEAR_MIPMAP_LINEAR */
};

constexpr uint32_t GL_COLOR_ATTACHMENTS[] = {
	GL_COLOR_ATTACHMENT0,                                     /**< target number is 1 */
	GL_COLOR_ATTACHMENT1,                                     /**< target number is 2 */
	GL_COLOR_ATTACHMENT2,                                     /**< target number is 3 */
	GL_COLOR_ATTACHMENT3,                                     /**< target number is 4 */
	GL_COLOR_ATTACHMENT4,                                     /**< target number is 5 */
	GL_COLOR_ATTACHMENT5,                                     /**< target number is 6 */
	GL_COLOR_ATTACHMENT6,                                     /**< target number is 7 */
	GL_COLOR_ATTACHMENT7,                                     /**< target number is 8 */
	GL_COLOR_ATTACHMENT8,                                     /**< target number is 9 */
	GL_COLOR_ATTACHMENT9,                                     /**< target number is 10 */
	GL_COLOR_ATTACHMENT10,                                    /**< target number is 11 */
	GL_COLOR_ATTACHMENT11,                                    /**< target number is 12 */
	GL_COLOR_ATTACHMENT12,                                    /**< target number is 13 */
	GL_COLOR_ATTACHMENT13,                                    /**< target number is 14 */
	GL_COLOR_ATTACHMENT14,                                    /**< target number is 15 */
	GL_COLOR_ATTACHMENT15,                                    /**< target number is 16 */
	GL_COLOR_ATTACHMENT16,                                    /**< target number is 17 */
	GL_COLOR_ATTACHMENT17,                                    /**< target number is 18 */
	GL_COLOR_ATTACHMENT18,                                    /**< target number is 19 */
	GL_COLOR_ATTACHMENT19,                                    /**< target number is 20 */
	GL_COLOR_ATTACHMENT20,                                    /**< target number is 21 */
	GL_COLOR_ATTACHMENT21,                                    /**< target number is 22 */
	GL_COLOR_ATTACHMENT22,                                    /**< target number is 23 */
	GL_COLOR_ATTACHMENT23,                                    /**< target number is 24 */
	GL_COLOR_ATTACHMENT24,                                    /**< target number is 25 */
	GL_COLOR_ATTACHMENT25,                                    /**< target number is 26 */
	GL_COLOR_ATTACHMENT26,                                    /**< target number is 27 */
	GL_COLOR_ATTACHMENT27,                                    /**< target number is 28 */
	GL_COLOR_ATTACHMENT28,                                    /**< target number is 29 */
	GL_COLOR_ATTACHMENT29,                                    /**< target number is 30 */
	GL_COLOR_ATTACHMENT30,                                    /**< target number is 31 */
	GL_COLOR_ATTACHMENT31,                                    /**< target number is 32 */
};

ComparisonFunc get_comparison_function(uint32_t v) {
	if (v == GL_NEVER   ) return FUNC_NEVER;
	if (v == GL_LESS    ) return FUNC_LESS;
	if (v == GL_EQUAL   ) return FUNC_EQUAL;
	if (v == GL_LEQUAL  ) return FUNC_LEQUAL;
	if (v == GL_GREATER ) return FUNC_GREATER;
	if (v == GL_NOTEQUAL) return FUNC_NOTEQUAL;
	if (v == GL_GEQUAL  ) return FUNC_GEQUAL;
	/*   ... GL_ALWAYS */ return FUNC_ALWAYS;
}

StencilOperation get_stencil_operation(uint32_t v) {
	if (v == GL_ZERO     ) return STENCIL_ZERO;
	if (v == GL_KEEP     ) return STENCIL_KEEP;
	if (v == GL_REPLACE  ) return STENCIL_REPLACE;
	if (v == GL_INCR     ) return STENCIL_INCR;
	if (v == GL_DECR     ) return STENCIL_DECR;
	if (v == GL_INCR_WRAP) return STENCIL_INCR_WRAP;
	if (v == GL_DECR_WRAP) return STENCIL_DECR_WRAP;
	/*   ... GL_ZERO    */ return STENCIL_INVERT;
}

BlendOperation get_blend_operation(uint32_t v) {
	if (v == GL_FUNC_ADD             ) return BLEND_ADD;
	if (v == GL_FUNC_SUBTRACT        ) return BLEND_SUBTRACT;
	if (v == GL_FUNC_REVERSE_SUBTRACT) return BLEND_REVERSE_SUBTRACT;
	if (v == GL_MIN                  ) return BLEND_MIN;
	/*   ... GL_MAX                 */ return BLEND_MAX;
}

BlendFactor get_blend_factor(uint32_t v) {
	if (v == GL_ZERO               ) return FACTOR_ZERO;
	if (v == GL_ONE                ) return FACTOR_ONE;
	if (v == GL_SRC_COLOR          ) return FACTOR_SRC_COLOR;
	if (v == GL_ONE_MINUS_SRC_COLOR) return FACTOR_ONE_MINUS_SRC_COLOR;
	if (v == GL_DST_COLOR          ) return FACTOR_DST_COLOR;
	if (v == GL_ONE_MINUS_DST_COLOR) return FACTOR_ONE_MINUS_DST_COLOR;
	if (v == GL_SRC_ALPHA          ) return FACTOR_SRC_ALPHA;
	if (v == GL_ONE_MINUS_SRC_ALPHA) return FACTOR_ONE_MINUS_SRC_ALPHA;
	if (v == GL_DST_ALPHA          ) return FACTOR_DST_ALPHA;
	/* ... GL_ONE_MINUS_DST_ALPHA */ return FACTOR_ONE_MINUS_DST_ALPHA;
}

RenderSide get_render_side(uint32_t v) {
	if (v == GL_FRONT         ) return FRONT_SIDE;
	if (v == GL_BACK          ) return BACK_SIDE;
	/* ... GL_FRONT_AND_BACK */ return DOUBLE_SIDE;
}

Rect::Rect(int w, int h) : width(w), height(h) {}

Rect::Rect(int x, int y, int w, int h) : x(x), y(y), width(w), height(h) {}

std::string State::get_device_info() {
	std::string info = "Vender: ";
	const uint8_t* vender = glGetString(GL_VENDOR);
	info += reinterpret_cast<const char*>(vender);
	info += "\nRenderer: ";
	const uint8_t* renderer = glGetString(GL_RENDERER);
	info += reinterpret_cast<const char*>(renderer);
	info += "\nVersion: ";
	const uint8_t* version = glGetString(GL_VERSION);
	info += reinterpret_cast<const char*>(version);
	info += "\nGLSL Version: ";
	const uint8_t* glsl = glGetString(GL_SHADING_LANGUAGE_VERSION);
	info += reinterpret_cast<const char*>(glsl);
	return info + "\n";
}

void State::finish() {
	glFinish();
}

void State::flush() {
	glFlush();
}

std::string State::get_error() {
	std::string info;
	uint32_t error = glGetError();
	while (error != GL_NO_ERROR) {
		info += "OpenGL Error: " + std::to_string(error) + ": ";
		if (error == GL_INVALID_ENUM) {
			info += "An unacceptable value is specified for an enumerated argument.\n";
		} else if (error == GL_INVALID_VALUE) {
			info += "A numeric argument is out of range.\n";
		} else if (error == GL_INVALID_OPERATION) {
			info += "The specified operation is not allowed in the current state.\n";
		} else if (error == GL_OUT_OF_MEMORY) {
			info += "There is not enough memory left to execute the command.\n";
		} else {
			info += "An unknown error has occurred.\n";
		}
		error = glGetError();
	}
	return info;
}

void State::clear(bool c, bool d, bool s) {
	GLbitfield flag = 0;
	flag |= GL_COLOR_BUFFER_BIT * c;
	flag |= GL_DEPTH_BUFFER_BIT * d;
	flag |= GL_STENCIL_BUFFER_BIT * s;
	glClear(flag);
}

Vec4 State::get_clear_color() {
	Vec4 clear_color;
	glGetFloatv(GL_COLOR_CLEAR_VALUE, &clear_color.x);
	return clear_color;
}

void State::set_clear_color(const Vec3& c, float a) {
	glClearColor(c.x, c.y, c.z, a);
}

void State::set_clear_color(const Vec4& c) {
	glClearColor(c.x, c.y, c.z, c.w);
}

Vec4 State::get_color_writemask() {
	int color_writemasks[4];
	glGetIntegerv(GL_COLOR_WRITEMASK, &color_writemasks[0]);
	return Vec4(color_writemasks[0], color_writemasks[1],
				color_writemasks[2], color_writemasks[3]);
}

void State::set_color_writemask(bool r, bool g, bool b, bool a) {
	glColorMask(r, g, b, a);
}

void State::enable_depth_test() {
	glEnable(GL_DEPTH_TEST);
}

void State::disable_depth_test() {
	glDisable(GL_DEPTH_TEST);
}

double State::get_clear_depth() {
	double clear_depth = 0;
	glGetDoublev(GL_DEPTH_CLEAR_VALUE, &clear_depth);
	return clear_depth;
}

void State::set_clear_depth(double d) {
	glClearDepth(d);
}

bool State::get_depth_writemask() {
	int depth_writemask = 0;
	glGetIntegerv(GL_DEPTH_WRITEMASK, &depth_writemask);
	return depth_writemask;
}

void State::set_depth_writemask(bool m) {
	glDepthMask(m);
}

ComparisonFunc State::get_depth_func() {
	int depth_func = 0;
	glGetIntegerv(GL_DEPTH_FUNC, &depth_func);
	return get_comparison_function(depth_func);
}

void State::set_depth_func(ComparisonFunc f) {
	glDepthFunc(GL_COMPARISON_FUNCTIONS[f]);
}

void State::enable_stencil_test() {
	glEnable(GL_STENCIL_TEST);
}

void State::disable_stencil_test() {
	glDisable(GL_STENCIL_TEST);
}

int State::get_clear_stencil() {
	int clear_stencil = 0;
	glGetIntegerv(GL_STENCIL_CLEAR_VALUE, &clear_stencil);
	return clear_stencil;
}

void State::set_clear_stencil(int s) {
	glClearStencil(s);
}

unsigned int State::get_stencil_writemask() {
	int stencil_writemask = 0;
	glGetIntegerv(GL_STENCIL_WRITEMASK, &stencil_writemask);
	return stencil_writemask;
}

void State::set_stencil_writemask(unsigned int m) {
	glStencilMask(m);
}

ComparisonFunc State::get_stencil_func() {
	int stencil_func = 0;
	glGetIntegerv(GL_STENCIL_FUNC, &stencil_func);
	return get_comparison_function(stencil_func);
}

int State::get_stencil_ref() {
	int stencil_ref = 0;
	glGetIntegerv(GL_STENCIL_REF, &stencil_ref);
	return stencil_ref;
}

int State::get_stencil_mask() {
	int stencil_mask = 0;
	glGetIntegerv(GL_STENCIL_VALUE_MASK, &stencil_mask);
	return stencil_mask;
}

void State::set_stencil_func(ComparisonFunc f, int r, int m) {
	glStencilFunc(GL_COMPARISON_FUNCTIONS[f], r, m);
}

StencilOperation State::get_stencil_fail() {
	int stencil_fail = 0;
	glGetIntegerv(GL_STENCIL_FAIL, &stencil_fail);
	return get_stencil_operation(stencil_fail);
}

StencilOperation State::get_stencil_zfail() {
	int stencil_zfail = 0;
	glGetIntegerv(GL_STENCIL_PASS_DEPTH_FAIL, &stencil_zfail);
	return get_stencil_operation(stencil_zfail);
}

StencilOperation State::get_stencil_zpass() {
	int stencil_zpass = 0;
	glGetIntegerv(GL_STENCIL_PASS_DEPTH_PASS, &stencil_zpass);
	return get_stencil_operation(stencil_zpass);
}

void State::set_stencil_op(StencilOperation f,
						   StencilOperation zf,
						   StencilOperation zp) {
	glStencilOp(GL_STENCIL_OPERATIONS[f],
				GL_STENCIL_OPERATIONS[zf],
				GL_STENCIL_OPERATIONS[zp]);
}

void State::enable_blending() {
	glEnable(GL_BLEND);
}

void State::disable_blending() {
	glDisable(GL_BLEND);
}

BlendOperation State::get_blend_op_rgb() {
	int blend_op_rgb = 0;
	glGetIntegerv(GL_BLEND_EQUATION_RGB, &blend_op_rgb);
	return get_blend_operation(blend_op_rgb);
}

BlendOperation State::get_blend_op_alpha() {
	int blend_op_alpha = 0;
	glGetIntegerv(GL_BLEND_EQUATION_ALPHA, &blend_op_alpha);
	return get_blend_operation(blend_op_alpha);
}

void State::set_blend_op(BlendOperation o) {
	glBlendEquation(GL_BLEND_OPERATIONS[o]);
}

void State::set_blend_op(BlendOperation rgb, BlendOperation a) {
	glBlendEquationSeparate(GL_BLEND_OPERATIONS[rgb], GL_BLEND_OPERATIONS[a]);
}

BlendFactor State::get_blend_src_rgb() {
	int blend_src_rgb = 0;
	glGetIntegerv(GL_BLEND_SRC_RGB, &blend_src_rgb);
	return get_blend_factor(blend_src_rgb);
}

BlendFactor State::get_blend_src_alpha() {
	int blend_src_alpha = 0;
	glGetIntegerv(GL_BLEND_SRC_ALPHA, &blend_src_alpha);
	return get_blend_factor(blend_src_alpha);
}

BlendFactor State::get_blend_dst_rgb() {
	int blend_dst_rgb = 0;
	glGetIntegerv(GL_BLEND_DST_RGB, &blend_dst_rgb);
	return get_blend_factor(blend_dst_rgb);
}

BlendFactor State::get_blend_dst_alpha() {
	int blend_dst_alpha = 0;
	glGetIntegerv(GL_BLEND_DST_ALPHA, &blend_dst_alpha);
	return get_blend_factor(blend_dst_alpha);
}

void State::set_blend_factor(BlendFactor s, BlendFactor d) {
	glBlendFunc(GL_BLEND_FACTORS[s], GL_BLEND_FACTORS[d]);
}

void State::set_blend_factor(BlendFactor sr, BlendFactor dr,
							 BlendFactor sa, BlendFactor da) {
	glBlendFuncSeparate(GL_BLEND_FACTORS[sr], GL_BLEND_FACTORS[dr],
						GL_BLEND_FACTORS[sa], GL_BLEND_FACTORS[da]);
}

Rect State::get_viewport() {
	Rect viewport_rect;
	glGetIntegerv(GL_VIEWPORT, &viewport_rect.x);
	return viewport_rect;
}

void State::set_viewport(const Rect& v) {
	glViewport(v.x, v.y, v.width, v.height);
}

void State::enable_scissor_test() {
	glEnable(GL_SCISSOR_TEST);
}

void State::disable_scissor_test() {
	glDisable(GL_SCISSOR_TEST);
}

Rect State::get_scissor() {
	Rect scissor_rect;
	glGetIntegerv(GL_SCISSOR_BOX, &scissor_rect.x);
	return scissor_rect;
}

void State::set_scissor(const Rect& s) {
	glScissor(s.x, s.y, s.width, s.height);
}

void State::enable_wireframe() {
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void State::disable_wireframe() {
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void State::enable_culling() {
	glEnable(GL_CULL_FACE);
}

void State::disable_culling() {
	glDisable(GL_CULL_FACE);
}

RenderSide State::get_cull_side() {
	int cull_side = 0;
	glGetIntegerv(GL_CULL_FACE, &cull_side);
	return get_render_side(cull_side);
}

void State::set_cull_side(RenderSide s) {
	glCullFace(GL_RENDER_SIDES[s]);
}

void State::enable_polygon_offset() {
	glEnable(GL_POLYGON_OFFSET_FILL);
}

void State::disable_polygon_offset() {
	glDisable(GL_POLYGON_OFFSET_FILL);
}

float State::get_polygon_offset_factor() {
	float polygon_offset_factor = 0;
	glGetFloatv(GL_POLYGON_OFFSET_FACTOR, &polygon_offset_factor);
	return polygon_offset_factor;
}

float State::get_polygon_offset_units() {
	float polygon_offset_units = 0;
	glGetFloatv(GL_POLYGON_OFFSET_UNITS, &polygon_offset_units);
	return polygon_offset_units;
}

void State::set_polygon_offset(float f, float u) {
	glPolygonOffset(f, u);
}

void State::enable_dithering() {
	glEnable(GL_DITHER);
}

void State::disable_dithering() {
	glDisable(GL_DITHER);
}

void State::enable_multisample() {
	glEnable(GL_MULTISAMPLE);
}

void State::disable_multisample() {
	glDisable(GL_MULTISAMPLE);
}

void State::enable_alpha_to_coverage() {
	glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
}

void State::disable_alpha_to_coverage() {
	glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
}

void State::enable_texture_cube_seamless() {
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}

void State::disable_texture_cube_seamless() {
	glDisable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}

void MaterialState::set_depth(const Material& m) {
	if (!m.depth_test) return State::disable_depth_test();
	State::enable_depth_test();
	State::set_depth_func(m.depth_func);
}

void MaterialState::set_stencil(const Material& m) {
	if (!m.stencil_test) return State::disable_stencil_test();
	State::enable_stencil_test();
	State::set_stencil_writemask(m.stencil_writemask);
	State::set_stencil_func(m.stencil_func, m.stencil_ref, m.stencil_mask);
	State::set_stencil_op(m.stencil_fail, m.stencil_zfail, m.stencil_zpass);
}

void MaterialState::set_blending(const Material& m) {
	if (!m.blending) return State::disable_blending();
	State::enable_blending();
	State::set_blend_op(m.blend_op_rgb, m.blend_op_alpha);
	State::set_blend_factor(m.blend_src_rgb, m.blend_dst_rgb, m.blend_src_alpha, m.blend_dst_alpha);
}

void MaterialState::set_wireframe(const Material& m) {
	if (!m.wireframe) return State::disable_wireframe();
	State::enable_wireframe();
}

void MaterialState::set_side(const Material& m) {
	if (m.side == FRONT_SIDE) {
		State::enable_culling();
		State::set_cull_side(BACK_SIDE);
	} else if (m.side == BACK_SIDE) {
		State::enable_culling();
		State::set_cull_side(FRONT_SIDE);
	} else if (m.side == DOUBLE_SIDE) {
		State::disable_culling();
	}
}

void MaterialState::set_shadow_side(const Material& m) {
	if (m.shadow_side == FRONT_SIDE) {
		State::enable_culling();
		State::set_cull_side(BACK_SIDE);
	} else if (m.shadow_side == BACK_SIDE) {
		State::enable_culling();
		State::set_cull_side(FRONT_SIDE);
	} else if (m.shadow_side == DOUBLE_SIDE) {
		State::disable_culling();
	}
}

Shader::Shader() {
	program = glCreateProgram();
}

Shader::~Shader() {
	glDeleteProgram(program);
}

void Shader::load_vert(const char* s) {
	vert_shader = s;
}

void Shader::load_vert(const std::string& s) {
	vert_shader = s;
}

void Shader::load_geom(const char* s) {
	geom_shader = s;
}

void Shader::load_geom(const std::string& s) {
	geom_shader = s;
}

void Shader::load_frag(const char* s) {
	frag_shader = s;
}

void Shader::load_frag(const std::string& s) {
	frag_shader = s;
}

void Shader::load_vert_file(const std::string& p) {
	vert_shader = File::read(p);
}

void Shader::load_geom_file(const std::string& p) {
	geom_shader = File::read(p);
}

void Shader::load_frag_file(const std::string& p) {
	frag_shader = File::read(p);
}

void Shader::compile() const {
	compile_shaders();
}

void Shader::use_program() const {
	glUseProgram(program);
}

void Shader::set_defines(const Defines& d) {
	defines = d.get();
}

void Shader::set_uniform_i(const std::string& n, int v) const {
	glUniform1i(glGetUniformLocation(program, n.c_str()), v);
}

void Shader::set_uniform_u(const std::string& n, unsigned int v) const {
	glUniform1ui(glGetUniformLocation(program, n.c_str()), v);
}

void Shader::set_uniform_f(const std::string& n, float v) const {
	glUniform1f(glGetUniformLocation(program, n.c_str()), v);
}

void Shader::set_uniform_v2(const std::string& n, const Vec2& v) const {
	glUniform2fv(glGetUniformLocation(program, n.c_str()), 1, &v.x);
}

void Shader::set_uniform_v3(const std::string& n, const Vec3& v) const {
	glUniform3fv(glGetUniformLocation(program, n.c_str()), 1, &v.x);
}

void Shader::set_uniform_v4(const std::string& n, const Vec4& v) const {
	glUniform4fv(glGetUniformLocation(program, n.c_str()), 1, &v.x);
}

void Shader::set_uniform_m2(const std::string& n, const Mat2& v) const {
	glUniformMatrix2fv(glGetUniformLocation(program, n.c_str()), 1, GL_TRUE, v[0]);
}

void Shader::set_uniform_m3(const std::string& n, const Mat3& v) const {
	glUniformMatrix3fv(glGetUniformLocation(program, n.c_str()), 1, GL_TRUE, v[0]);
}

void Shader::set_uniform_m4(const std::string& n, const Mat4& v) const {
	glUniformMatrix4fv(glGetUniformLocation(program, n.c_str()), 1, GL_TRUE, v[0]);
}

void Shader::set_uniforms(const Uniforms& u) const {
	auto* data_f = u.get_data();
	auto* data_i = reinterpret_cast<const int*>(data_f);
	auto* data_u = reinterpret_cast<const unsigned int*>(data_f);
	size_t uniform_count = u.get_count();
	for (int i = 0; i < uniform_count; ++i) {
		std::string name = u.get_name(i);
		int32_t gl_location = glGetUniformLocation(program, name.c_str());
		int type = u.get_type(i);
		int location = u.get_location(i);
		if (type == 0 /* int */ ) {
			glUniform1i(gl_location, data_i[location]);
		} else if (type == 1 /* unsigned int */ ) {
			glUniform1ui(gl_location, data_u[location]);
		} else if (type == 2 /* float */ ) {
			glUniform1f(gl_location, data_f[location]);
		} else if (type == 3 /* Vec2 */ ) {
			glUniform2fv(gl_location, 1, data_f + location);
		} else if (type == 4 /* Vec3 */ ) {
			glUniform3fv(gl_location, 1, data_f + location);
		} else if (type == 5 /* Vec4 */ ) {
			glUniform4fv(gl_location, 1, data_f + location);
		} else if (type == 6 /* Mat2 */ ) {
			glUniformMatrix2fv(gl_location, 1, GL_TRUE, data_f + location);
		} else if (type == 7 /* Mat3 */ ) {
			glUniformMatrix3fv(gl_location, 1, GL_TRUE, data_f + location);
		} else if (type == 8 /* Mat4 */ ) {
			glUniformMatrix4fv(gl_location, 1, GL_TRUE, data_f + location);
		} else {
			Error::set("Shader", "Unknown uniform variable type");
		}
	}
}

void Shader::set_glsl_version(const std::string& v) {
	glsl_version = v;
}

uint32_t Shader::compile_shader(const std::string& s, int32_t t) const {
	std::string shader_string = s;
	resolve_defines(shader_string);
	resolve_version(shader_string);
	uint32_t shader_id = glCreateShader(t);
	const char* shader_str = shader_string.c_str();
	glShaderSource(shader_id, 1, &shader_str, nullptr);
	glCompileShader(shader_id);
	std::string info = get_compile_info(shader_id, t);
	if (!info.empty()) {
		Error::set("Shader", get_error_info(info, shader_string));
	}
	glAttachShader(program, shader_id);
	return shader_id;
}

void Shader::compile_shaders() const {
	/* compile vertex shader */
	bool use_vert_shader = !vert_shader.empty();
	uint32_t vert_id = 0;
	if (use_vert_shader) {
		vert_id = compile_shader(vert_shader, GL_VERTEX_SHADER);
	} else {
		return Error::set("Shader", "Vertex shader is missing");
	}
	
	/* compile geometry shader */
	bool use_geom_shader = !geom_shader.empty();
	uint32_t geom_id = 0;
	if (use_geom_shader) {
		geom_id = compile_shader(geom_shader, GL_GEOMETRY_SHADER);
	}
	
	/* compile fragment shader */
	bool use_frag_shader = !frag_shader.empty();
	uint32_t frag_id = 0;
	if (use_frag_shader) {
		frag_id = compile_shader(frag_shader, GL_FRAGMENT_SHADER);
	} else {
		return Error::set("Shader", "Fragment shader is missing");
	}
	
	/* link shaders to program */
	glLinkProgram(program);
	std::string info = get_link_info();
	if (!info.empty()) std::cerr << info;
	
	/* delete vertex shader */
	glDeleteShader(vert_id);
	
	/* delete geometry shader */
	if (use_frag_shader) glDeleteShader(geom_id);
	
	/* delete fragment shader */
	glDeleteShader(frag_id);
}

std::string Shader::get_link_info() const {
	int32_t success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (success == GL_TRUE) return "";
	char info[1024];
	glGetProgramInfoLog(program, 1024, nullptr, info);
	return std::string("Shader: Link error\n") + info;
}

void Shader::resolve_defines(std::string& s) const {
	s = defines + s;
}

void Shader::resolve_version(std::string& s) {
	s = "#version " + glsl_version + "\n" + s;
}

std::string Shader::get_compile_info(uint32_t s, uint32_t t) {
	int32_t success;
	glGetShaderiv(s, GL_COMPILE_STATUS, &success);
	if (success == GL_TRUE) return "";
	char info[1024];
	glGetShaderInfoLog(s, 1024, nullptr, info);
	if (t == GL_VERTEX_SHADER) {
		return std::string("Shader: Vertex shader compile error\n") + info;
	} else if (t == GL_GEOMETRY_SHADER) {
		return std::string("Shader: Geometry shader compile error\n") + info;
	} else if (t == GL_FRAGMENT_SHADER) {
		return std::string("Shader: Fragment shader compile error\n") + info;
	}
	return "Shader: No such shader";
}

std::string Shader::get_error_info(const std::string& c, const std::string& s) {
	size_t line_begin = 0;
	size_t line_end = -1;
	
	/* initialize error information and error number */
	std::string info;
	int error_number = 0;
	
	while (true) {
		/* search the string of each line */
		line_begin = line_end + 1;
		line_end = c.find('\n', line_begin);
		if (line_end == -1) break;
		size_t line_length = line_end - line_begin;
		std::string line = c.substr(line_begin, line_length);
		info += line + '\n';
		if (line.substr(0, 9) != "ERROR: 0:") continue;
		
		/* get the line number from error information */
		size_t number_begin = line.find(':', 7) + 1;
		size_t number_end = c.find(':', number_begin);
		size_t number_length = number_end - number_begin;
		int line_number = std::stoi(line.substr(number_begin, number_length));
		
		/* search the code where the error occurred */
		size_t error_begin = 0;
		size_t error_end = -1;
		for (int i = 0; i < line_number; ++i) {
			error_begin = error_end + 1;
			error_end = s.find('\n', error_begin);
			if (error_end == -1) break;
		}
		size_t error_length = error_end - error_begin;
		std::string error_line = s.substr(error_begin, error_length);
		
		/* add code line to error information */
		info += error_line + "\n\n";
		++error_number;
	}
	
	/* count how many errors have occurred */
	info += std::to_string(error_number);
	info += error_number == 1 ? " error generated.\n" : " errors generated.\n";
	
	return info; /* return error infomation */
}

std::string Shader::glsl_version = "410";

VertexObject::VertexObject() {
	glGenVertexArrays(1, &id);
	glGenBuffers(1, &buffer_id);
}

VertexObject::~VertexObject() {
	glDeleteVertexArrays(1, &id);
	glDeleteBuffers(1, &buffer_id);
}

void VertexObject::load(const Mesh& m, const MeshGroup& g) {
	auto& vertex = m.vertex;
	auto& normal = m.normal;
	auto& uv = m.uv;
	auto& tangent = m.tangent;
	auto& color = m.color;
	
	/* check whether to use attributes */
	bool has_normal = !normal.empty();
	bool has_uv = !uv.empty();
	bool has_tangent = !tangent.empty();
	bool has_color = !color.empty();
	
	/* calculate length and stride */
	length = g.length;
	int group_end = g.position + g.length;
	int stride = 3;
	if (has_normal) stride += 3;
	if (has_uv) stride += 2;
	if (has_tangent) stride += 4;
	if (has_color) stride += 3;
	
	/* pack attributes' data into one vector */
	std::vector<float> data(length * stride);
	/* has_vertex */ {
		names = {"vertex"};
		sizes = {3};
		locations = {0};
		auto* data_ptr = data.data() + locations.back();
		for (int i = g.position; i < group_end; ++i) {
			data_ptr[0] = vertex[i].x;
			data_ptr[1] = vertex[i].y;
			data_ptr[2] = vertex[i].z;
			data_ptr += stride;
		}
		locations.emplace_back(locations.back() + 3);
	}
	if (has_normal) {
		names.emplace_back("normal");
		sizes.emplace_back(3);
		auto* data_ptr = data.data() + locations.back();
		for (int i = g.position; i < group_end; ++i) {
			data_ptr[0] = normal[i].x;
			data_ptr[1] = normal[i].y;
			data_ptr[2] = normal[i].z;
			data_ptr += stride;
		}
		locations.emplace_back(locations.back() + 3);
	}
	if (has_uv) {
		names.emplace_back("uv");
		sizes.emplace_back(2);
		auto* data_ptr = data.data() + locations.back();
		for (int i = g.position; i < group_end; ++i) {
			data_ptr[0] = uv[i].x;
			data_ptr[1] = uv[i].y;
			data_ptr += stride;
		}
		locations.emplace_back(locations.back() + 2);
	}
	if (has_tangent) {
		names.emplace_back("tangent");
		sizes.emplace_back(4);
		auto* data_ptr = data.data() + locations.back();
		for (int i = g.position; i < group_end; ++i) {
			data_ptr[0] = tangent[i].x;
			data_ptr[1] = tangent[i].y;
			data_ptr[2] = tangent[i].z;
			data_ptr[3] = tangent[i].w;
			data_ptr += stride;
		}
		locations.emplace_back(locations.back() + 4);
	}
	if (has_color) {
		names.emplace_back("color");
		sizes.emplace_back(3);
		auto* data_ptr = data.data() + locations.back();
		for (int i = g.position; i < group_end; ++i) {
			data_ptr[0] = color[i].x;
			data_ptr[1] = color[i].y;
			data_ptr[2] = color[i].z;
			data_ptr += stride;
		}
		locations.emplace_back(locations.back() + 3);
	}
	
	/* upload data to GPU */
	glBindVertexArray(id);
	glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * data.size(), data.data(), GL_STATIC_DRAW);
}

void VertexObject::attach(const Shader& s) const {
	int stride = locations.back();
	glBindVertexArray(id);
	glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
	for (int i = 0; i < sizes.size(); ++i) {
		int32_t attrib = glGetAttribLocation(s.program, names[i].c_str());
		if (attrib == -1) continue;
		void* pointer = reinterpret_cast<void*>(sizeof(float) * locations[i]);
		glVertexAttribPointer(attrib, sizes[i], GL_FLOAT, GL_FALSE, sizeof(float) * stride, pointer);
		glEnableVertexAttribArray(attrib);
	}
}

void VertexObject::render() const {
	glBindVertexArray(id);
	glDrawArrays(GL_TRIANGLES, 0, length);
}

Texture::Texture() {
	glGenTextures(1, &id);
}

Texture::~Texture() {
	glDeleteTextures(1, &id);
}

void Texture::init_1d(int w, TextureFormat f, ImageType t) {
	int32_t internal = GL_TEXTURE_FORMATS[f].first;
	uint32_t external = GL_TEXTURE_FORMATS[f].second;
	uint32_t data = GL_IMAGE_TYPES[t];
	glBindTexture(GL_TEXTURE_1D, id);
	glTexImage1D(GL_TEXTURE_1D, 0, internal, w, 0, external, data, nullptr);
	set_dimensions(w, 0, 0);
	set_parameters(TEXTURE_1D, f);
}

void Texture::init_2d(int w, int h, TextureFormat f, ImageType t) {
	int32_t internal = GL_TEXTURE_FORMATS[f].first;
	uint32_t external = GL_TEXTURE_FORMATS[f].second;
	uint32_t data = GL_IMAGE_TYPES[t];
	glBindTexture(GL_TEXTURE_2D, id);
	glTexImage2D(GL_TEXTURE_2D, 0, internal, w, h, 0, external, data, nullptr);
	set_dimensions(w, h, 0);
	set_parameters(TEXTURE_2D, f);
}

void Texture::init_2d(const Image& i, TextureFormat f, ImageFormat t) {
	int32_t internal = GL_TEXTURE_FORMATS[f].first;
	uint32_t external = GL_IMAGE_FORMATS[t];
	uint32_t data = GL_IMAGE_TYPES[i.bytes == 1 ? IMAGE_UBYTE : IMAGE_FLOAT];
	if (external == GL_RGBA) external = GL_IMAGE_COLORS[i.channel - 1];
	if (external == GL_RGBA_INTEGER) external = GL_IMAGE_COLOR_INTEGERS[i.channel - 1];
	glBindTexture(GL_TEXTURE_2D, id);
	glTexImage2D(GL_TEXTURE_2D, 0, internal, i.width, i.height, 0, external, data, i.data.data());
	set_dimensions(i.width, i.height, 0);
	set_parameters(TEXTURE_2D, f);
}

void Texture::init_3d(int w, int h, int d, TextureFormat f, ImageType t) {
	int32_t internal = GL_TEXTURE_FORMATS[f].first;
	uint32_t external = GL_TEXTURE_FORMATS[f].second;
	uint32_t data = GL_IMAGE_TYPES[t];
	glBindTexture(GL_TEXTURE_3D, id);
	glTexImage3D(GL_TEXTURE_3D, 0, internal, w, h, d, 0, external, data, nullptr);
	set_dimensions(w, h, d);
	set_parameters(TEXTURE_3D, f);
}

void Texture::init_cube(int w, int h, TextureFormat f, ImageType t) {
	int32_t internal = GL_TEXTURE_FORMATS[f].first;
	uint32_t external = GL_TEXTURE_FORMATS[f].second;
	uint32_t data = GL_IMAGE_TYPES[t];
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);
	for (int i = 0; i < 6; ++i) {
		uint32_t target = GL_TEXTURE_CUBE_MAP_POSITIVE_X + i;
		glTexImage2D(target, 0, internal, w, h, 0, external, data, nullptr);
	}
	set_dimensions(w, h, 0);
	set_parameters(TEXTURE_CUBE, f);
}

void Texture::init_cube(const Image& px, const Image& nx, const Image& py, const Image& ny,
						const Image& pz, const Image& nz, TextureFormat f, ImageFormat t) {
	int32_t internal = GL_TEXTURE_FORMATS[f].first;
	uint32_t external = GL_IMAGE_FORMATS[t];
	uint32_t data = GL_IMAGE_TYPES[px.bytes == 1 ? IMAGE_UBYTE : IMAGE_FLOAT];
	if (external == GL_RGBA) external = GL_IMAGE_COLORS[px.channel - 1];
	if (external == GL_RGBA_INTEGER) external = GL_IMAGE_COLOR_INTEGERS[px.channel - 1];
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);
	uint32_t target = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
	glTexImage2D(target, 0, internal, px.width, px.height, 0, external, data, px.data.data());
	target = GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
	glTexImage2D(target, 0, internal, nx.width, nx.height, 0, external, data, nx.data.data());
	target = GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
	glTexImage2D(target, 0, internal, py.width, py.height, 0, external, data, py.data.data());
	target = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
	glTexImage2D(target, 0, internal, ny.width, ny.height, 0, external, data, ny.data.data());
	target = GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
	glTexImage2D(target, 0, internal, pz.width, pz.height, 0, external, data, pz.data.data());
	target = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
	glTexImage2D(target, 0, internal, nz.width, nz.height, 0, external, data, nz.data.data());
	set_dimensions(px.width, px.height, 0);
	set_parameters(TEXTURE_CUBE, f);
}

void Texture::init_1d_array(int w, int l, TextureFormat f, ImageType t) {
	int32_t internal = GL_TEXTURE_FORMATS[f].first;
	uint32_t external = GL_TEXTURE_FORMATS[f].second;
	uint32_t data = GL_IMAGE_TYPES[t];
	glBindTexture(GL_TEXTURE_1D_ARRAY, id);
	glTexImage2D(GL_TEXTURE_1D_ARRAY, 0, internal, w, l, 0, external, data, nullptr);
	set_dimensions(w, l, 0);
	set_parameters(TEXTURE_1D_ARRAY, f);
}

void Texture::init_2d_array(int w, int h, int l, TextureFormat f, ImageType t) {
	int32_t internal = GL_TEXTURE_FORMATS[f].first;
	uint32_t external = GL_TEXTURE_FORMATS[f].second;
	uint32_t data = GL_IMAGE_TYPES[t];
	glBindTexture(GL_TEXTURE_2D_ARRAY, id);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, internal, w, h, l, 0, external, data, nullptr);
	set_dimensions(w, h, l);
	set_parameters(TEXTURE_2D_ARRAY, f);
}

void Texture::init_cube_array(int w, int h, int l, TextureFormat f, ImageType t) {
	int32_t internal = GL_TEXTURE_FORMATS[f].first;
	uint32_t external = GL_TEXTURE_FORMATS[f].second;
	uint32_t data = GL_IMAGE_TYPES[t];
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, id);
	for (int i = 0; i < l * 6; ++i) {
		glTexImage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 0, internal, w, h, i, 0, external, data, nullptr);
	}
	set_dimensions(w, h, l);
	set_parameters(TEXTURE_CUBE_ARRAY, f);
}

int Texture::get_width() const {
	return width;
}

int Texture::get_height() const {
	return height;
}

int Texture::get_depth() const {
	return depth;
}

int Texture::get_layer() const {
	/* texture 1D: width x layer */
	if (type == TEXTURE_1D_ARRAY) return height;
	
	/* texture 2D: width x height x layer */
	if (type == TEXTURE_2D_ARRAY) return depth;
	
	/* texture cube: width x height x layer */
	if (type == TEXTURE_CUBE_ARRAY) return depth;
	
	return 0; /* illegal texture */
}

TextureType Texture::get_type() const {
	return type;
}

TextureFormat Texture::get_format() const {
	return format;
}

void Texture::copy_to_image(Image& i) const {
	/* check whether the texture is 2D */
	if (type != TEXTURE_2D) {
		return Error::set("Texture", "Cannot get image from non-2D texture");
	}
	
	/* get the external format of texture */
	uint32_t external = GL_TEXTURE_FORMATS[format].second;
	
	/* get image data from GPU */
	uint8_t* image_data = i.data.data();
	uint32_t image_type = i.bytes == 1 ? GL_UNSIGNED_BYTE : GL_FLOAT;
	glGetTexImage(GL_TEXTURE_2D, 0, external, image_type, image_data);
}

void Texture::generate_mipmap() const {
	uint32_t gl_type = GL_TEXTURE_TYPES[type];
	glGenerateMipmap(gl_type);
}

void Texture::set_wrap_s(TextureWrappingMode m) const {
	uint32_t gl_type = GL_TEXTURE_TYPES[type];
	glBindTexture(gl_type, id);
	glTexParameteri(gl_type, GL_TEXTURE_WRAP_S, GL_TEXTURE_WRAPPINGS[m]);
}

void Texture::set_wrap_t(TextureWrappingMode m) const {
	uint32_t gl_type = GL_TEXTURE_TYPES[type];
	glBindTexture(gl_type, id);
	glTexParameteri(gl_type, GL_TEXTURE_WRAP_T, GL_TEXTURE_WRAPPINGS[m]);
}

void Texture::set_wrap_r(TextureWrappingMode m) const {
	uint32_t gl_type = GL_TEXTURE_TYPES[type];
	glBindTexture(gl_type, id);
	glTexParameteri(gl_type, GL_TEXTURE_WRAP_R, GL_TEXTURE_WRAPPINGS[m]);
}

void Texture::set_wrap_all(TextureWrappingMode m) const {
	uint32_t gl_type = GL_TEXTURE_TYPES[type];
	glBindTexture(gl_type, id);
	glTexParameteri(gl_type, GL_TEXTURE_WRAP_S, GL_TEXTURE_WRAPPINGS[m]);
	glTexParameteri(gl_type, GL_TEXTURE_WRAP_T, GL_TEXTURE_WRAPPINGS[m]);
	glTexParameteri(gl_type, GL_TEXTURE_WRAP_R, GL_TEXTURE_WRAPPINGS[m]);
}

void Texture::set_filters(TextureFilter mag, TextureFilter min) const {
	uint32_t gl_type = GL_TEXTURE_TYPES[type];
	glBindTexture(gl_type, id);
	glTexParameteri(gl_type, GL_TEXTURE_MAG_FILTER, GL_TEXTURE_FILTERS[mag]);
	glTexParameteri(gl_type, GL_TEXTURE_MIN_FILTER, GL_TEXTURE_FILTERS[min]);
}

void Texture::set_border_color(const Vec4& c) const {
	uint32_t gl_type = GL_TEXTURE_TYPES[type];
	glBindTexture(gl_type, id);
	glTexParameterfv(gl_type, GL_TEXTURE_BORDER_COLOR, &c.x);
}

void Texture::set_lod_range(int min, int max) const {
	uint32_t gl_type = GL_TEXTURE_TYPES[type];
	glBindTexture(gl_type, id);
	glTexParameteri(gl_type, GL_TEXTURE_BASE_LEVEL, min);
	glTexParameteri(gl_type, GL_TEXTURE_MAX_LEVEL, max);
}

void Texture::set_lod_bias(int b) const {
	uint32_t gl_type = GL_TEXTURE_TYPES[type];
	glBindTexture(gl_type, id);
	glTexParameteri(gl_type, GL_TEXTURE_LOD_BIAS, b);
}

int Texture::activate(int l) const {
	uint32_t gl_type = GL_TEXTURE_TYPES[type];
	glActiveTexture(GL_TEXTURE0 + l);
	glBindTexture(gl_type, id);
	return l;
}

void Texture::set_dimensions(int w, int h, int d) {
	width = w;
	height = h;
	depth = d;
}

void Texture::set_parameters(TextureType t, TextureFormat f) {
	type = t;
	format = f;
}

TextureFormat Texture::default_format(int c, int b) {
	if (c == 1 && b == 1) return TEXTURE_R8_UNORM;
	if (c == 1 && b == 4) return TEXTURE_R16_SFLOAT;
	if (c == 2 && b == 1) return TEXTURE_R8G8_UNORM;
	if (c == 2 && b == 4) return TEXTURE_R16G16_SFLOAT;
	if (c == 3 && b == 1) return TEXTURE_R8G8B8_UNORM;
	if (c == 3 && b == 4) return TEXTURE_R16G16B16A16_SFLOAT;
	if (c == 4 && b == 1) return TEXTURE_R8G8B8A8_UNORM;
	/* c == 4 && b == 4 */ return TEXTURE_R16G16B16A16_SFLOAT;
}

TextureFormat Texture::default_format(const Image& i) {
	return default_format(i.channel, i.bytes);
}

RenderBuffer::RenderBuffer() {
	glGenRenderbuffers(1, &id);
}

RenderBuffer::~RenderBuffer() {
	glDeleteRenderbuffers(1, &id);
}

void RenderBuffer::init(int w, int h, TextureFormat f) const {
	int32_t internal = GL_TEXTURE_FORMATS[f].second;
	glBindRenderbuffer(GL_RENDERBUFFER, id);
	glRenderbufferStorage(GL_RENDERBUFFER, internal, w, h);
}

RenderTarget::RenderTarget() {
	glGenFramebuffers(1, &id);
}

RenderTarget::~RenderTarget() {
	glDeleteFramebuffers(1, &id);
}

void RenderTarget::set_texture(const Texture& t, unsigned int i, int l, int p) const {
	set_framebuffer(t, GL_COLOR_ATTACHMENT0 + i, l, p);
}

void RenderTarget::set_depth_texture(const Texture& t, int l, int p) const {
	set_framebuffer(t, GL_DEPTH_ATTACHMENT, l, p);
}

void RenderTarget::set_stencil_texture(const Texture& t, int l, int p) const {
	set_framebuffer(t, GL_STENCIL_ATTACHMENT, l, p);
}

void RenderTarget::set_depth_stencil_texture(const Texture& t, int l, int p) const {
	set_framebuffer(t, GL_DEPTH_STENCIL_ATTACHMENT, l, p);
}

void RenderTarget::set_depth_buffer(const RenderBuffer& r) const {
	set_framebuffer(r, GL_DEPTH_ATTACHMENT);
}

void RenderTarget::set_stencil_buffer(const RenderBuffer& r) const {
	set_framebuffer(r, GL_STENCIL_ATTACHMENT);
}

void RenderTarget::set_depth_stencil_buffer(const RenderBuffer& r) const {
	set_framebuffer(r, GL_DEPTH_STENCIL_ATTACHMENT);
}

void RenderTarget::set_target_number(int n) const {
	glBindFramebuffer(GL_FRAMEBUFFER, id);
	glDrawBuffers(n, GL_COLOR_ATTACHMENTS);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderTarget::activate() const {
	glBindFramebuffer(GL_FRAMEBUFFER, id);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		Error::set("RenderTarget", "Render target is not complete");
	}
}

void RenderTarget::activate(const RenderTarget* f) {
	uint32_t id = f == nullptr ? 0 : f->id;
	glBindFramebuffer(GL_FRAMEBUFFER, id);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		Error::set("RenderTarget", "Render target is not complete");
	}
}

void RenderTarget::set_framebuffer(const Texture& t, uint32_t a, int l, int p) const {
	glBindFramebuffer(GL_FRAMEBUFFER, id);
	if (t.type == TEXTURE_2D) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, a, GL_TEXTURE_2D, t.id, l);
	} else if (t.type == TEXTURE_CUBE) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, a, GL_TEXTURE_CUBE_MAP_POSITIVE_X + p, t.id, l);
	} else if (t.type == TEXTURE_3D) {
		glFramebufferTexture3D(GL_FRAMEBUFFER, a, GL_TEXTURE_3D, t.id, l, p);
	} else if (t.type == TEXTURE_2D_ARRAY) {
		glFramebufferTextureLayer(GL_FRAMEBUFFER, a, t.id, l, p);
	} else if (t.type == TEXTURE_CUBE_ARRAY) {
		glFramebufferTextureLayer(GL_FRAMEBUFFER, a, t.id, l, p);
	} else {
		Error::set("RenderTarget", "Texture type is not supported");
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderTarget::set_framebuffer(const RenderBuffer& r, uint32_t a) const {
	glBindFramebuffer(GL_FRAMEBUFFER, id);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, a, GL_RENDERBUFFER, r.id);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

}

/* -------------------------------------------------------------------------- */
/* ---- ink/shaders/ShaderCache.cxx ----------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

void ShaderCache::load_vert(const std::string& n, const char* s) {
	vert_shaders.insert_or_assign(n, s);
	resolve_includes(vert_shaders[n]);
}

void ShaderCache::load_vert(const std::string& n, const std::string& s) {
	vert_shaders.insert_or_assign(n, s);
	resolve_includes(vert_shaders[n]);
}

void ShaderCache::load_geom(const std::string& n, const char* s) {
	geom_shaders.insert_or_assign(n, s);
	resolve_includes(geom_shaders[n]);
}

void ShaderCache::load_geom(const std::string& n, const std::string& s) {
	geom_shaders.insert_or_assign(n, s);
	resolve_includes(geom_shaders[n]);
}

void ShaderCache::load_frag(const std::string& n, const char* s) {
	frag_shaders.insert_or_assign(n, s);
	resolve_includes(frag_shaders[n]);
}

void ShaderCache::load_frag(const std::string& n, const std::string& s) {
	frag_shaders.insert_or_assign(n, s);
	resolve_includes(frag_shaders[n]);
}

void ShaderCache::load_vert_file(const std::string& n, const std::string& s) {
	vert_shaders.insert_or_assign(n, File::read(s));
	resolve_includes(vert_shaders[n]);
}

void ShaderCache::load_geom_file(const std::string& n, const std::string& s) {
	geom_shaders.insert_or_assign(n, File::read(s));
	resolve_includes(geom_shaders[n]);
}

void ShaderCache::load_frag_file(const std::string& n, const std::string& s) {
	frag_shaders.insert_or_assign(n, File::read(s));
	resolve_includes(frag_shaders[n]);
}

bool ShaderCache::has_vert(const std::string& n) {
	return vert_shaders.count(n) != 0;
}

bool ShaderCache::has_geom(const std::string& n) {
	return geom_shaders.count(n) != 0;
}

bool ShaderCache::has_frag(const std::string& n) {
	return frag_shaders.count(n) != 0;
}

const Gpu::Shader* ShaderCache::fetch(const std::string& n) {
	/* set the name as cache key */
	std::string key = n;
	
	/* check whether the key exists */
	if (cache.count(key) != 0) return cache.at(key).get();
	
	/* insert key and shader to the cache */
	auto p = cache.insert({key, std::make_unique<Gpu::Shader>()});
	auto* shader = p.first->second.get();
	
	/* load vertex, geometry and fragment shaders */
	if (vert_shaders.count(n) != 0) {
		shader->load_vert(vert_shaders[n]);
	} else {
		Error::set("ShaderCache", "Vertex shader is missing");
	}
	if (geom_shaders.count(n) != 0) {
		shader->load_geom(geom_shaders[n]);
	}
	if (frag_shaders.count(n) != 0) {
		shader->load_frag(frag_shaders[n]);
	} else {
		Error::set("ShaderCache", "Fragment shader is missing");
	}
	
	/* compile shader */
	shader->compile();
	
	return shader; /* return the shader */
}

const Gpu::Shader* ShaderCache::fetch(const std::string& n, const Defines& d) {
	/* set the name and defines as cache key */
	std::string key = n + '\n' + d.get();
	
	/* check whether the key exists */
	if (cache.count(key) != 0) return cache.at(key).get();
	
	/* insert key and shader to the cache */
	auto p = cache.insert({key, std::make_unique<Gpu::Shader>()});
	auto* shader = p.first->second.get();
	
	/* load vertex, geometry and fragment shaders */
	if (vert_shaders.count(n) != 0) {
		shader->load_vert(vert_shaders[n]);
	} else {
		Error::set("ShaderCache", "Vertex shader is missing");
	}
	if (geom_shaders.count(n) != 0) {
		shader->load_geom(geom_shaders[n]);
	}
	if (frag_shaders.count(n) != 0) {
		shader->load_frag(frag_shaders[n]);
	} else {
		Error::set("ShaderCache", "Fragment shader is missing");
	}
	
	/* set defines and compile shader */
	shader->set_defines(d);
	shader->compile();
	
	return shader; /* return the shader */
}

void ShaderCache::clear_cache(const std::string& n) {
	cache.erase(n);
}

void ShaderCache::clear_cache(const std::string& n, const Defines& d) {
	cache.erase(n + '\n' + d.get());
}

void ShaderCache::clear_caches() {
	cache.clear();
}

std::string ShaderCache::get_include_path() {
	return include_path;
}

void ShaderCache::set_include_path(const std::string& p) {
	include_path = p;
}

void ShaderCache::resolve_includes(std::string& s) {
	size_t line_begin = 0;
	size_t line_end = -1;
	
	/* record include times to avoid circular include dependency */
	int include_times = 0;
	constexpr int max_include_times = 256;
	
	while (line_end != s.length()) {
		/* search the string of each line */
		line_begin = line_end + 1;
		line_end = s.find('\n', line_begin);
		line_end = line_end == -1 ? s.length() : line_end;
		size_t line_length = line_end - line_begin;
		std::string line = s.substr(line_begin, line_length);
		
		/* search for the include name */
		size_t char_1 = line.find_first_not_of(" \t");
		if (char_1 == -1 || line[char_1] != '#') {
			continue;
		}
		size_t char_2 = line.find_first_not_of(" \t", char_1 + 1);
		if (char_2 == -1 || line.substr(char_2, 7) != "include") {
			continue;
		}
		size_t char_3 = line.find_first_not_of(" \t", char_2 + 7);
		if (char_3 == -1 || line[char_3] != '<') {
			continue;
		}
		size_t char_4 = line.find('>', char_3 + 1);
		if (char_4 == -1) {
			Error::set("ShaderCache", "Invalid preprocessing directive");
			continue;
		}
		std::string include = line.substr(char_3 + 1, char_4 - char_3 - 1);
		
		/* read included file into content */
		std::string content = File::read(include_path + include + ".glsl");
		
		/* replace the line with content */
		s.replace(line_begin, line_end - line_begin, content);
		line_end = line_begin - 1;
		
		/* check if there is circular include dependency */
		if (include_times++ == max_include_times) {
			return Error::set("ShaderCache", "Circular include dependency");
		}
	}
}

std::string ShaderCache::include_path = "ink/shaders/include/";

std::unordered_map<std::string, std::string> ShaderCache::vert_shaders;
std::unordered_map<std::string, std::string> ShaderCache::geom_shaders;
std::unordered_map<std::string, std::string> ShaderCache::frag_shaders;

std::unordered_map<std::string, std::unique_ptr<Gpu::Shader> > ShaderCache::cache;

}

/* -------------------------------------------------------------------------- */
/* ---- ink/shaders/ShaderLib.cxx ------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

const Gpu::Shader* ShaderLib::fetch(const std::string& n) {
	std::string shader_file = library_path + n;
	if (!ShaderCache::has_vert(n)) {
		ShaderCache::load_vert(n, File::read(shader_file + ".vert.glsl"));
	}
	if (!ShaderCache::has_frag(n)) {
		ShaderCache::load_frag(n, File::read(shader_file + ".frag.glsl"));
	}
	return ShaderCache::fetch(n);
}

const Gpu::Shader* ShaderLib::fetch(const std::string& n, const Defines& d) {
	std::string shader_file = library_path + n;
	if (!ShaderCache::has_vert(n)) {
		ShaderCache::load_vert(n, File::read(shader_file + ".vert.glsl"));
	}
	if (!ShaderCache::has_frag(n)) {
		ShaderCache::load_frag(n, File::read(shader_file + ".frag.glsl"));
	}
	return ShaderCache::fetch(n, d);
}

std::string ShaderLib::get_library_path() {
	return library_path;
}

void ShaderLib::set_library_path(const std::string& p) {
	library_path = p;
}

std::string ShaderLib::library_path = "ink/shaders/library/";

}

/* -------------------------------------------------------------------------- */
/* ---- ink/lights/LinearFog.cxx -------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

LinearFog::LinearFog(const Vec3& c, float n, float f) :
color(c), near(n), far(f) {}

}

/* -------------------------------------------------------------------------- */
/* ---- ink/lights/Exp2Fog.cxx ---------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

Exp2Fog::Exp2Fog(const Vec3& c, float n, float d) :
color(c), near(n), density(d) {}

}

/* -------------------------------------------------------------------------- */
/* ---- ink/lights/Shadow.cxx ----------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

void Shadow::init(int w, int h, int n) {
	resolution = Vec2(w, h);
	shadow_map = std::make_unique<Gpu::Texture>();
	shadow_map->init_2d_array(w, h, n, TEXTURE_D24_UNORM);
	shadow_map->set_wrap_all(TEXTURE_CLAMP_TO_BORDER);
	shadow_map->set_filters(TEXTURE_LINEAR, TEXTURE_NEAREST);
	shadow_map->set_border_color({1, 1, 1, 1});
	shadow_target = std::make_unique<Gpu::RenderTarget>();
	shadow_target->set_target_number(0);
}

int Shadow::get_samples() {
	return samples;
}

void Shadow::set_samples(int s) {
	samples = s;
}

Vec2 Shadow::get_resolution() {
	return resolution;
}

int Shadow::activate_texture(int l) {
	return shadow_map->activate(l);
}

const Gpu::RenderTarget* Shadow::get_target() const {
	shadow_target->set_depth_texture(*shadow_map, 0, map_id);
	return shadow_target.get();
}

int Shadow::samples = 32;

Vec2 Shadow::resolution;

std::unique_ptr<Gpu::Texture> Shadow::shadow_map;

std::unique_ptr<Gpu::RenderTarget> Shadow::shadow_target;

}

/* -------------------------------------------------------------------------- */
/* ---- ink/lights/Light.cxx ------------------------------------------------ */
/* -------------------------------------------------------------------------- */

namespace Ink {

Light::Light(const Vec3& c, float i) : color(c), intensity(i) {}

}

/* -------------------------------------------------------------------------- */
/* ---- ink/lights/PointLight.cxx ------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

PointLight::PointLight(const Vec3& c, float i) : Light(c, i) {}

}

/* -------------------------------------------------------------------------- */
/* ---- ink/lights/SpotLight.cxx -------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

SpotLight::SpotLight(const Vec3& c, float i) : Light(c, i) {
	shadow.camera = PerspCamera(angle * 2, 1, 0.5, 500);
}

}

/* -------------------------------------------------------------------------- */
/* ---- ink/lights/DirectionalLight.cxx ------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

DirectionalLight::DirectionalLight(const Vec3& c, float i) : Light(c, i) {
	shadow.camera = static_cast<Camera>(OrthoCamera(-5, 5, -5, 5, 0.5, 500));
}

}

/* -------------------------------------------------------------------------- */
/* ---- ink/lights/HemisphereLight.cxx -------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

HemisphereLight::HemisphereLight(const Vec3& s, const Vec3& g, float i) :
Light(s, i), ground_color(g) {}

}

/* -------------------------------------------------------------------------- */
/* ---- ink/probes/IBLFilter.cxx -------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

constexpr float PHI = 1.618033989;
constexpr float INV_PHI = 0.618033989;

void IBLFilter::load_cubemap(const Image& px, const Image& nx,
							 const Image& py, const Image& ny,
							 const Image& pz, const Image& nz,
							 Gpu::Texture& m, int s) {
	Gpu::Texture source_map;
	source_map.init_cube(px, nx, py, ny, pz, nz,
						 Gpu::Texture::default_format(px));
	source_map.set_filters(TEXTURE_LINEAR, TEXTURE_LINEAR);
	load_texture(source_map, m, s);
}

void IBLFilter::load_equirect(const Image& i, Gpu::Texture& m, int s) {
	Gpu::Texture source_map;
	source_map.init_2d(i, Gpu::Texture::default_format(i));
	source_map.set_filters(TEXTURE_LINEAR, TEXTURE_LINEAR);
	load_texture(source_map, m, s);
}

void IBLFilter::load_texture(const Gpu::Texture& t, Gpu::Texture& m, int s) {
	/* initialize fullscreen plane */
	[[maybe_unused]]
	static bool inited = init_fullscreen_plane();
	
	/* disable depth & stencil & scissor test */
	Gpu::State::disable_depth_test();
	Gpu::State::disable_stencil_test();
	Gpu::State::disable_scissor_test();
	
	/* disable blending & wireframe & culling */
	Gpu::State::disable_blending();
	Gpu::State::disable_wireframe();
	Gpu::State::disable_culling();
	
	/* enable texture cube seamless */
	Gpu::State::enable_texture_cube_seamless();
	
	/* change the current viewport */
	Gpu::State::set_viewport(Gpu::Rect(s, s));
	
	/* initialize radiance environment map */
	m.init_cube(s, s, t.get_format());
	m.set_filters(TEXTURE_LINEAR, TEXTURE_LINEAR_MIPMAP_LINEAR);
	m.generate_mipmap();
	
	/* fetch the blur shader from shader lib */
	auto* blur_shader = ShaderLib::fetch("SphericalBlur");
	
	/* fetch the cubemap shader from shader lib */
	Defines cubemap_defines;
	int type = t.get_type();
	cubemap_defines.set_if("USE_EQUIRECT", type == TEXTURE_2D);
	cubemap_defines.set_if("USE_CUBEMAP", type == TEXTURE_CUBE);
	auto* cubemap_shader = ShaderLib::fetch("Cubemap", cubemap_defines);
	
	/* prepare cubemap render target */
	if (!cubemap_target) {
		cubemap_target = std::make_unique<Gpu::RenderTarget>();
	}
	
	/* render to cube texture */
	for (int i = 0; i < 6; ++i) {
		cubemap_target->set_texture(m, 0, 0, i);
		Gpu::RenderTarget::activate(cubemap_target.get());
		cubemap_shader->use_program();
		cubemap_shader->set_uniform_i("face", i);
		cubemap_shader->set_uniform_i("map", t.activate(0));
		fullscreen_plane->attach(*cubemap_shader);
		fullscreen_plane->render();
	}
	
	/* prepare blur map */
	if (!blur_map) blur_map = std::make_unique<Gpu::Texture>();
	blur_map->init_cube(s, s, t.get_format());
	blur_map->generate_mipmap();
	blur_map->set_filters(TEXTURE_LINEAR, TEXTURE_LINEAR_MIPMAP_LINEAR);
	
	/* prepare blur render target */
	if (!blur_target) {
		blur_target = std::make_unique<Gpu::RenderTarget>();
	}
	
	/* blur cube texture latitudinally and longitudinally */
	float weights[20];
	int size_lod = s;
	int max_lod = log2f(size_lod);
	for (int lod = 1; lod <= max_lod; ++lod) {
		
		/* calculate sigma radians */
		float sigma_radians = sqrtf(3) / size_lod;
		if (lod == 1) sigma_radians = 2.f / size_lod;
		
		/* calculate blur parameters */
		Vec3 pole_axis = axes[(lod - 1) % 10];
		float d_theta = PI / (size_lod * 2 - 2);
		float sigma = sigma_radians / d_theta;
		int samples = 1 + floorf(sigma * 3);
		gaussian_weights(sigma, 20, weights);
		
		/* change the current viewport */
		Gpu::State::set_viewport(Gpu::Rect(size_lod / 2, size_lod / 2));
		
		/* blur texture latitudinally */
		for (int i = 0; i < 6; ++i) {
			blur_target->set_texture(*blur_map, 0, lod, i);
			Gpu::RenderTarget::activate(blur_target.get());
			blur_shader->use_program();
			blur_shader->set_uniform_f("lod", lod - 1);
			blur_shader->set_uniform_i("face", i);
			blur_shader->set_uniform_i("samples", samples);
			blur_shader->set_uniform_i("latitudinal", 1);
			blur_shader->set_uniform_f("d_theta", d_theta);
			blur_shader->set_uniform_v3("pole_axis", pole_axis);
			blur_shader->set_uniform_i("map", m.activate(0));
			for (int w = 0; w < 20; ++w) {
				std::string weights_i = fmt::format("weights[{}]", w);
				blur_shader->set_uniform_f(weights_i, weights[w]);
			}
			fullscreen_plane->attach(*blur_shader);
			fullscreen_plane->render();
		}
		
		/* calculate blur parameters */
		size_lod /= 2;
		d_theta = PI / (size_lod * 2 - 2);
		if (std::isinf(d_theta)) d_theta = PI;
		sigma = sigma_radians / d_theta;
		samples = 1 + floor(sigma * 3);
		gaussian_weights(sigma, 20, weights);
		
		/* blur texture longitudinally */
		for (int f = 0; f < 6; ++f) {
			blur_target->set_texture(m, 0, lod, f);
			Gpu::RenderTarget::activate(blur_target.get());
			blur_shader->use_program();
			blur_shader->set_uniform_f("lod", lod);
			blur_shader->set_uniform_i("face", f);
			blur_shader->set_uniform_i("samples", samples);
			blur_shader->set_uniform_i("latitudinal", 0);
			blur_shader->set_uniform_f("d_theta", d_theta);
			blur_shader->set_uniform_v3("pole_axis", pole_axis);
			blur_shader->set_uniform_i("map", blur_map->activate(0));
			for (int w = 0; w < 20; ++w) {
				std::string weights_i = fmt::format("weights[{}]", w);
				blur_shader->set_uniform_f(weights_i, weights[w]);
			}
			fullscreen_plane->attach(*blur_shader);
			fullscreen_plane->render();
		}
	}
	
	/* set back to the default render target */
	Gpu::RenderTarget::activate(nullptr);
}

bool IBLFilter::init_fullscreen_plane() {
	/* prepare triangle mesh */
	Mesh triangle_mesh = Mesh("fullscreen");
	triangle_mesh.groups = {{"default", 0, 3}};
	triangle_mesh.vertex = {{-1, 3, 0}, {-1, -1, 0}, {3, -1, 0}};
	triangle_mesh.uv = {{0, 2}, {0, 0}, {2, 0}};
	
	/* prepare fullscreen plane vertex object */
	fullscreen_plane = std::make_unique<Gpu::VertexObject>();
	fullscreen_plane->load(triangle_mesh, triangle_mesh.groups[0]);
	
	return true; /* finish */
}

void IBLFilter::gaussian_weights(float s, int n, float* w) {
	float weight_sum = 0;
	for (int i = 0; i < n; ++i) {
		float x = i / s;
		float weight = expf(-x * x * .5f);
		w[i] = weight;
		if (i == 0) {
			weight_sum += weight;
		} else {
			weight_sum += weight * 2;
		}
	}
	for (int i = 0; i < 20; ++i) {
		w[i] /= weight_sum;
	}
}

Vec3 IBLFilter::axes[] = {
	{ 1,     1,      1},
	{-1,     1,      1},
	{ 1,     1,     -1},
	{-1,     1,     -1},
	{ 0, PHI,  INV_PHI},
	{ 0, PHI, -INV_PHI},
	{ INV_PHI, 0,  PHI},
	{-INV_PHI, 0,  PHI},
	{ PHI, INV_PHI,  0},
	{-PHI, INV_PHI,  0},
};

std::unique_ptr<Gpu::VertexObject> IBLFilter::fullscreen_plane;

std::unique_ptr<Gpu::Texture> IBLFilter::blur_map;

std::unique_ptr<Gpu::RenderTarget> IBLFilter::cubemap_target;

std::unique_ptr<Gpu::RenderTarget> IBLFilter::blur_target;

}

/* -------------------------------------------------------------------------- */
/* ---- ink/probes/ReflectionProbe.cxx -------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

ReflectionProbe::ReflectionProbe(float i, int r, const Vec3& p) :
intensity(i), resolution(r), position(p) {}

void ReflectionProbe::load_cubemap(const Image& px, const Image& nx,
								   const Image& py, const Image& ny,
								   const Image& pz, const Image& nz) {
	if (!reflection_map) {
		reflection_map = std::make_unique<Gpu::Texture>();
	}
	IBLFilter::load_cubemap(px, nx, py, ny, pz, nz, *reflection_map, resolution);
}

void ReflectionProbe::load_equirect(const Image& i) {
	if (!reflection_map) {
		reflection_map = std::make_unique<Gpu::Texture>();
	}
	IBLFilter::load_equirect(i, *reflection_map, resolution);
}

void ReflectionProbe::load_texture(const Gpu::Texture& t) {
	if (!reflection_map) {
		reflection_map = std::make_unique<Gpu::Texture>();
	}
	IBLFilter::load_texture(t, *reflection_map, resolution);
}

int ReflectionProbe::activate(int l) const {
	return reflection_map->activate(l);
}

}

/* -------------------------------------------------------------------------- */
/* ---- ink/scene/Scene.cxx ------------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

Scene::Scene(const std::string& n) : Instance(n) {}

Material* Scene::get_material(const std::string& n) const {
	if (material_library.count(n) == 0) {
		return nullptr;
	}
	return material_library.at(n);
}

Material* Scene::get_material(const std::string& n, const Mesh& s) const {
	auto name = fmt::format("M{}#{}", reinterpret_cast<size_t>(&s), n);
	if (material_library.count(name) == 0) {
		return nullptr;
	}
	return material_library.at(name);
}

Material* Scene::get_material(const std::string& n, const Instance& s) const {
	auto name = fmt::format("I{}#{}", reinterpret_cast<size_t>(&s), n);
	if (material_library.count(name) == 0) {
		return nullptr;
	}
	return material_library.at(name);
}

void Scene::set_material(const std::string& n, Material* m) {
	material_library.insert_or_assign(n, m);
}

void Scene::set_material(const std::string& n, const Mesh& s, Material* m) {
	auto name = fmt::format("M{}#{}", reinterpret_cast<size_t>(&s), n);
	material_library.insert_or_assign(name, m);
}

void Scene::set_material(const std::string& n, const Instance& s, Material* m) {
	auto name = fmt::format("I{}#{}", reinterpret_cast<size_t>(&s), n);
	material_library.insert_or_assign(name, m);
}

void Scene::remove_material(const std::string& n) {
	material_library.erase(n);
}

void Scene::remove_material(const std::string& n, const Mesh& s) {
	auto name = fmt::format("M{}#{}", reinterpret_cast<size_t>(&s), n);
	material_library.erase(name);
}

void Scene::remove_material(const std::string& n, const Instance& s) {
	auto name = fmt::format("I{}#{}", reinterpret_cast<size_t>(&s), n);
	material_library.erase(name);
}

void Scene::clear_materials() {
	material_library.clear();
}

std::vector<Material*> Scene::get_materials() const {
	std::vector<Material*> materials;
	for (auto& [name, material] : material_library) {
		materials.emplace_back(material);
	}
	return materials;
}

LinearFog* Scene::get_linear_fog() const {
	return linear_fog;
}

void Scene::set_fog(LinearFog* f) {
	linear_fog = f;
	exp2_fog = nullptr;
}

Exp2Fog* Scene::get_exp2_fog() const {
	return exp2_fog;
}

void Scene::set_fog(Exp2Fog* f) {
	exp2_fog = f;
	linear_fog = nullptr;
}

void Scene::add_light(PointLight* l) {
	point_lights.emplace_back(l);
}

void Scene::remove_light(PointLight* l) {
	size_t size = point_lights.size();
	for (int i = 0; i < size; ++i) {
		if (point_lights[i] == l) {
			point_lights.erase(point_lights.begin() + i);
		}
	}
}

size_t Scene::get_point_light_count() const {
	return point_lights.size();
}

PointLight* Scene::get_point_light(int i) const {
	return point_lights[i];
}

void Scene::add_light(SpotLight* l) {
	spot_lights.emplace_back(l);
}

void Scene::remove_light(SpotLight* l) {
	size_t size = spot_lights.size();
	for (int i = 0; i < size; ++i) {
		if (spot_lights[i] == l) {
			spot_lights.erase(spot_lights.begin() + i);
		}
	}
}

size_t Scene::get_spot_light_count() const {
	return spot_lights.size();
}

SpotLight* Scene::get_spot_light(int i) const {
	return spot_lights[i];
}

void Scene::add_light(DirectionalLight* l) {
	directional_lights.emplace_back(l);
}

void Scene::remove_light(DirectionalLight* l) {
	size_t size = directional_lights.size();
	for (int i = 0; i < size; ++i) {
		if (directional_lights[i] == l) {
			directional_lights.erase(directional_lights.begin() + i);
		}
	}
}

size_t Scene::get_directional_light_count() const {
	return directional_lights.size();
}

DirectionalLight* Scene::get_directional_light(int i) const {
	return directional_lights[i];
}

void Scene::add_light(HemisphereLight* l) {
	hemisphere_lights.emplace_back(l);
}

void Scene::remove_light(HemisphereLight* l) {
	size_t size = hemisphere_lights.size();
	for (int i = 0; i < size; ++i) {
		if (hemisphere_lights[i] == l) {
			hemisphere_lights.erase(hemisphere_lights.begin() + i);
		}
	}
}

size_t Scene::get_hemisphere_light_count() const {
	return hemisphere_lights.size();
}

HemisphereLight* Scene::get_hemisphere_light(int i) const {
	return hemisphere_lights[i];
}

void Scene::clear_lights() {
	point_lights.clear();
	spot_lights.clear();
	directional_lights.clear();
	hemisphere_lights.clear();
}

void Scene::update_instances() {
	std::vector<Instance*> unvisited;
	matrix_local = transform();
	matrix_global = matrix_local;
	unvisited.emplace_back(this);
	while (!unvisited.empty()) {
		Instance* current = unvisited.back();
		unvisited.pop_back();
		size_t count = current->get_child_count();
		for (int i = 0; i < count; ++i) {
			Instance* child = current->get_child(i);
			child->matrix_local = child->transform();
			child->matrix_global = current->matrix_global * child->matrix_local;
			unvisited.emplace_back(child);
		}
	}
}

std::vector<const Instance*> Scene::to_instances() const {
	std::vector<const Instance*> unvisited;
	std::vector<const Instance*> instances;
	unvisited.emplace_back(this);
	while (!unvisited.empty()) {
		const Instance* current = unvisited.back();
		unvisited.pop_back();
		size_t count = current->get_child_count();
		for (int i = 0; i < count; ++i) {
			unvisited.emplace_back(current->get_child(i));
		}
		instances.emplace_back(current);
	}
	return instances;
}

std::vector<const Instance*> Scene::to_visible_instances() const {
	std::vector<const Instance*> unvisited;
	std::vector<const Instance*> instances;
	unvisited.emplace_back(this);
	while (!unvisited.empty()) {
		const Instance* current = unvisited.back();
		unvisited.pop_back();
		if (!current->visible) continue;
		size_t count = current->get_child_count();
		for (int i = 0; i < count; ++i) {
			unvisited.emplace_back(current->get_child(i));
		}
		if (current->mesh != nullptr) {
			instances.emplace_back(current);
		}
	}
	return instances;
}

}

/* -------------------------------------------------------------------------- */
/* ---- ink/renderer/Renderer.cxx ------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

const Vec3 CUBE_DIRECTIONS[] = {
	{-1, 0, 0}, {1, 0, 0}, {0, -1, 0},
	{0, 1, 0}, {0, 0, -1}, {0, 0, 1},
};

const Vec3 CUBE_UPS[] = {
	{0, -1, 0}, {0, -1, 0}, {0, 0, 1},
	{0, 0, -1}, {0, -1, 0}, {0, -1, 0},
};

Vec4 Renderer::get_clear_color() const {
	return clear_color;
}

void Renderer::set_clear_color(const Vec4& c) {
	clear_color = c;
}

void Renderer::clear(bool c, bool d, bool s) const {
	Gpu::RenderTarget::activate(target);
	Gpu::State::set_clear_color(clear_color);
	Gpu::State::clear(c, d, s);
	Gpu::RenderTarget::activate(nullptr);
}

Gpu::Rect Renderer::get_viewport() const {
	return viewport;
}

void Renderer::set_viewport(const Gpu::Rect& v) {
	viewport = v;
}

bool Renderer::get_scissor_test() const {
	return scissor_test;
}

void Renderer::set_scissor_test(bool t) {
	scissor_test = t;
}

Gpu::Rect Renderer::get_scissor() const {
	return scissor;
}

void Renderer::set_scissor(const Gpu::Rect& s) {
	scissor = s;
}

RenderingMode Renderer::get_rendering_mode() const {
	return rendering_mode;
}

void Renderer::set_rendering_mode(RenderingMode m) {
	rendering_mode = m;
}

ToneMapMode Renderer::get_tone_map_mode() const {
	return tone_map_mode;
}

float Renderer::get_tone_map_exposure() const {
	return tone_map_exposure;
}

void Renderer::set_tone_map(ToneMapMode m, float e) {
	tone_map_mode = m;
	tone_map_exposure = e;
}

const Gpu::RenderTarget* Renderer::get_target() const {
	return target;
}

void Renderer::set_target(const Gpu::RenderTarget *t) {
	target = t;
}

void Renderer::set_texture_callback(const TextureCallback& f) {
	texture_callback = f;
}

float Renderer::get_skybox_intensity() const {
	return skybox_intensity;
}

void Renderer::set_skybox_intensity(float i) {
	skybox_intensity = i;
}

void Renderer::load_skybox(const Image& px, const Image& nx,
						   const Image& py, const Image& ny,
						   const Image& pz, const Image& nz) {
	skybox_map = std::make_unique<Gpu::Texture>();
	skybox_map->init_cube(px, nx, py, ny, pz, nz,
						  Gpu::Texture::default_format(px));
	skybox_map->set_filters(TEXTURE_LINEAR, TEXTURE_LINEAR);
}

void Renderer::load_skybox(const Image& i) {
	skybox_map = std::make_unique<Gpu::Texture>();
	skybox_map->init_2d(i, Gpu::Texture::default_format(i));
	skybox_map->set_filters(TEXTURE_LINEAR, TEXTURE_LINEAR);
}

void Renderer::render_skybox(const Camera& c) const {
	/* activate the render target */
	Gpu::RenderTarget::activate(target);
	
	/* set the viewport region */
	Gpu::State::set_viewport(viewport);
	
	/* set the scissor test and region */
	if (scissor_test) {
		Gpu::State::enable_scissor_test();
		Gpu::State::set_scissor(scissor);
	} else {
		Gpu::State::disable_scissor_test();
	}
	
	/* render the skybox to buffer */
	render_skybox_to_buffer(c, rendering_mode);
	
	/* set back to the default render target */
	Gpu::RenderTarget::activate(nullptr);
}

void Renderer::load_mesh(const Mesh& m) {
	if (mesh_cache.count(&m) != 0) return;
	size_t size = m.groups.size();
	auto p = mesh_cache.insert({&m, std::make_unique<Gpu::VertexObject[]>(size)});
	auto* vertex_object = p.first->second.get();
	for (int i = 0; i < size; ++i) {
		vertex_object[i].load(m, m.groups[i]);
	}
}

void Renderer::unload_mesh(const Mesh& m) {
	mesh_cache.erase(&m);
}

void Renderer::clear_mesh_caches() {
	mesh_cache.clear();
}

void Renderer::load_image(const Image& i) {
	if (image_cache.count(&i) != 0) return;
	auto p = image_cache.insert({&i, std::make_unique<Gpu::Texture>()});
	auto* texture = p.first->second.get();
	texture->init_2d(i, Gpu::Texture::default_format(i));
	if (texture_callback) {
		std::invoke(texture_callback, *texture);
	}
}

void Renderer::unload_image(const Image& i) {
	image_cache.erase(&i);
}

void Renderer::clear_image_caches() {
	image_cache.clear();
}

void Renderer::load_scene(const Scene& s) {
	/* load the meshes linked with instance */
	for (auto& instance : s.to_instances()) {
		auto* mesh = instance->mesh;
		if (mesh != nullptr) load_mesh(*mesh);
	}
	
	/* load the images linked with instance */
	for (auto& material : s.get_materials()) {
		if (material->normal_map != nullptr) {
			load_image(*material->normal_map);
		}
		if (material->displacement_map != nullptr) {
			load_image(*material->displacement_map);
		}
		if (material->color_map != nullptr) {
			load_image(*material->color_map);
		}
		if (material->alpha_map != nullptr) {
			load_image(*material->alpha_map);
		}
		if (material->emissive_map != nullptr) {
			load_image(*material->emissive_map);
		}
		if (material->ao_map != nullptr) {
			load_image(*material->ao_map);
		}
		if (material->roughness_map != nullptr) {
			load_image(*material->roughness_map);
		}
		if (material->metalness_map != nullptr) {
			load_image(*material->metalness_map);
		}
		if (material->specular_map != nullptr) {
			load_image(*material->specular_map);
		}
		for (int i = 0; i < 16; ++i) {
			auto* image = material->custom_maps[i];
			if (image != nullptr) load_image(*image);
		}
	}
}

void Renderer::unload_scene(const Scene& s) {
	/* unload the meshes linked with instance */
	for (auto& instance : s.to_instances()) {
		auto* mesh = instance->mesh;
		if (mesh != nullptr) unload_mesh(*mesh);
	}
	
	/* unload the images linked with instance */
	for (auto& material : s.get_materials()) {
		if (material->normal_map != nullptr) {
			unload_image(*material->normal_map);
		}
		if (material->displacement_map != nullptr) {
			unload_image(*material->displacement_map);
		}
		if (material->color_map != nullptr) {
			unload_image(*material->color_map);
		}
		if (material->alpha_map != nullptr) {
			unload_image(*material->alpha_map);
		}
		if (material->emissive_map != nullptr) {
			unload_image(*material->emissive_map);
		}
		if (material->ao_map != nullptr) {
			unload_image(*material->ao_map);
		}
		if (material->roughness_map != nullptr) {
			unload_image(*material->roughness_map);
		}
		if (material->metalness_map != nullptr) {
			unload_image(*material->metalness_map);
		}
		if (material->specular_map != nullptr) {
			unload_image(*material->specular_map);
		}
		for (int i = 0; i < 16; ++i) {
			auto* image = material->custom_maps[i];
			if (image != nullptr) unload_image(*image);
		}
	}
}

void Renderer::clear_scene_caches() {
	mesh_cache.clear();
	image_cache.clear();
}

void Renderer::render(const Scene& s, const Camera& c) const {
	/* activate the render target */
	Gpu::RenderTarget::activate(target);
	
	/* set the viewport region */
	Gpu::State::set_viewport(viewport);
	
	/* set the scissor test and region */
	if (scissor_test) {
		Gpu::State::enable_scissor_test();
		Gpu::State::set_scissor(scissor);
	} else {
		Gpu::State::disable_scissor_test();
	}
	
	/* render the scene to buffer */
	render_to_buffer(s, c, rendering_mode, false);
	
	/* set back to the default render target */
	Gpu::RenderTarget::activate(nullptr);
}

void Renderer::render_transparent(const Scene& s, const Camera& c) const {
	/* activate the render target */
	Gpu::RenderTarget::activate(target);
	
	/* set the viewport region */
	Gpu::State::set_viewport(viewport);
	
	/* set the scissor test and region */
	if (scissor_test) {
		Gpu::State::enable_scissor_test();
		Gpu::State::set_scissor(scissor);
	} else {
		Gpu::State::disable_scissor_test();
	}
	
	/* render the scene to buffer */
	Gpu::State::set_depth_writemask(false);
	render_to_buffer(s, c, rendering_mode, true);
	Gpu::State::set_depth_writemask(true);
	
	/* set back to the default render target */
	Gpu::RenderTarget::activate(nullptr);
}

void Renderer::render_shadow(const Scene& s, const Shadow& t) const {
	/* activate the render target */
	Gpu::RenderTarget::activate(t.get_target());
	
	/* set the viewport to fit the resolution of shadow map */
	Vec2 resolution = Shadow::get_resolution();
	Gpu::State::set_viewport(Gpu::Rect(resolution.x, resolution.y));
	
	/* disable the scissor test */
	Gpu::State::disable_scissor_test();
	
	/* clear the shadow map (depth only) */
	Gpu::State::clear(false, true, false);
	
	/* render the scene to shadow */
	render_to_shadow(s, t.camera);
	
	/* set back to the default render target */
	Gpu::RenderTarget::activate(nullptr);
}

void Renderer::update_shadow(const Scene& s, SpotLight& l) const {
	Vec3 up = l.direction.cross({0, 0, 1});
	if (up.magnitude() < 1E-4) up = {0, 1, 0};
	l.shadow.camera.lookat(l.position, -l.direction, up);
	render_shadow(s, l.shadow);
}

void Renderer::update_shadow(const Scene& s, DirectionalLight& l) const {
	Vec3 up = l.direction.cross({0, 0, 1});
	if (up.magnitude() < 1E-4) up = {0, 1, 0};
	l.shadow.camera.lookat(l.position, -l.direction, up);
	render_shadow(s, l.shadow);
}

void Renderer::update_probe(const Scene& s, ReflectionProbe& r) const {
	/* set the viewport with the resolution of probe */
	Gpu::State::set_viewport(Gpu::Rect(r.resolution, r.resolution));
	
	/* disable the scissor test */
	Gpu::State::disable_scissor_test();
	
	/* prepare texture for probe */
	if (!probe_map) {
		probe_map = std::make_unique<Gpu::Texture>();
	}
	probe_map->init_cube(r.resolution, r.resolution, TEXTURE_R8G8B8A8_UNORM);
	probe_map->set_filters(TEXTURE_LINEAR, TEXTURE_LINEAR);
	
	/* prepare depth texture for rendering */
	if (!probe_buffer) {
		probe_buffer = std::make_unique<Gpu::RenderBuffer>();
	}
	probe_buffer->init(r.resolution, r.resolution, TEXTURE_D24_UNORM);
	
	/* prepare frame buffer for probe */
	if (!probe_target) {
		probe_target = std::make_unique<Gpu::RenderTarget>();
	}
	probe_target->set_depth_buffer(*probe_buffer);
	
	/* initialize camera for probe */
	PerspCamera camera = PerspCamera(PI_2, 1, 0.1, 100);
	
	for (int i = 0; i < 6; ++i) {
		/* update camera for each side of cube */
		camera.lookat(r.position, CUBE_DIRECTIONS[i], CUBE_UPS[i]);
		
		/* update and activate render target */
		probe_target->set_texture(*probe_map, 0, 0, i);
		Gpu::RenderTarget::activate(probe_target.get());
		
		/* clear depth buffer before rendering */
		Gpu::State::set_clear_color(clear_color);
		Gpu::State::clear(true, true, false);
		
		/* render skybox loaded by renderer */
		if (skybox_map) {
			render_skybox_to_buffer(camera, FORWARD_RENDERING);
		}
		
		/* render the opaque objects in scene */
		render_to_buffer(s, camera, FORWARD_RENDERING, false);
		
		/* render the transparent objects in scene */
		Gpu::State::set_depth_writemask(false);
		render_to_buffer(s, camera, FORWARD_RENDERING, true);
		Gpu::State::set_depth_writemask(true);
	}
	
	/* set back to the default render target */
	Gpu::RenderTarget::activate(nullptr);
	
	/* update probe with probe texture */
	r.load_texture(*probe_map);
}

void Renderer::update_scene(Scene& s) {
	s.update_instances();
}

void Renderer::set_material_defines(const Material& m, Defines& d) {
	/* check whether to use vertex color */
	d.set_if("USE_VERTEX_COLOR", m.use_vertex_color);
	
	/* check whether to use normal map */
	d.set_if("USE_NORMAL_MAP", m.normal_map != nullptr);
	
	/* check whether to use normal map in tangent space */
	d.set_if("USE_TANGENT_SPACE", m.normal_map != nullptr && m.use_tangent_space);
	
	/* check whether to use normal map in object space */
	d.set_if("USE_OBJECT_SPACE", m.normal_map != nullptr && !m.use_tangent_space);
	
	/* check whether to use displacement map */
	d.set_if("USE_DISPLACEMENT_MAP", m.displacement_map != nullptr);
	
	/* check whether to use color map */
	d.set_if("USE_COLOR_MAP", m.color_map != nullptr && !m.use_map_with_alpha);
	
	/* check whether to use color map with alpha channel */
	d.set_if("USE_COLOR_ALPHA_MAP", m.color_map != nullptr && m.use_map_with_alpha);
	
	/* check whether to use alpha map */
	d.set_if("USE_ALPHA_MAP", m.alpha_map != nullptr);
	
	/* check whether to use emissive map */
	d.set_if("USE_EMISSIVE_MAP", m.emissive_map != nullptr);
	
	/* check whether to use ambient occlusion map */
	d.set_if("USE_AO_MAP", m.ao_map != nullptr);
	
	/* check whether to use metalness map */
	d.set_if("USE_METALNESS_MAP", m.metalness_map != nullptr);
	
	/* check whether to use roughness map */
	d.set_if("USE_ROUGHNESS_MAP", m.roughness_map != nullptr);
	
	/* check whether to use specular map */
	d.set_if("USE_SPECULAR_MAP", m.specular_map != nullptr);
	
	/* check whether to use reflection probe */
	d.set_if("USE_REFLECTION_PROBE", m.reflection_probe != nullptr);
}

void Renderer::set_scene_defines(const Scene& s, Defines& d) {
	/* set the sample numbers of shadow */
	d.set_i("SHADOW_SAMPLES", Shadow::get_samples());
	
	/* set the number of point lights */
	size_t light_count = s.get_point_light_count();
	d.set_l("NUM_POINT_LIGHT", light_count);
	
	/* set the number of spot lights */
	light_count = s.get_spot_light_count();
	d.set_l("NUM_SPOT_LIGHT", light_count);
	
	/* set the number of directional lights */
	light_count = s.get_directional_light_count();
	d.set_l("NUM_DIRECTIONAL_LIGHT", light_count);
	
	/* set the number of hemisphere lights */
	light_count = s.get_hemisphere_light_count();
	d.set_l("NUM_HEMISPHERE_LIGHT", light_count);
	
	/* set whether to use linear fog */
	d.set_if("USE_LINEAR_FOG", s.get_linear_fog() != nullptr);
	
	/* set whether to use exp square fog */
	d.set_if("USE_EXP2_FOG", s.get_exp2_fog() != nullptr);
}

void Renderer::set_tone_map_defines(int m, Defines& d) {
	/* set which to use linear tone mapping */
	if (m == LINEAR_TONE_MAP) {
		d.set("TONE_MAP", "linear_tone_map");
	}
	
	/* set which to use reinhard tone mapping */
	if (m == REINHARD_TONE_MAP) {
		d.set("TONE_MAP", "reinhard_tone_map");
	}
	
	/* set which to use optimized tone mapping */
	if (m == OPTIMIZED_TONE_MAP) {
		d.set("TONE_MAP", "optimized_tone_map");
	}
	
	/* set which to use ACES filmic tone mapping */
	if (m == ACES_FILMIC_TONE_MAP) {
		d.set("TONE_MAP", "aces_filmic_tone_map");
	}
}

void Renderer::set_light_uniforms(const Scene& s, const Gpu::Shader& shader) {
	/* determines whether to enable shadow */
	bool enable_shadow = false;
	
	/* apply point lights in lighting */
	size_t point_light_count = s.get_point_light_count();
	for (int i = 0; i < point_light_count; ++i) {
		
		/* pass the light information to shader */
		auto& light = *s.get_point_light(i);
		auto lights_i = fmt::format("point_lights[{}]", i);
		Vec3 light_color = light.color * light.intensity * PI;
		shader.set_uniform_i(lights_i + ".visible", light.visible);
		shader.set_uniform_v3(lights_i + ".position", light.position);
		shader.set_uniform_v3(lights_i + ".color", light_color);
		shader.set_uniform_f(lights_i + ".distance", light.distance);
		shader.set_uniform_f(lights_i + ".decay", light.decay);
	}
	
	/* apply spot lights in lighting */
	size_t spot_light_count = s.get_spot_light_count();
	for (int i = 0; i < spot_light_count; ++i) {
		
		/* pass the light information to shader */
		auto& light = *s.get_spot_light(i);
		std::string lights_i = fmt::format("spot_lights[{}]", i);
		Vec3 light_direction = -light.direction.normalize();
		Vec3 light_color = light.color * light.intensity * PI;
		float light_angle = cosf(light.angle);
		float light_penumbra = cosf(light.angle * (1 - light.penumbra));
		shader.set_uniform_i(lights_i + ".visible", light.visible);
		shader.set_uniform_v3(lights_i + ".position", light.position);
		shader.set_uniform_v3(lights_i + ".direction", light_direction);
		shader.set_uniform_v3(lights_i + ".color", light_color);
		shader.set_uniform_f(lights_i + ".distance", light.distance);
		shader.set_uniform_f(lights_i + ".decay", light.decay);
		shader.set_uniform_f(lights_i + ".angle", light_angle);
		shader.set_uniform_f(lights_i + ".penumbra", light_penumbra);
		shader.set_uniform_i(lights_i + ".cast_shadow", light.cast_shadow);
		
		/* check whether the light cast shadow */
		if (!light.cast_shadow) continue;
		enable_shadow = true;
		
		/* pass the shadow information to shader */
		auto& shadow = light.shadow;
		std::string shadows_i = fmt::format("spot_lights[{}].shadow", i);
		Mat4 view_proj = shadow.camera.projection * shadow.camera.viewing;
		shader.set_uniform_i(shadows_i + ".type", shadow.type);
		shader.set_uniform_i(shadows_i + ".map_id", shadow.map_id);
		shader.set_uniform_f(shadows_i + ".bias", shadow.bias);
		shader.set_uniform_f(shadows_i + ".normal_bias", shadow.normal_bias);
		shader.set_uniform_f(shadows_i + ".radius", shadow.radius);
		shader.set_uniform_m4(shadows_i + ".view_proj", view_proj);
	}
	
	/* apply directional lights in lighting */
	size_t directional_light_count = s.get_directional_light_count();
	for (int i = 0; i < directional_light_count; ++i) {
		
		/* pass the light information to shader */
		auto& light = *s.get_directional_light(i);
		std::string lights_i = fmt::format("directional_lights[{}]", i);
		Vec3 light_direction = -light.direction.normalize();
		Vec3 light_color = light.color * light.intensity * PI;
		shader.set_uniform_i(lights_i + ".visible", light.visible);
		shader.set_uniform_v3(lights_i + ".direction", light_direction);
		shader.set_uniform_v3(lights_i + ".color", light_color);
		shader.set_uniform_i(lights_i + ".cast_shadow", light.cast_shadow);
		
		/* check whether the light cast shadow */
		if (!light.cast_shadow) continue;
		enable_shadow = true;
		
		/* pass the shadow information to shader */
		auto& shadow = light.shadow;
		std::string shadows_i = fmt::format("directional_lights[{}].shadow", i);
		Mat4 view_proj = shadow.camera.projection * shadow.camera.viewing;
		shader.set_uniform_i(shadows_i + ".type", shadow.type);
		shader.set_uniform_i(shadows_i + ".map_id", shadow.map_id);
		shader.set_uniform_f(shadows_i + ".bias", shadow.bias);
		shader.set_uniform_f(shadows_i + ".normal_bias", shadow.normal_bias);
		shader.set_uniform_f(shadows_i + ".radius", shadow.radius);
		shader.set_uniform_m4(shadows_i + ".view_proj", view_proj);
	}
	
	/* apply hemisphere lights in lighting */
	size_t hemisphere_light_count = s.get_hemisphere_light_count();
	for (int i = 0; i < hemisphere_light_count; ++i) {
		
		/* pass the light information to shader */
		auto& light = *s.get_hemisphere_light(i);
		std::string lights_i = fmt::format("hemisphere_lights[{}]", i);
		Vec3 light_sky_color = light.color * light.intensity * PI;
		Vec3 light_ground_color = light.ground_color * light.intensity * PI;
		shader.set_uniform_i(lights_i + ".visible", light.visible);
		shader.set_uniform_v3(lights_i + ".direction", light.direction);
		shader.set_uniform_v3(lights_i + ".sky_color", light_sky_color);
		shader.set_uniform_v3(lights_i + ".ground_color", light_ground_color);
	}
	
	/* pass the shadow parameters to shader */
	if (enable_shadow) Shadow::activate_texture(26);
	shader.set_uniform_i("global_shadow.map", 26);
	shader.set_uniform_v2("global_shadow.size", Shadow::get_resolution());
	
	/* pass the linear fog parameters to shader */
	auto* linear_fog = s.get_linear_fog();
	if (linear_fog != nullptr) {
		shader.set_uniform_i("fog.visible", linear_fog->visible);
		shader.set_uniform_v3("fog.color", linear_fog->color);
		shader.set_uniform_f("fog.near", linear_fog->near);
		shader.set_uniform_f("fog.far", linear_fog->far);
	}
	
	/* pass the exp square fog parameters to shader */
	auto* exp2_fog = s.get_exp2_fog();
	if (exp2_fog != nullptr) {
		shader.set_uniform_i("fog.visible", exp2_fog->visible);
		shader.set_uniform_v3("fog.color", exp2_fog->color);
		shader.set_uniform_f("fog.near", exp2_fog->near);
		shader.set_uniform_f("fog.density", exp2_fog->density);
	}
}

void Renderer::render_skybox_to_buffer(const Camera& c, RenderingMode r) const {
	/* initialize cube vertex object */
	[[maybe_unused]]
	static bool inited = init_cube();
	
	/* disable the depth & stencil test */
	Gpu::State::disable_depth_test();
	Gpu::State::disable_stencil_test();
	
	/* disable blending & wireframe */
	Gpu::State::disable_blending();
	Gpu::State::disable_wireframe();
	
	/* cull the front sides of faces */
	Gpu::State::enable_culling();
	Gpu::State::set_cull_side(FRONT_SIDE);
	
	/* render cube texture seamlessly */
	Gpu::State::enable_texture_cube_seamless();
	
	/* fetch the skybox shader from shader lib */
	Defines defines;
	if (r == FORWARD_RENDERING) {
		set_tone_map_defines(tone_map_mode, defines);
	} else {
		defines.set("TONE_MAP", "linear_tone_map");
	}
	int type = skybox_map->get_type();
	defines.set_if("USE_EQUIRECT", type == TEXTURE_2D);
	defines.set_if("USE_CUBEMAP", type == TEXTURE_CUBE);
	auto* shader = ShaderLib::fetch("Skybox", defines);
	
	/* calculate transform matrice */
	Mat4 viewing = c.viewing;
	for (int i = 0; i < 3; ++i) {
		viewing[i][3] = 0;
	}
	Mat4 view_proj = c.projection * viewing;
	
	/* calculate exposure in tone mapping */
	float exposure = 1;
	if (r == FORWARD_RENDERING) exposure = tone_map_exposure;
	
	/* render to the render target */
	shader->use_program();
	shader->set_uniform_m4("view_proj", view_proj);
	shader->set_uniform_f("exposure", exposure);
	shader->set_uniform_f("intensity", skybox_intensity);
	shader->set_uniform_i("map", skybox_map->activate(0));
	cube->attach(*shader);
	cube->render();
}

void Renderer::render_to_buffer(const Scene& s, const Camera& c, RenderingMode r, bool t) const {
	/* create transform matrices & vectors */
	Mat4 model;
	Mat4 view = c.viewing;
	Mat4 proj = c.projection;
	Mat4 model_view;
	Mat4 model_view_proj;
	Mat3 normal_mat;
	Vec3 camera_pos = c.position;
	
	/* sort the vector of visible instances */
	auto instances = s.to_visible_instances();
	sort_instances(c, instances, t);
	
	/* render all instances in sorted vector */
	for (auto& instance : instances) {
		
		/* get matrices from instance */
		model = instance->matrix_global;
		model_view = view * model;
		model_view_proj = proj * model_view;
		normal_mat = inverse_3x3(Mat3{
			model[0][0], model[1][0], model[2][0],
			model[0][1], model[1][1], model[2][1],
			model[0][2], model[1][2], model[2][2],
		});
		
		/* get mesh from instance */
		auto* mesh = instance->mesh;
		
		/* check whether the scene is loaded */
		if (mesh_cache.count(mesh) == 0) {
			return Error::set("Renderer", "Scene is not loaded");
		}
		
		/* get vertex objects from mesh cache */
		auto* vertex_object = mesh_cache.at(mesh).get();
		size_t group_size = mesh->groups.size();
		for (int i = 0; i < group_size; ++i) {
			
			/* get material from material groups */
			auto& group = mesh->groups[i];
			auto* material = s.get_material(group.name, *instance);
			if (material == nullptr) {
				material = s.get_material(group.name, *mesh);
			}
			if (material == nullptr) {
				material = s.get_material(group.name);
			}
			if (material == nullptr) {
				Error::set("Renderer", "Material is not linked");
				continue;
			}
			
			/* check whether the material is visible */
			if (!material->visible) continue;
			
			/* check whether the material is transparent */
			bool is_transparent = material->blending;
			if (is_transparent != t) continue;
			
			/* fetch the standard shader from shader lib */
			auto* shader = static_cast<const Gpu::Shader*>(material->shader);
			if (shader == nullptr) {
				Defines defines;
				set_material_defines(*material, defines);
				if (!t && r == DEFERRED_RENDERING) {
					/* use defines in deferred rendering */
					defines.set("DEFERRED_RENDERING");
				} else {
					/* use defines in forward rendering */
					defines.set("FORWARD_RENDERING");
					set_scene_defines(s, defines);
					set_tone_map_defines(tone_map_mode, defines);
				}
				shader = ShaderLib::fetch("Standard", defines);
			}
			
			/* render vertex object with shader */
			shader->use_program();
			vertex_object[i].attach(*shader);
			
			if (is_transparent || r == FORWARD_RENDERING) {
				/* pass camera parameters to shader */
				Mat4 inv_view_proj = inverse_4x4(c.projection * c.viewing);
				shader->set_uniform_m4("inv_view_proj", inv_view_proj);
				
				/* pass the tone mapping parameters to shader */
				shader->set_uniform_f("exposure", tone_map_exposure);
				
				/* pass the lights & fogs parameters to shader */
				set_light_uniforms(s, *shader);
			}
			
			/* pass the renderer parameters to shader */
			shader->set_uniform_m4("model"          , model          );
			shader->set_uniform_m4("view"           , view           );
			shader->set_uniform_m4("proj"           , proj           );
			shader->set_uniform_m4("model_view"     , model_view     );
			shader->set_uniform_m4("model_view_proj", model_view_proj);
			shader->set_uniform_m3("normal_mat"     , normal_mat     );
			shader->set_uniform_v3("camera_pos"     , camera_pos     );
			
			/* pass the material parameters to shader */
			shader->set_uniform_v3("color"      , material->color       );
			shader->set_uniform_f("alpha_test"  , material->alpha_test  );
			shader->set_uniform_f("alpha"       , material->alpha       );
			shader->set_uniform_f("ao_intensity", material->ao_intensity);
			shader->set_uniform_f("specular"    , material->specular    );
			shader->set_uniform_f("metalness"   , material->metalness   );
			shader->set_uniform_f("roughness"   , material->roughness   );
			
			/* pass the emissive parameter to shader */
			Vec3 emissive = material->emissive * material->emissive_intensity;
			shader->set_uniform_v3("emissive", emissive);
			
			/* pass the normal scale if use normal map */
			if (material->normal_map != nullptr) {
				shader->set_uniform_f("normal_scale", material->normal_scale);
			}
			
			/* pass the displacement scale if use displacement map */
			if (material->displacement_map != nullptr) {
				shader->set_uniform_f("displacement_scale", material->displacement_scale);
			}
			
			/* pass the images linked with material */
			for (int j = 0; j < 16; ++j) {
				auto* image = material->custom_maps[j];
				if (image != nullptr) image_cache.at(image)->activate(j);
			}
			if (material->normal_map != nullptr) {
				auto& map = image_cache.at(material->normal_map);
				shader->set_uniform_i("normal_map", map->activate(16));
			}
			if (material->displacement_map != nullptr) {
				auto& map = image_cache.at(material->displacement_map);
				shader->set_uniform_i("displacement_map", map->activate(17));
			}
			if (material->color_map != nullptr) {
				auto& map = image_cache.at(material->color_map);
				shader->set_uniform_i("color_map", map->activate(18));
			}
			if (material->alpha_map != nullptr) {
				auto& map = image_cache.at(material->alpha_map);
				shader->set_uniform_i("alpha_map", map->activate(19));
			}
			if (material->emissive_map != nullptr) {
				auto& map = image_cache.at(material->emissive_map);
				shader->set_uniform_i("emissive_map", map->activate(20));
			}
			if (material->ao_map != nullptr) {
				auto& map = image_cache.at(material->ao_map);
				shader->set_uniform_i("ao_map", map->activate(21));
			}
			if (material->roughness_map != nullptr) {
				auto& map = image_cache.at(material->roughness_map);
				shader->set_uniform_i("roughness_map", map->activate(22));
			}
			if (material->metalness_map != nullptr) {
				auto& map = image_cache.at(material->metalness_map);
				shader->set_uniform_i("metalness_map", map->activate(23));
			}
			if (material->specular_map != nullptr) {
				auto& map = image_cache.at(material->specular_map);
				shader->set_uniform_i("specular_map", map->activate(24));
			}
			
			/* pass the reflection probe linked with material */
			auto* ref_probe = static_cast<const ReflectionProbe*>(material->reflection_probe);
			if (ref_probe != nullptr) {
				int ref_lod = log2f(ref_probe->resolution);
				shader->set_uniform_i("ref_map", ref_probe->activate(25));
				shader->set_uniform_f("ref_lod", ref_lod);
				shader->set_uniform_f("ref_intensity", ref_probe->intensity);
			}
			
			/* pass the custom uniforms linked with material */
			if (material->uniforms != nullptr) {
				shader->set_uniforms(*material->uniforms);
			}
			
			/* apply the depth test setting for material */
			Gpu::MaterialState::set_depth(*material);
			
			/* apply the stencil test setting for material */
			Gpu::MaterialState::set_stencil(*material);
			
			/* apply the wireframe setting for material */
			Gpu::MaterialState::set_wireframe(*material);
			
			/* apply the blending setting for material */
			Gpu::MaterialState::set_blending(*material);
			
			/* apply the render side setting for material */
			Gpu::MaterialState::set_side(*material);
			
			/* cull back side of face if side is DOUBLE_SIDE */
			if (is_transparent && material->side == DOUBLE_SIDE) {
				Gpu::State::enable_culling();
				Gpu::State::set_cull_side(BACK_SIDE);
			}
			
			/* render the vertex object at group i */
			vertex_object[i].render();
		}
	}
}

void Renderer::render_to_shadow(const Scene& s, const Camera& c) const {
	/* create transform matrices & vectors */
	Mat4 model;
	Mat4 view = c.viewing;
	Mat4 proj = c.projection;
	Mat4 model_view;
	Mat4 model_view_proj;
	
	/* sort the vector of visible instances */
	auto instances = s.to_visible_instances();
	sort_instances(c, instances, false);
	
	/* render all instances in sorted vector */
	for (auto& instance : instances) {
		
		/* check whether the instance casts shadow */
		if (!instance->cast_shadow) continue;
		
		/* get matrices from instance */
		model = instance->matrix_global;
		model_view = view * model;
		model_view_proj = proj * model_view;
		
		/* get mesh from instance */
		auto* mesh = instance->mesh;
		
		/* check whether the scene is loaded */
		if (mesh_cache.count(mesh) == 0) {
			Error::set("Renderer", "Scene is not loaded");
			continue;
		}
		
		/* get vertex objects from cache */
		auto* vertex_object = mesh_cache.at(mesh).get();
		size_t group_size = mesh->groups.size();
		for (int i = 0; i < group_size; ++i) {
			
			/* get material from material groups */
			auto& group = mesh->groups[i];
			auto* material = s.get_material(group.name, *mesh);
			if (material == nullptr) {
				material = s.get_material(group.name);
			}
			if (material == nullptr) {
				Error::set("Renderer", "Material is not linked");
				continue;
			}
			
			/* whether to use color map and alpha map */
			bool use_color_map = material->color_map != nullptr && material->use_map_with_alpha;
			bool use_alpha_map = material->alpha_map != nullptr;
			
			/* fetch the shadow shader from shader lib */
			Defines defines;
			defines.set_if("USE_COLOR_MAP", use_color_map);
			defines.set_if("USE_ALPHA_MAP", use_alpha_map);
			auto* shader = ShaderLib::fetch("Shadow", defines);
			
			/* check whether the material is visible */
			if (!material->visible) continue;
			
			/* check whether the material is transparent */
			if (material->blending) continue;
			
			/* render vertex object with shader */
			shader->use_program();
			vertex_object[i].attach(*shader);
			
			/* pass the renderer parameters to shader */
			shader->set_uniform_m4("model_view_proj", model_view_proj);
			
			/* pass the material parameters to shader */
			shader->set_uniform_f("alpha", material->alpha);
			shader->set_uniform_f("alpha_test", material->alpha_test);
			shader->set_uniform_i("color_map", 0);
			shader->set_uniform_i("alpha_map", 1);
			
			/* activate color map linked with material */
			if (use_color_map) {
				image_cache.at(material->color_map)->activate(0);
			}
			if (use_alpha_map) {
				image_cache.at(material->alpha_map)->activate(1);
			}
			
			/* apply the depth test setting for material */
			Gpu::MaterialState::set_depth(*material);
			
			/* apply the stencil test setting for material */
			Gpu::MaterialState::set_stencil(*material);
			
			/* apply the wireframe setting for material */
			Gpu::MaterialState::set_wireframe(*material);
			
			/* apply the blending setting for material */
			Gpu::MaterialState::set_blending(*material);
			
			/* apply the render side setting for material */
			Gpu::MaterialState::set_shadow_side(*material);
			
			/* render the vertex object for group i */
			vertex_object[i].render();
		}
	}
}

bool Renderer::init_cube() {
	cube = std::make_unique<Gpu::VertexObject>();
	Mesh box = BoxMesh::create();
	cube->load(box, box.groups[0]);
	return true; /* finish */
}

void Renderer::sort_instances(const Camera& c, std::vector<const Instance*>& l, bool t) {
	using InstanceItem = std::pair<const Instance*, float>;
	std::vector<InstanceItem> sorted_instances;
	auto instances_size = l.size();
	sorted_instances.resize(instances_size);
	for (int i = 0; i < instances_size; ++i) {
		Vec3 position = l[i]->local_to_global({});
		sorted_instances[i].first = l[i];
		sorted_instances[i].second = (c.position - position).dot(c.direction);
	}
	auto compare = t ?
	[](const InstanceItem& a, const InstanceItem& b) -> bool {
		if (a.first->priority != b.first->priority) {
			return a.first->priority < b.first->priority;
		}
		return a.second > b.second;
	} :
	[](const InstanceItem& a, const InstanceItem& b) -> bool {
		if (a.first->priority != b.first->priority) {
			return a.first->priority < b.first->priority;
		}
		return a.second < b.second;
	};
	std::sort(sorted_instances.begin(), sorted_instances.end(), compare);
	for (int i = 0; i < instances_size; ++i) {
		l[i] = sorted_instances[i].first;
	}
}

std::unique_ptr<Gpu::VertexObject> Renderer::cube;

std::unique_ptr<Gpu::Texture> Renderer::probe_map;

std::unique_ptr<Gpu::RenderBuffer> Renderer::probe_buffer;

std::unique_ptr<Gpu::RenderTarget> Renderer::probe_target;

}

/* -------------------------------------------------------------------------- */
/* ---- ink/postprocess/RenderPass.cxx -------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

const Gpu::RenderTarget* RenderPass::get_target() const {
	return target;
}

void RenderPass::set_target(const Gpu::RenderTarget* t) {
	target = t;
}

Gpu::Rect RenderPass::get_viewport() {
	return viewport;
}

void RenderPass::set_viewport(const Gpu::Rect& v) {
	viewport = v;
}

void RenderPass::render_to(const Gpu::Shader* s, const Gpu::RenderTarget* t) {
	/* initialize fullscreen plane */
	[[maybe_unused]]
	static bool inited = init_fullscreen_plane();
	
	/* activate render target */
	Gpu::RenderTarget::activate(t);
	
	/* disable depth & stencil & scissor test */
	Gpu::State::disable_depth_test();
	Gpu::State::disable_stencil_test();
	Gpu::State::disable_scissor_test();
	
	/* disable blending & wireframe & culling */
	Gpu::State::disable_blending();
	Gpu::State::disable_wireframe();
	Gpu::State::disable_culling();
	
	/* set the viewport region */
	Gpu::State::set_viewport(viewport);
	
	/* draw the fullscreen plane with shader */
	fullscreen_plane->attach(*s);
	fullscreen_plane->render();
	
	/* set to default render target */
	Gpu::RenderTarget::activate(nullptr);
}

bool RenderPass::init_fullscreen_plane() {
	/* prepare triangle mesh */
	Mesh triangle_mesh = Mesh("fullscreen");
	triangle_mesh.groups = {{"default", 0, 3}};
	triangle_mesh.vertex = {{-1, 3, 0}, {-1, -1, 0}, {3, -1, 0}};
	triangle_mesh.uv = {{0, 2}, {0, 0}, {2, 0}};
	
	/* prepare fullscreen plane vertex object */
	fullscreen_plane = std::make_unique<Gpu::VertexObject>();
	fullscreen_plane->load(triangle_mesh, triangle_mesh.groups[0]);
	
	return true; /* finish */
}

Gpu::Rect RenderPass::viewport;

std::unique_ptr<Gpu::VertexObject> RenderPass::fullscreen_plane;

}

/* -------------------------------------------------------------------------- */
/* ---- ink/postprocess/CopyPass.cxx ---------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

void CopyPass::init() {}

void CopyPass::render() {
	auto* copy_shader = ShaderLib::fetch("Copy");
	copy_shader->use_program();
	copy_shader->set_uniform_f("lod", 0);
	copy_shader->set_uniform_i("map", map->activate(0));
	RenderPass::render_to(copy_shader, target);
}

const Gpu::Texture* CopyPass::get_texture() const {
	return map;
}

void CopyPass::set_texture(const Gpu::Texture* t) {
	map = t;
}

}

/* -------------------------------------------------------------------------- */
/* ---- ink/postprocess/BlendPass.cxx --------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

void BlendPass::init() {}

void BlendPass::render() {
	Defines blend_defines;
	blend_defines.set("BLEND_OP(a, b)", operation);
	blend_defines.set("A_SWIZZLE", swizzle_a);
	blend_defines.set("B_SWIZZLE", swizzle_b);
	auto* blend_shader = ShaderLib::fetch("Blend", blend_defines);
	blend_shader->use_program();
	blend_shader->set_uniform_i("map_a", map_a->activate(0));
	blend_shader->set_uniform_i("map_b", map_b->activate(1));
	RenderPass::render_to(blend_shader, target);
}

const Gpu::Texture* BlendPass::get_texture_a() const {
	return map_a;
}

void BlendPass::set_texture_a(const Gpu::Texture* t) {
	map_a = t;
}

const Gpu::Texture* BlendPass::get_texture_b() const {
	return map_b;
}

void BlendPass::set_texture_b(const Gpu::Texture* t) {
	map_b = t;
}

std::string BlendPass::get_operation() const {
	return operation;
}

void BlendPass::set_operation(const std::string& o) {
	operation = o;
}

std::string BlendPass::get_swizzle_a() const {
	return swizzle_a;
}

void BlendPass::set_swizzle_a(const std::string& s) {
	swizzle_a = s;
}

std::string BlendPass::get_swizzle_b() const {
	return swizzle_b;
}

void BlendPass::set_swizzle_b(const std::string& s) {
	swizzle_b = s;
}

}

/* -------------------------------------------------------------------------- */
/* ---- ink/postprocess/BlurPass.cxx ---------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

const std::string TYPES[] = {
	"float", "vec2", "vec3", "vec4"
};

const std::string SWIZZLES[] = {
	".x", ".xy", ".xyz", ".xyzw"
};

BlurPass::BlurPass(int w, int h) : width(w), height(h) {}

void BlurPass::init() {
	/* check the width and height */
	if (width == 0 || height == 0) {
		return Error::set("BlurPass", "Width and height should be greater than 0");
	}
	
	/* get default format with channel */
	TextureFormat format = Gpu::Texture::default_format(channel, 1);
	
	/* prepare blur map 1 */
	blur_map_1 = std::make_unique<Gpu::Texture>();
	blur_map_1->init_2d(width / 2, height / 2, format);
	blur_map_1->set_filters(TEXTURE_NEAREST, TEXTURE_NEAREST);
	blur_map_1->set_wrap_all(TEXTURE_CLAMP_TO_EDGE);
	
	/* prepare blur map 2 */
	blur_map_2 = std::make_unique<Gpu::Texture>();
	blur_map_2->init_2d(width / 2, height / 2, format);
	blur_map_2->set_filters(TEXTURE_LINEAR, TEXTURE_LINEAR);
	blur_map_2->set_wrap_all(TEXTURE_CLAMP_TO_EDGE);
	
	/* prepare blur render target 1 */
	blur_target_1 = std::make_unique<Gpu::RenderTarget>();
	blur_target_1->set_texture(*blur_map_1, 0);
	
	/* prepare blur render target 2 */
	blur_target_2 = std::make_unique<Gpu::RenderTarget>();
	blur_target_2->set_texture(*blur_map_2, 0);
}

void BlurPass::render() {
	/* fetch box / Gaussian / bilateral blur shader from shader lib */
	Defines blur_defines;
	blur_defines.set("TYPE", TYPES[channel - 1]);
	blur_defines.set("SWIZZLE", SWIZZLES[channel - 1]);
	const Gpu::Shader* blur_shader = nullptr;
	if (type == BLUR_BOX) {
		blur_shader = ShaderLib::fetch("BoxBlur", blur_defines);
	} else if (type == BLUR_GAUSSIAN) {
		blur_shader = ShaderLib::fetch("GaussianBlur", blur_defines);
	} else if (type == BLUR_BILATERAL) {
		blur_shader = ShaderLib::fetch("BilateralBlur", blur_defines);
	}
	
	/* fetch copy shader from shader lib */
	auto* copy_shader = ShaderLib::fetch("Copy");
	
	/* calculate screen parameter */
	Vec2 screen_size = Vec2(width / 2, height / 2);
	
	/* change the current viewport */
	Gpu::Rect viewport = RenderPass::get_viewport();
	RenderPass::set_viewport(Gpu::Rect(width / 2, height / 2));
	
	/* 1. blur texture horizontally (down-sampling) */
	blur_shader->use_program();
	blur_shader->set_uniform_f("lod", 0);
	blur_shader->set_uniform_v2("direction", Vec2(1 / screen_size.x, 0));
	blur_shader->set_uniform_i("radius", radius);
	if (type == BLUR_GAUSSIAN || type == BLUR_BILATERAL) {
		blur_shader->set_uniform_f("sigma_s", sigma_s);
	}
	if (type == BLUR_BILATERAL) {
		blur_shader->set_uniform_f("sigma_r", sigma_r);
	}
	blur_shader->set_uniform_i("map", map->activate(0));
	RenderPass::render_to(blur_shader, blur_target_1.get());
	
	/* 2. blur texture vertically */
	blur_shader->use_program();
	blur_shader->set_uniform_f("lod", 0);
	blur_shader->set_uniform_v2("direction", Vec2(0, 1 / screen_size.y));
	blur_shader->set_uniform_i("radius", radius);
	if (type == BLUR_GAUSSIAN || type == BLUR_BILATERAL) {
		blur_shader->set_uniform_f("sigma_s", sigma_s);
	}
	if (type == BLUR_BILATERAL) {
		blur_shader->set_uniform_f("sigma_r", sigma_r);
	}
	blur_shader->set_uniform_i("map", blur_map_1->activate(0));
	RenderPass::render_to(blur_shader, blur_target_2.get());
	
	/* set back to the initial viewport */
	RenderPass::set_viewport(viewport);
	
	/* 3. render results to render target (up-sampling) */
	copy_shader->use_program();
	copy_shader->set_uniform_i("map", blur_map_2->activate(0));
	RenderPass::render_to(copy_shader, target);
}

const Gpu::Texture* BlurPass::get_texture() const {
	return map;
}

void BlurPass::set_texture(const Gpu::Texture* t) {
	map = t;
}

}

/* -------------------------------------------------------------------------- */
/* ---- ink/postprocess/LightPass.cxx --------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

void LightPass::init() {}

void LightPass::render() {
	/* fetch light shader from shader lib */
	Defines light_defines;
	Renderer::set_tone_map_defines(tone_map_mode, light_defines);
	Renderer::set_scene_defines(*scene, light_defines);
	auto* light_shader = ShaderLib::fetch("Lighting", light_defines);
	
	/* pass parameters and G-Buffers to shader */
	light_shader->use_program();
	light_shader->set_uniform_v3("camera_pos", camera->position);
	light_shader->set_uniform_f("exposure", tone_map_exposure);
	light_shader->set_uniform_i("g_color", g_color->activate(0));
	light_shader->set_uniform_i("g_normal", g_normal->activate(1));
	light_shader->set_uniform_i("g_material", g_material->activate(2));
	light_shader->set_uniform_i("g_light", g_light->activate(3));
	light_shader->set_uniform_i("z_map", z_map->activate(4));
	
	/* pass camera parameter to shader */
	Mat4 inv_view_proj = inverse_4x4(camera->projection * camera->viewing);
	light_shader->set_uniform_m4("inv_view_proj", inv_view_proj);
	
	/* pass the lights & fogs parameters to shader */
	Renderer::set_light_uniforms(*scene, *light_shader);
	
	/* render results to render target */
	RenderPass::render_to(light_shader, target);
}

const Scene* LightPass::get_scene() const {
	return scene;
}

void LightPass::set_scene(const Scene* s) {
	scene = s;
}

const Camera* LightPass::get_camera() const {
	return camera;
}

void LightPass::set_camera(const Camera* c) {
	camera = c;
}

int LightPass::get_tone_map_mode() const {
	return tone_map_mode;
}

float LightPass::get_tone_map_exposure() const {
	return tone_map_exposure;
}

void LightPass::set_tone_map(int m, float e) {
	tone_map_mode = m;
	tone_map_exposure = e;
}

const Gpu::Texture* LightPass::get_texture_color() const {
	return g_color;
}

void LightPass::set_texture_color(const Gpu::Texture* t) {
	g_color = t;
}

const Gpu::Texture* LightPass::get_texture_normal() const {
	return g_normal;
}

void LightPass::set_texture_normal(const Gpu::Texture* t) {
	g_normal = t;
}

const Gpu::Texture* LightPass::get_texture_material() const {
	return g_material;
}

void LightPass::set_texture_material(const Gpu::Texture* t) {
	g_material = t;
}

const Gpu::Texture* LightPass::get_texture_light() const {
	return g_light;
}

void LightPass::set_texture_light(const Gpu::Texture* t) {
	g_light = t;
}

const Gpu::Texture* LightPass::get_texture_depth() const {
	return z_map;
}

void LightPass::set_texture_depth(const Gpu::Texture* t) {
	z_map = t;
}

}

/* -------------------------------------------------------------------------- */
/* ---- ink/postprocess/SSAOPass.cxx ---------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

SSAOPass::SSAOPass(int w, int h, float r, float m, float i) :
width(w), height(h), radius(r), max_radius(m), intensity(i) {}

void SSAOPass::init() {
	/* check the width and height */
	if (width == 0 || height == 0) {
		return Error::set("SSAOPass", "Width and height should be greater than 0");
	}
	
	/* prepare blur map 1 */
	blur_map_1 = std::make_unique<Gpu::Texture>();
	blur_map_1->init_2d(width / 2, height / 2, TEXTURE_R8_UNORM);
	blur_map_1->set_filters(TEXTURE_NEAREST, TEXTURE_NEAREST);
	blur_map_1->set_wrap_all(TEXTURE_CLAMP_TO_EDGE);
	
	/* prepare blur map 2 */
	blur_map_2 = std::make_unique<Gpu::Texture>();
	blur_map_2->init_2d(width / 2, height / 2, TEXTURE_R8_UNORM);
	blur_map_2->set_filters(TEXTURE_LINEAR, TEXTURE_LINEAR);
	blur_map_2->set_wrap_all(TEXTURE_CLAMP_TO_EDGE);
	
	/* prepare blur render target 1 */
	blur_target_1 = std::make_unique<Gpu::RenderTarget>();
	blur_target_1->set_texture(*blur_map_1, 0);
	
	/* prepare blur render target 2 */
	blur_target_2 = std::make_unique<Gpu::RenderTarget>();
	blur_target_2->set_texture(*blur_map_2, 0);
}

void SSAOPass::render() {
	/* fetch SSAO shader from shader lib */
	Defines ssao_defines;
	ssao_defines.set("SAMPLES", std::to_string(samples));
	auto* ssao_shader = ShaderLib::fetch("SSAO", ssao_defines);
	
	/* fetch blur shader from shader lib */
	Defines blur_defines;
	blur_defines.set("TYPE", "float");
	blur_defines.set("SWIZZLE", ".x");
	auto* blur_shader = ShaderLib::fetch("BilateralBlur", blur_defines);
	
	/* fetch blend shader from shader lib */
	Defines blend_defines;
	blend_defines.set("BLEND_OP(a, b)", "a * b");
	blend_defines.set("A_SWIZZLE", ".xyzw");
	blend_defines.set("B_SWIZZLE", ".xxxx");
	auto* blend_shader = ShaderLib::fetch("Blend", blend_defines);
	
	/* calculate camera & screen parameters */
	Mat4 inv_proj = inverse_4x4(camera->projection);
	Vec2 screen_size = Vec2(width / 2, height / 2);
	
	/* change the current viewport */
	Gpu::Rect viewport = RenderPass::get_viewport();
	RenderPass::set_viewport(Gpu::Rect(width / 2, height / 2));
	
	/* 1. render SSAO to texture (down-sampling) */
	ssao_shader->use_program();
	ssao_shader->set_uniform_f("intensity", intensity);
	ssao_shader->set_uniform_f("radius", radius);
	ssao_shader->set_uniform_f("max_radius", max_radius);
	ssao_shader->set_uniform_f("max_z", max_z);
	ssao_shader->set_uniform_f("near", camera->near);
	ssao_shader->set_uniform_f("far", camera->far);
	ssao_shader->set_uniform_m4("view", camera->viewing);
	ssao_shader->set_uniform_m4("proj", camera->projection);
	ssao_shader->set_uniform_m4("inv_proj", inv_proj);
	ssao_shader->set_uniform_i("g_normal", g_normal->activate(0));
	ssao_shader->set_uniform_i("z_map", z_map->activate(1));
	RenderPass::render_to(ssao_shader, blur_target_1.get());
	
	/* 2. blur texture for two times */
	for (int i = 0; i < 2; ++i) {
		
		/* blur texture horizontally */
		blur_shader->use_program();
		blur_shader->set_uniform_f("lod", 0);
		blur_shader->set_uniform_v2("direction", Vec2(1 / screen_size.x, 0));
		blur_shader->set_uniform_i("radius", 7);
		blur_shader->set_uniform_f("sigma_s", 2);
		blur_shader->set_uniform_f("sigma_r", 0.25);
		blur_shader->set_uniform_i("map", blur_map_1->activate(0));
		RenderPass::render_to(blur_shader, blur_target_2.get());
		
		/* blur texture vertically */
		blur_shader->use_program();
		blur_shader->set_uniform_f("lod", 0);
		blur_shader->set_uniform_v2("direction", Vec2(0, 1 / screen_size.y));
		blur_shader->set_uniform_i("radius", 7);
		blur_shader->set_uniform_f("sigma_s", 2);
		blur_shader->set_uniform_f("sigma_r", 0.25);
		blur_shader->set_uniform_i("map", blur_map_2->activate(0));
		RenderPass::render_to(blur_shader, blur_target_1.get());
	}
	
	/* set back to the initial viewport */
	RenderPass::set_viewport(viewport);
	
	/* 3. render results to render target (up-sampling) */
	blend_shader->use_program();
	blend_shader->set_uniform_i("map_a", map->activate(1));
	blend_shader->set_uniform_i("map_b", blur_map_1->activate(0));
	RenderPass::render_to(blend_shader, target);
}

const Camera* SSAOPass::get_camera() const {
	return camera;
}

void SSAOPass::set_camera(const Camera* c) {
	camera = c;
}

const Gpu::Texture* SSAOPass::get_texture() const {
	return map;
}

void SSAOPass::set_texture(const Gpu::Texture* t) {
	map = t;
}

const Gpu::Texture* SSAOPass::get_texture_normal() const {
	return g_normal;
}

void SSAOPass::set_texture_normal(const Gpu::Texture* t) {
	g_normal = t;
}

const Gpu::Texture* SSAOPass::get_texture_depth() const {
	return z_map;
}

void SSAOPass::set_texture_depth(const Gpu::Texture* t) {
	z_map = t;
}

}

/* -------------------------------------------------------------------------- */
/* ---- ink/postprocess/SSRPass.cxx ----------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

SSRPass::SSRPass(int w, int h, float t, float i) :
width(w), height(h), thickness(t), intensity(i) {}

void SSRPass::init() {}

void SSRPass::render() {
	/* fetch SSR shader from shader lib */
	auto* ssr_shader = ShaderLib::fetch("SSR");
	
	/* calculate camera parameters */
	Gpu::Rect viewport = RenderPass::get_viewport();
	Vec2 screen_size = Vec2(viewport.width, viewport.height);
	Mat4 inv_proj = inverse_4x4(camera->projection);
	
	/* render SSR results to render target */
	ssr_shader->use_program();
	ssr_shader->set_uniform_i("max_steps", max_steps);
	ssr_shader->set_uniform_f("intensity", intensity);
	ssr_shader->set_uniform_f("thickness", thickness);
	ssr_shader->set_uniform_f("max_roughness", max_roughness);
	ssr_shader->set_uniform_f("near", camera->near);
	ssr_shader->set_uniform_f("far", camera->far);
	ssr_shader->set_uniform_v2("screen_size", screen_size);
	ssr_shader->set_uniform_m4("view", camera->viewing);
	ssr_shader->set_uniform_m4("proj", camera->projection);
	ssr_shader->set_uniform_m4("inv_proj", inv_proj);
	ssr_shader->set_uniform_i("map", map->activate(0));
	ssr_shader->set_uniform_i("g_normal", g_normal->activate(1));
	ssr_shader->set_uniform_i("g_material", g_material->activate(2));
	ssr_shader->set_uniform_i("z_map", z_map->activate(3));
	RenderPass::render_to(ssr_shader, target);
}

const Camera* SSRPass::get_camera() const {
	return camera;
}

void SSRPass::set_camera(const Camera* c) {
	camera = c;
}

const Gpu::Texture* SSRPass::get_texture() const {
	return map;
}

void SSRPass::set_texture(const Gpu::Texture* t) {
	map = t;
}

const Gpu::Texture* SSRPass::get_texture_normal() const {
	return g_normal;
}

void SSRPass::set_texture_normal(const Gpu::Texture* n) {
	g_normal = n;
}

const Gpu::Texture* SSRPass::get_texture_material() const {
	return g_material;
}

void SSRPass::set_texture_material(const Gpu::Texture* m) {
	g_material = m;
}

const Gpu::Texture* SSRPass::get_texture_depth() const {
	return z_map;
}

void SSRPass::set_texture_depth(const Gpu::Texture* t) {
	z_map = t;
}

}

/* -------------------------------------------------------------------------- */
/* ---- ink/postprocess/BloomPass.cxx --------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

BloomPass::BloomPass(int w, int h, float t, float i, float r) :
width(w), height(h), threshold(t), intensity(i), radius(r) {}

void BloomPass::init() {
	/* check the width and height */
	if (width == 0 || height == 0) {
		return Error::set("BloomPass", "Width and height should be greater than 0");
	}
	
	/* prepare bloom map 1 */
	bloom_map_1 = std::make_unique<Gpu::Texture>();
	bloom_map_1->init_2d(width / 2, height / 2, TEXTURE_R16G16B16_SFLOAT);
	bloom_map_1->set_filters(TEXTURE_LINEAR, TEXTURE_LINEAR_MIPMAP_LINEAR);
	bloom_map_1->set_wrap_all(TEXTURE_CLAMP_TO_EDGE);
	bloom_map_1->generate_mipmap();
	
	/* prepare bloom map 2 */
	bloom_map_2 = std::make_unique<Gpu::Texture>();
	bloom_map_2->init_2d(width / 2, height / 2, TEXTURE_R16G16B16_SFLOAT);
	bloom_map_2->set_filters(TEXTURE_LINEAR, TEXTURE_LINEAR_MIPMAP_LINEAR);
	bloom_map_2->set_wrap_all(TEXTURE_CLAMP_TO_EDGE);
	bloom_map_2->generate_mipmap();
	
	/* prepare bloom render target */
	bloom_target = std::make_unique<Gpu::RenderTarget>();
}

void BloomPass::render() {
	/* fetch bright pass shader from shader lib */
	auto* bright_pass_shader = ShaderLib::fetch("BrightPass");
	
	/* fetch blur shader from shader lib */
	Defines blur_defines;
	blur_defines.set("TYPE", "vec3");
	blur_defines.set("SWIZZLE", ".xyz");
	auto* blur_shader = ShaderLib::fetch("GaussianBlur", blur_defines);
	
	/* fetch bloom shader from shader lib */
	auto* bloom_shader = ShaderLib::fetch("Bloom");
	
	/* change the current viewport */
	Gpu::Rect viewport = RenderPass::get_viewport();
	RenderPass::set_viewport(Gpu::Rect(width / 2, height / 2));
	
	/* 1. render bright pixels to blur map 1 */
	bright_pass_shader->use_program();
	bright_pass_shader->set_uniform_f("threshold", threshold);
	bright_pass_shader->set_uniform_i("map", map->activate(0));
	bloom_target->set_texture(*bloom_map_1, 0, 0);
	RenderPass::render_to(bright_pass_shader, bloom_target.get());
	
	/* initialize size lod */
	Vec2 size_lod = Vec2(width / 2, height / 2);
	
	/* 2. blur texture on the mipmap chain */
	for (int lod = 0; lod < 5; ++lod) {
		int sigma = lod * 2 + 3;
		
		/* set the viewport of size lod */
		RenderPass::set_viewport(Gpu::Rect(size_lod.x, size_lod.y));
		
		/* blur texture horizontally */
		blur_shader->use_program();
		blur_shader->set_uniform_f("lod", fmax(0, lod - 1));
		blur_shader->set_uniform_v2("direction", {1 / size_lod.x, 0});
		blur_shader->set_uniform_i("radius", sigma * 3);
		blur_shader->set_uniform_f("sigma_s", sigma);
		blur_shader->set_uniform_i("map", bloom_map_1->activate(0));
		bloom_target->set_texture(*bloom_map_2, 0, lod);
		RenderPass::render_to(blur_shader, bloom_target.get());
		
		/* blur texture vertically */
		blur_shader->use_program();
		blur_shader->set_uniform_f("lod", lod);
		blur_shader->set_uniform_v2("direction", {0, 1 / size_lod.y});
		blur_shader->set_uniform_i("radius", sigma * 3);
		blur_shader->set_uniform_f("sigma_s", sigma);
		blur_shader->set_uniform_i("map", bloom_map_2->activate(0));
		bloom_target->set_texture(*bloom_map_1, 0, lod);
		RenderPass::render_to(blur_shader, bloom_target.get());
		
		/* update size lod to lower lod */
		size_lod.x = fmax(1, floorf(size_lod.x / 2));
		size_lod.y = fmax(1, floorf(size_lod.y / 2));
	}
	
	/* set back to the initial viewport */
	RenderPass::set_viewport(viewport);
	
	/* 3. blur texture on the mipmap chain */
	bloom_shader->use_program();
	bloom_shader->set_uniform_v3("tint", tint);
	bloom_shader->set_uniform_f("intensity", intensity);
	bloom_shader->set_uniform_f("radius", radius);
	bloom_shader->set_uniform_i("map", map->activate(0));
	bloom_shader->set_uniform_i("bloom_map", bloom_map_1->activate(1));
	RenderPass::render_to(bloom_shader, target);
}

const Gpu::Texture* BloomPass::get_texture() const {
	return map;
}

void BloomPass::set_texture(const Gpu::Texture* t) {
	map = t;
}

}

/* -------------------------------------------------------------------------- */
/* ---- ink/postprocess/FXAAPass.cxx ---------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

void FXAAPass::init() {}

void FXAAPass::render() {
	auto* fxaa_shader = ShaderLib::fetch("FXAA");
	Gpu::Rect viewport = RenderPass::get_viewport();
	Vec2 screen_size = Vec2(viewport.width, viewport.height);
	fxaa_shader->use_program();
	fxaa_shader->set_uniform_v2("screen_size", screen_size);
	fxaa_shader->set_uniform_i("map", map->activate(0));
	RenderPass::render_to(fxaa_shader, target);
}

const Gpu::Texture* FXAAPass::get_texture() const {
	return map;
}

void FXAAPass::set_texture(const Gpu::Texture* t) {
	map = t;
}

}

/* -------------------------------------------------------------------------- */
/* ---- ink/postprocess/GrainPass.cxx --------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

void GrainPass::init() {}

void GrainPass::render() {
	auto* grain_shader = ShaderLib::fetch("Grain");
	Gpu::Rect viewport = RenderPass::get_viewport();
	Vec2 screen_size = Vec2(viewport.width, viewport.height);
	grain_shader->use_program();
	grain_shader->set_uniform_f("intensity", intensity);
	grain_shader->set_uniform_f("seed", Random::random_f() + 1);
	grain_shader->set_uniform_i("map", map->activate(0));
	RenderPass::render_to(grain_shader, target);
}

const Gpu::Texture* GrainPass::get_texture() const {
	return map;
}

void GrainPass::set_texture(const Gpu::Texture* t) {
	map = t;
}

}

/* -------------------------------------------------------------------------- */
/* ---- ink/postprocess/ToneMapPass.cxx ------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

void ToneMapPass::init() {}

void ToneMapPass::render() {
	Defines tone_map_defines;
	Renderer::set_tone_map_defines(mode, tone_map_defines);
	auto* tone_map_shader = ShaderLib::fetch("ToneMapping", tone_map_defines);
	tone_map_shader->use_program();
	tone_map_shader->set_uniform_f("exposure", exposure);
	tone_map_shader->set_uniform_i("map", map->activate(0));
	RenderPass::render_to(tone_map_shader, target);
}

const Gpu::Texture* ToneMapPass::get_texture() const {
	return map;
}

void ToneMapPass::set_texture(const Gpu::Texture* t) {
	map = t;
}

}

/* -------------------------------------------------------------------------- */
/* ---- ink/postprocess/ColorGradePass.cxx ---------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

void ColorGradePass::init() {}

void ColorGradePass::render() {
	auto* color_grade_shader = ShaderLib::fetch("ColorGrading");
	color_grade_shader->use_program();
	color_grade_shader->set_uniform_v3("saturation", saturation);
	color_grade_shader->set_uniform_v3("contrast", contrast);
	color_grade_shader->set_uniform_v3("gamma", gamma);
	color_grade_shader->set_uniform_v3("gain", gain);
	color_grade_shader->set_uniform_v3("offset", offset);
	color_grade_shader->set_uniform_i("map", map->activate(0));
	RenderPass::render_to(color_grade_shader, target);
}

const Gpu::Texture* ColorGradePass::get_texture() const {
	return map;
}

void ColorGradePass::set_texture(const Gpu::Texture* t) {
	map = t;
}

}

/* -------------------------------------------------------------------------- */
/* ---- ink/audio/Audio.cxx ------------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

void Audio::init() {
	SDL_Init(SDL_INIT_AUDIO);
}

Audio::Audio(const std::string& p) {
	if (SDL_LoadWAV(p.c_str(), &spec, &buffer, &length) == nullptr) {
		auto error = SDL_GetError();
		printf("%s\n", error);
		Error::set("Audio", "Failed to read from WAVE file");
	}
	ratio = spec.channels * spec.freq * (spec.format >> 3 & 0x1F);
	spec.userdata = this;
	spec.callback = [](void* u, uint8_t* s, int l) -> void {
		auto* a = static_cast<Audio*>(u);
		uint8_t* buffer = a->buffer + a->position;
		uint32_t length = 0;
		uint32_t volume = SDL_MIX_MAXVOLUME * a->volume;
		if (a->position + l < a->length) {
			length = l;
			a->position += l;
		} else {
			length = a->length - a->position;
			a->position = 0;
			if (!a->loop) SDL_PauseAudioDevice(a->device, 1);
		}
		SDL_memset(s, 0, l);
		SDL_MixAudioFormat(s, buffer, a->spec.format, length, volume);
	};
	device = SDL_OpenAudioDevice(nullptr, 0, &spec, nullptr, 0);
}

Audio::~Audio() {
	SDL_CloseAudioDevice(device);
	SDL_FreeWAV(buffer);
}

void Audio::play() const {
	SDL_PauseAudioDevice(device, 0);
}

void Audio::pause() const {
	SDL_PauseAudioDevice(device, 1);
}

void Audio::stop() {
	SDL_PauseAudioDevice(device, 1);
	position = 0;
}

float Audio::get_duration() const {
	return length / ratio;
}

bool Audio::get_loop() const {
	return loop;
}

void Audio::set_loop(bool l) {
	loop = l;
}

float Audio::get_volume() const {
	return volume;
}

void Audio::set_volume(float v) {
	volume = v;
}

float Audio::get_position() const {
	return position / ratio;
}

void Audio::set_position(float p) {
	position = p * ratio;
}

}

/* -------------------------------------------------------------------------- */
/* ---- ink/window/Window.cxx ----------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

void Window::init(const std::string& t, int x, int y, int w, int h, bool d) {
	/* whether to center the window */
	if (x == -1) x = SDL_WINDOWPOS_CENTERED;
	if (y == -1) y = SDL_WINDOWPOS_CENTERED;
	
	/* initialize keydown with false */
	std::fill_n(keydown, 512, false);
	
	/* create a window in SDL system */
	SDL_Init(SDL_INIT_VIDEO);
	uint32_t flags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL;
	if (d) flags |= SDL_WINDOW_ALLOW_HIGHDPI;
	sdl_window = SDL_CreateWindow(t.c_str(), x, y, w, h, flags);
}

void Window::init_opengl(int v, int d, int s, int m, bool a) {
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, d);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, s);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, m != 0);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, m);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, a);
	context = SDL_GL_CreateContext(sdl_window);
	SDL_GL_SetSwapInterval(v);
	if (gladLoadGL() == 0) Error::set("Window", "Failed to load OpenGL");
}

void Window::close() {
	open = false;
	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(sdl_window);
	SDL_Quit();
}

void Window::update() {
	/* init key pressed & released array */
	std::fill_n(keypressed, 512, false);
	std::fill_n(keyreleased, 512, false);
	
	/* handle events */
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event_callback && !std::invoke(event_callback, event)) {
			continue;
		}
		int32_t keycode = event.key.keysym.sym;
		if (keycode > 127) keycode -= 1073741696;
		if (event.type == SDL_QUIT) {
			return close();
		} else if (event.type == SDL_KEYDOWN) {
			keypressed[keycode] = !keydown[keycode];
			keydown[keycode] = true;
		} else if (event.type == SDL_KEYUP) {
			keyreleased[keycode] = keydown[keycode];
			keydown[keycode] = false;
		} else if (event.type == SDL_MOUSEBUTTONDOWN) {
			if (event.button.button == SDL_BUTTON_LEFT) {
				keypressed[1] = !keydown[1];
				keydown[1] = true;
			} else if (event.button.button == SDL_BUTTON_RIGHT) {
				keypressed[2] = !keydown[2];
				keydown[2] = true;
			}
		} else if (event.type == SDL_MOUSEBUTTONUP) {
			if (event.button.button == SDL_BUTTON_LEFT) {
				keyreleased[1] = keydown[1];
				keydown[1] = false;
			} else if (event.button.button == SDL_BUTTON_RIGHT) {
				keyreleased[2] = keydown[2];
				keydown[2] = false;
			}
		} else if (event.type == SDL_MOUSEMOTION) {
			if (!ignore_cursor_motion) {
				cursor_x = event.motion.x;
				cursor_y = event.motion.y;
			}
		}
	}
	
	/* use ignore cursor motion to discard events */
	ignore_cursor_motion = false;
	
	/* lock cursor to the center of window */
	if (cursor_locked && SDL_GetKeyboardFocus()) {
		std::pair<int, int> size;
		SDL_GetWindowSize(sdl_window, &size.first, &size.second);
		SDL_WarpMouseInWindow(sdl_window, size.first / 2, size.second / 2);
	}
	
	/* refresh window */
	SDL_GL_SwapWindow(sdl_window);
	
	/* calculate delta time */
	uint32_t deltatime = SDL_GetTicks() - time;
	if (deltatime <= interval) SDL_Delay(interval - deltatime);
	time = SDL_GetTicks();
}

bool Window::is_open() {
	return open;
}

unsigned int Window::get_time() {
	return SDL_GetTicks();
}

std::string Window::get_title() {
	return SDL_GetWindowTitle(sdl_window);
}

void Window::set_title(const std::string& t) {
	SDL_SetWindowTitle(sdl_window, t.c_str());
}

std::pair<int, int> Window::get_size() {
	std::pair<int, int> size;
	SDL_GetWindowSize(sdl_window, &size.first, &size.second);
	return size;
}

std::pair<int, int> Window::get_position() {
	int position[2];
	SDL_GetWindowPosition(sdl_window, position, position + 1);
	return {position[0], position[1]};
}

void Window::set_position(int x, int y) {
	SDL_SetWindowPosition(sdl_window, x, y);
}

uint32_t Window::get_interval() {
	return interval;
}

void Window::set_interval(uint32_t i) {
	interval = i;
}

void Window::set_borderless(bool b) {
	SDL_SetWindowBordered(sdl_window, b ? SDL_FALSE : SDL_TRUE);
}

void Window::set_resizable(bool r) {
	SDL_SetWindowResizable(sdl_window, r ? SDL_TRUE : SDL_FALSE);
}

void Window::set_min_size(int w, int h) {
	SDL_SetWindowMinimumSize(sdl_window, w, h);
}

void Window::set_fullscreen(bool f) {
	SDL_SetWindowFullscreen(sdl_window, f ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
}

void Window::maximize() {
	SDL_MaximizeWindow(sdl_window);
}

void Window::minimize() {
	SDL_MinimizeWindow(sdl_window);
}

std::pair<int, int> Window::get_cursor_position() {
	return {cursor_x, cursor_y};
}

void Window::set_cursor_position(int x, int y) {
	SDL_WarpMouseInWindow(sdl_window, x, y);
	cursor_x = x;
	cursor_y = y;
	ignore_cursor_motion = true;
}

void Window::set_cursor_visible(bool v) {
	SDL_ShowCursor(v ? SDL_ENABLE : SDL_DISABLE);
}

void Window::set_cursor_locked(bool l) {
	cursor_locked = l;
}

bool Window::is_down(unsigned int k) {
	if (k > 127) k -= 1073741696;
	return keydown[k];
}

bool Window::is_pressed(unsigned int k) {
	if (k > 127) k -= 1073741696;
	return keypressed[k];
}

bool Window::is_released(unsigned int k) {
	if (k > 127) k -= 1073741696;
	return keyreleased[k];
}

void Window::set_event_callback(const EventCallback& f) {
	event_callback = f;
}

bool Window::open = true;
int Window::cursor_x = 0;
int Window::cursor_y = 0;
bool Window::cursor_locked = false;
bool Window::ignore_cursor_motion = false;

uint32_t Window::time = 0;
uint32_t Window::interval = 0;

Window::EventCallback Window::event_callback;

SDL_Window* Window::sdl_window = nullptr;
SDL_GLContext Window::context = nullptr;

bool Window::keydown[512];
bool Window::keypressed[512];
bool Window::keyreleased[512];

}

/* -------------------------------------------------------------------------- */
/* ---- ink/utils/ConvexHull.cxx -------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

void ConvexHull::add_vertex(const Vec3& v) {
	vertices.emplace_back(v);
}

size_t ConvexHull::get_vertex_count() const {
	return vertices.size();
}

Vec3 ConvexHull::get_vertex(int i) const {
	return vertices[i];
}

size_t ConvexHull::get_face_count() const {
	return faces.size();
}

std::array<int, 3> ConvexHull::get_face(int i) const {
	return faces[i];
}

void ConvexHull::compute() {
	insert_face(0, 1, 2);
	insert_face(2, 1, 0);
	size_t size = vertices.size();
	for (int i = 3; i < size; ++i) {
		std::unordered_set<long long> new_faces;
		auto face_iter = faces.begin();
		auto normal_iter = normals.begin();
		while (face_iter != faces.end()) {
			Vec3 dir = vertices[i] - vertices[(*face_iter)[0]];
			if (normal_iter->dot(dir) <= 0) {
				++face_iter;
				++normal_iter;
				continue;
			}
			for (int k = 0; k < 3; ++k) {
				long long u = (*face_iter)[k];
				long long v = (*face_iter)[(k + 1) % 3];
				long long uv = v << 32 | u;
				if (new_faces.count(uv) != 0) {
					new_faces.erase(uv);
				} else {
					new_faces.insert(u << 32 | v);
				}
			}
			face_iter = faces.erase(face_iter);
			normal_iter = normals.erase(normal_iter);
		}
		for (auto& f : new_faces) {
			insert_face(i, f >> 32, f & 0xFFFFFFFFll);
		}
	}
}

void ConvexHull::insert_face(int a, int b, int c) {
	faces.emplace_back(std::array<int, 3>{a, b, c});
	normals.emplace_back((vertices[b] - vertices[a]).cross(vertices[c] - vertices[a]));
}

}

/* -------------------------------------------------------------------------- */
/* ---- ink/utils/Viewer.cxx ------------------------------------------------ */
/* -------------------------------------------------------------------------- */

namespace Ink {

Viewer::Viewer(Camera* c, float s) : camera(c), speed(s) {}

void Viewer::update(float dt) {
	/* receive keyboard event */
	Vec3 move = {0, 0, 0};
	if (Window::is_down(key_up)) ++move.z;
	if (Window::is_down(key_down)) --move.z;
	if (Window::is_down(key_left)) --move.x;
	if (Window::is_down(key_right)) ++move.x;
	if (move.magnitude() != 0) move = speed * dt * move.normalize();
	
	/* get the center of window */
	auto size = Window::get_size();
	Vec2 center = Vec2(size.first, size.second) * .5;
	
	/* get the position of window's cursor */
	auto cursor_position = Window::get_cursor_position();
	Vec2 cursor = Vec2(cursor_position.first, cursor_position.second);
	Vec2 delta = center - cursor;
	
	/* update angle along Y- and Z-axis */
	axis_y += delta.x * sensitivity;
	axis_z += delta.y * sensitivity;
	if (axis_z > PI_2) axis_z = PI_2;
	if (axis_z < -PI_2) axis_z = -PI_2;
	
	/* update the viewing direction of camera */
	camera->direction.x = sinf(axis_y) * cosf(axis_z);
	camera->direction.y = sinf(axis_z);
	camera->direction.z = cosf(axis_y) * cosf(axis_z);
	
	/* update the view-up vector of camera */
	camera->up.x = -sinf(axis_y) * sinf(axis_z);
	camera->up.y = cosf(axis_z);
	camera->up.z = -cosf(axis_y) * sinf(axis_z);
	
	/* update the movement of camera */
	if (mode == VIEWER_WALK) {
		camera->position.x += move.z * sinf(axis_y) - move.x * cosf(axis_y);
		camera->position.z += move.z * cosf(axis_y) + move.x * sinf(axis_y);
	} else if (mode == VIEWER_FLY) {
		camera->position += move.z * camera->direction;
		camera->position += move.x * (camera->direction.cross(camera->up));
	}
	camera->lookat(camera->position, -camera->direction, camera->up);
}

Camera* Viewer::get_camera() const {
	return camera;
}

void Viewer::set_camera(Camera* c) {
	camera = c;
	set_direction(c->direction);
}

void Viewer::set_position(const Vec3& p) {
	camera->position = p;
}

void Viewer::set_direction(const Vec3& d) {
	Vec3 direction = -d.normalize();
	axis_z = asinf(direction.y);
	axis_y = asinf(direction.x / cosf(axis_z));
	if (std::isnan(axis_y)) axis_y = 0;
	if (cosf(axis_y) * direction.z < 0) axis_y = -axis_y + PI;
}

}

/* -------------------------------------------------------------------------- */
/* ---- ink/utils/ImageUtils.cxx -------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

float ImageUtils::nearest_sample(const Image& i, int c, float u, float v) {
	int x = roundf(u * (i.width - 1));
	x = x < 0 ? 0 : x;
	x = x >= i.width ? i.width - 1 : x;
	int y = roundf(v * (i.height - 1));
	y = y < 0 ? 0 : y;
	y = y >= i.height ? i.height - 1 : y;
	if (i.bytes == 1) {
		return i.data[(x + y * i.width) * i.channel + c] / 255.f;
	} else {
		auto* data = reinterpret_cast<const float*>(i.data.data());
		return data[(x + y * i.width) * i.channel + c] / 255.f;
	}
}

float ImageUtils::nearest_sample(const Image& i, int c, const Vec2& uv) {
	return nearest_sample(i, c, uv.x, uv.y);
}

float ImageUtils::linear_sample(const Image& i, int c, float u, float v) {
	int x_0 = u * (i.width - 1);
	x_0 = x_0 < 0 ? 0 : x_0;
	x_0 = x_0 >= i.width ? i.width - 1 : x_0;
	int x_1 = x_0 + 1;
	x_1 = x_1 >= i.width ? i.width - 1 : x_1;
	int y_0 = v * (i.height - 1);
	y_0 = y_0 < 0 ? 0 : y_0;
	y_0 = y_0 >= i.height ? i.height - 1 : y_0;
	int y_1 = y_0 + 1;
	y_1 = y_1 >= i.height ? i.height - 1 : y_1;
	if (i.bytes == 1) {
		auto* data = i.data.data();
		float v_0 = data[(x_0 + y_0 * i.width) * i.channel + c];
		float v_1 = data[(x_0 + y_1 * i.width) * i.channel + c];
		float v_2 = data[(x_1 + y_0 * i.width) * i.channel + c];
		float v_3 = data[(x_1 + y_1 * i.width) * i.channel + c];
		return (v_0 * (y_1 - v) + v_1 * (v - y_0)) * (x_1 - u) +
			(v_2 * (y_1 - v) + v_3 * (v - y_0)) * (u - x_0);
	} else {
		auto* data = reinterpret_cast<const float*>(i.data.data());
		float v_0 = data[(x_0 + y_0 * i.width) * i.channel + c];
		float v_1 = data[(x_0 + y_1 * i.width) * i.channel + c];
		float v_2 = data[(x_1 + y_0 * i.width) * i.channel + c];
		float v_3 = data[(x_1 + y_1 * i.width) * i.channel + c];
		return (v_0 * (y_1 - v) + v_1 * (v - y_0)) * (x_1 - u) +
			(v_2 * (y_1 - v) + v_3 * (v - y_0)) * (u - x_0);
	}
}

float ImageUtils::linear_sample(const Image& i, int c, const Vec2& uv) {
	return linear_sample(i, c, uv.x, uv.y);
}

}

/* -------------------------------------------------------------------------- */
/* ---- ink/utils/ColorUtils.cxx -------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

Vec3 ColorUtils::to_rgb(unsigned int c) {
	float r = (c / 0x10000) / 255.f;
	float g = (c / 0x00100 % 0x100) / 255.f;
	float b = (c / 0x00001 % 0x100) / 255.f;
	return {r, g, b};
}

Vec4 ColorUtils::to_rgba(unsigned int c) {
	float r = (c / 0x1000000) / 255.f;
	float g = (c / 0x0010000 % 0x100) / 255.f;
	float b = (c / 0x0000100 % 0x100) / 255.f;
	float a = (c / 0x0000001 % 0x100) / 255.f;
	return {r, g, b, a};
}

unsigned int ColorUtils::to_hex(const Vec3& c) {
	unsigned int r = roundf(c.x * 0xff) * 0x10000;
	unsigned int g = roundf(c.y * 0xff) * 0x00100;
	unsigned int b = roundf(c.z * 0xff) * 0x00001;
	return r + g + b;
}

unsigned int ColorUtils::to_hex(const Vec4& c) {
	unsigned int r = roundf(c.x * 0xff) * 0x1000000;
	unsigned int g = roundf(c.y * 0xff) * 0x0010000;
	unsigned int b = roundf(c.z * 0xff) * 0x0000100;
	unsigned int a = roundf(c.w * 0xff) * 0x0000001;
	return r + g + b + a;
}

}
