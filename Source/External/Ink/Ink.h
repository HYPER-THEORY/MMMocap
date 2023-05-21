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

#pragma once

#include <unordered_map>
#include <vector>
#include <algorithm>
#include <cmath>
#include <unordered_set>
#include <array>
#include <functional>
#include <sstream>
#include <string>
#include <memory>
#include <SDL2/SDL.h>
#include <random>

/* -------------------------------------------------------------------------- */
/* ---- ink/core/Error.h ---------------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

class Error {
public:
	using ErrorCallback = std::function<void(const std::string&)>;
	
	/**
	 * Returns the specific string describing the error.
	 */
	static std::string get();
	
	/**
	 * Sets the string describing the error. calling this function will replace
	 * the previous error message.
	 *
	 * \param m error message
	 */
	static void set(const std::string& m);
	
	/**
	 * Sets the string describing the error. calling this function will replace
	 * the previous error message.
	 *
	 * \param l error location
	 * \param m error message
	 */
	static void set(const std::string& l, const std::string& m);
	
	/**
	 * Clears the current error message.
	 */
	static void clear();
	
	/**
	 * Sets the callback that will be triggered when a new error occurred.
	 *
	 * \param f error callback function
	 */
	static void set_callback(const ErrorCallback& f);
	
private:
	static std::string message;
	
	static ErrorCallback callback;
};

}

/* -------------------------------------------------------------------------- */
/* ---- ink/core/File.h ----------------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

class File {
public:
	/**
	 * Reads the content from the specified file into a string.
	 *
	 * \param p the path to the file
	 */
	static std::string read(const std::string& p);
	
	/**
	 * Writes the content of string into the specified file.
	 *
	 * \param p the path to the file
	 * \param c content string
	 */
	static void write(const std::string& p, const std::string& c);
	
	/**
	 * Writes the content of string into the specified file.
	 *
	 * \param p the path to the file
	 * \param c content string
	 */
	static void write(const std::string& p, const char* c);
	
	/**
	 * Appends the content of string into the specified file.
	 *
	 * \param p the path to the file
	 * \param c content string
	 */
	static void append(const std::string& p, const std::string& c);
	
	/**
	 * Appends the content of string into the specified file.
	 *
	 * \param p the path to the file
	 * \param c content string
	 */
	static void append(const std::string& p, const char* c);
};

}

/* -------------------------------------------------------------------------- */
/* ---- ink/core/Date.h ----------------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

class Date {
public:
	int year = 0;                /**< the full year, start from 1900 */
	int month = 0;               /**< the month, 0 for January */
	int day = 0;                 /**< the day of month, range from 1 to 31 */
	int hours = 0;               /**< the hours, range from 0 to 23 */
	int minutes = 0;             /**< the minutes, range from 0 to 59 */
	int seconds = 0;             /**< the seconds, range from 0 to 59 */
	int milliseconds = 0;        /**< the milliseconds, range from 0 to 999 */
	
	/**
	 * Creates a new Date object.
	 */
	explicit Date() = default;
	
	/**
	 * Creates a new Date object and initializes it with year, month, day, hour,
	 * minute, secoud, millisecond.
	 *
	 * \param year the full year, start from 1900
	 * \param month the month, 0 for January
	 * \param day the day of month, range from 1 to 31
	 * \param h the hours, range from 0 to 23
	 * \param m the minutes, range from 0 to 59
	 * \param s the seconds, range from 0 to 59
	 * \param ms the milliseconds, range from 0 to 999
	 */
	explicit Date(int year, int month, int day, int h, int m, int s, int ms);
	
	/**
	 * Returns a string represents the date in YYYY-MM-DD HH:MM:SS format.
	 */
	std::string format() const;
	
	/**
	 * Returns the specified number of milliseconds since January 1, 1970,
	 * 00:00:00.
	 */
	static long long get_time();
	
	/**
	 * Returns the date which is initialized according to local time.
	 *
	 * \param t the custom time in milliseconds
	 */
	static Date get_local(long long t = -1);
	
	/**
	 * Returns the date which is initialized according to coordinated universal
	 * time (UTC) time.
	 *
	 * \param t the custom time in milliseconds
	 */
	static Date get_utc(long long t = -1);
};

}

/* -------------------------------------------------------------------------- */
/* ---- ink/math/Constants.h ------------------------------------------------ */
/* -------------------------------------------------------------------------- */

namespace Ink {

/* common math constants */
constexpr double E        = 2.71828182845904523;
constexpr double LN10     = 2.30258509299404568;
constexpr double LN2      = 0.69314718055994530;
constexpr double LOG10E   = 0.43429448190325182;
constexpr double LOG2E    = 1.44269504088896340;
constexpr double PI       = 3.14159265358979323;
constexpr double PI_2     = 1.57079632679489661;
constexpr double SQRT_1_2 = 0.70710678118654752;
constexpr double SQRT_2   = 1.41421356237309504;

/* angle conversion constants */
constexpr double RAD_TO_DEG = 57.29577951308232087;
constexpr double DEG_TO_RAD = 0.017453292519943295;

}

/* -------------------------------------------------------------------------- */
/* ---- ink/math/Random.h --------------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

class Random {
public:
	/**
	 * Generates a random uniformly distributed number in range [0, 1).
	 */
	static double random();
	
	/**
	 * Generates a random uniformly distributed number in range [0, 1).
	 */
	static float random_f();
	
	/**
	 * Sets the seed of the random number generator. Default is zero.
	 *
	 * \param s seed
	 */
	static void set_seed(unsigned int s);
	
private:
	static std::mt19937 generator;
};

}

/* -------------------------------------------------------------------------- */
/* ---- ink/math/Vector2.h -------------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

class FVec2 {
public:
	float x = 0;
	float y = 0;
	
	FVec2(float x = 0);
	
	FVec2(float x, float y);
	
	FVec2 operator-() const;
	
	bool operator==(const FVec2& v) const;
	
	void operator+=(float v);
	
	void operator+=(const FVec2& v);
	
	void operator-=(float v);
	
	void operator-=(const FVec2& v);
	
	void operator*=(float v);
	
	void operator*=(const FVec2& v);
	
	void operator/=(float v);
	
	void operator/=(const FVec2& v);
	
	float dot(const FVec2& v) const;
	
	float cross(const FVec2& v) const;
	
	float magnitude() const;
	
	float distance(const FVec2& v) const;
	
	FVec2 normalize() const;
	
	FVec2 rotate(float a) const;
	
	std::string to_string(int p = 2) const;
	
	static FVec2 random();
};

FVec2 operator+(const FVec2& v1, float v2);

FVec2 operator+(float v1, const FVec2& v2);

FVec2 operator+(const FVec2& v1, const FVec2& v2);

FVec2 operator-(const FVec2& v1, float v2);

FVec2 operator-(float v1, const FVec2& v2);

FVec2 operator-(const FVec2& v1, const FVec2& v2);

FVec2 operator*(const FVec2& v1, float v2);

FVec2 operator*(float v1, const FVec2& v2);

FVec2 operator*(const FVec2& v1, const FVec2& v2);

FVec2 operator/(const FVec2& v1, float v2);

FVec2 operator/(float v1, const FVec2& v2);

FVec2 operator/(const FVec2& v1, const FVec2& v2);

using Vec2 = FVec2;

class DVec2 {
public:
	double x = 0;
	double y = 0;
	
	DVec2(double x = 0);
	
	DVec2(double x, double y);
	
	DVec2 operator-() const;
	
	bool operator==(const DVec2& v) const;
	
	void operator+=(double v);
	
	void operator+=(const DVec2& v);
	
	void operator-=(double v);
	
	void operator-=(const DVec2& v);
	
	void operator*=(double v);
	
	void operator*=(const DVec2& v);
	
	void operator/=(double v);
	
	void operator/=(const DVec2& v);
	
	double dot(const DVec2& v) const;
	
	double cross(const DVec2& v) const;
	
	double magnitude() const;
	
	double distance(const DVec2& v) const;
	
	DVec2 normalize() const;
	
	DVec2 rotate(double a) const;
	
	std::string to_string(int p = 2) const;
	
	static DVec2 random();
};

DVec2 operator+(const DVec2& v1, double v2);

DVec2 operator+(double v1, const DVec2& v2);

DVec2 operator+(const DVec2& v1, const DVec2& v2);

DVec2 operator-(const DVec2& v1, double v2);

DVec2 operator-(double v1, const DVec2& v2);

DVec2 operator-(const DVec2& v1, const DVec2& v2);

DVec2 operator*(const DVec2& v1, double v2);

DVec2 operator*(double v1, const DVec2& v2);

DVec2 operator*(const DVec2& v1, const DVec2& v2);

DVec2 operator/(const DVec2& v1, double v2);

DVec2 operator/(double v1, const DVec2& v2);

DVec2 operator/(const DVec2& v1, const DVec2& v2);

}

/* -------------------------------------------------------------------------- */
/* ---- ink/math/Vector3.h -------------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

class FVec3 {
public:
	float x = 0;
	float y = 0;
	float z = 0;
	
	FVec3(float x = 0);
	
	FVec3(float x, float y, float z);
	
	FVec3(const FVec2& v, float z);
	
	FVec3(float x, const FVec2& v);
	
	FVec3 operator-() const;
	
	bool operator==(const FVec3& v) const;
	
	void operator+=(float v);
		
	void operator+=(const FVec3& v);
	
	void operator-=(float v);
	
	void operator-=(const FVec3& v);
	
	void operator*=(float v);
	
	void operator*=(const FVec3& v);
	
	void operator/=(float v);
	
	void operator/=(const FVec3& v);
	
	float dot(const FVec3& v) const;
	
	FVec3 cross(const FVec3& v) const;
	
	float magnitude() const;
	
	float distance(const FVec3& v) const;
	
	FVec3 normalize() const;
	
	FVec3 rotate(const FVec3& v, float a) const;
	
	std::string to_string(int p = 2) const;
	
	static FVec3 random();
};

FVec3 operator+(const FVec3& v1, float v2);

FVec3 operator+(float v1, const FVec3& v2);

FVec3 operator+(const FVec3& v1, const FVec3& v2);

FVec3 operator-(const FVec3& v1, float v2);

FVec3 operator-(float v1, const FVec3& v2);

FVec3 operator-(const FVec3& v1, const FVec3& v2);

FVec3 operator*(const FVec3& v1, float v2);

FVec3 operator*(float v1, const FVec3& v2);

FVec3 operator*(const FVec3& v1, const FVec3& v2);

FVec3 operator/(const FVec3& v1, float v2);

FVec3 operator/(float v1, const FVec3& v2);

FVec3 operator/(const FVec3& v1, const FVec3& v2);

using Vec3 = FVec3;

class DVec3 {
public:
	double x = 0;
	double y = 0;
	double z = 0;
	
	DVec3(double x = 0);
	
	DVec3(double x, double y, double z);
	
	DVec3(const DVec2& v, double z);
	
	DVec3(double x, const DVec2& v);
	
	DVec3 operator-() const;
	
	bool operator==(const DVec3& v) const;
	
	void operator+=(double v);
		
	void operator+=(const DVec3& v);
	
	void operator-=(double v);
	
	void operator-=(const DVec3& v);
	
	void operator*=(double v);
	
	void operator*=(const DVec3& v);
	
	void operator/=(double v);
	
	void operator/=(const DVec3& v);
	
	double dot(const DVec3& v) const;
	
	DVec3 cross(const DVec3& v) const;
	
	double magnitude() const;
	
	double distance(const DVec3& v) const;
	
	DVec3 normalize() const;
	
	DVec3 rotate(const DVec3& v, double a) const;
	
	std::string to_string(int p = 2) const;
	
	static DVec3 random();
};

DVec3 operator+(const DVec3& v1, double v2);

DVec3 operator+(double v1, const DVec3& v2);

DVec3 operator+(const DVec3& v1, const DVec3& v2);

DVec3 operator-(const DVec3& v1, double v2);

DVec3 operator-(double v1, const DVec3& v2);

DVec3 operator-(const DVec3& v1, const DVec3& v2);

DVec3 operator*(const DVec3& v1, double v2);

DVec3 operator*(double v1, const DVec3& v2);

DVec3 operator*(const DVec3& v1, const DVec3& v2);

DVec3 operator/(const DVec3& v1, double v2);

DVec3 operator/(double v1, const DVec3& v2);

DVec3 operator/(const DVec3& v1, const DVec3& v2);

}

/* -------------------------------------------------------------------------- */
/* ---- ink/math/Vector4.h -------------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

class FVec4 {
public:
	float x = 0;
	float y = 0;
	float z = 0;
	float w = 0;
	
	FVec4() = default;
	
	FVec4(float x);
	
	FVec4(float x, float y, float z, float w);
	
	FVec4(const FVec2& v, float z, float w);
	
	FVec4(float x, const FVec2& v, float w);
	
	FVec4(float x, float y, const FVec2& v);
	
	FVec4(const FVec2& v1, const FVec2& v2);
	
	FVec4(const FVec3& v, float w);
	
	FVec4(float x, const FVec3& v);
	
	FVec4 operator-() const;
	
	bool operator==(const FVec4& v) const;
	
	void operator+=(float v);
	
	void operator+=(const FVec4& v);
	
	void operator-=(float v);
	
	void operator-=(const FVec4& v);
	
	void operator*=(float v);
	
	void operator*=(const FVec4& v);
	
	void operator/=(float v);
	
	void operator/=(const FVec4& v);
	
	float dot(const FVec4& v) const;
	
	float magnitude() const;
	
	float distance(const FVec4& v) const;
	
	FVec4 normalize() const;
	
	std::string to_string(int p = 2) const;
};

FVec4 operator+(const FVec4& v1, float v2);

FVec4 operator+(float v1, const FVec4& v2);

FVec4 operator+(const FVec4& v1, const FVec4& v2);

FVec4 operator-(const FVec4& v1, float v2);

FVec4 operator-(float v1, const FVec4& v2);

FVec4 operator-(const FVec4& v1, const FVec4& v2);

FVec4 operator*(const FVec4& v1, float v2);

FVec4 operator*(float v1, const FVec4& v2);

FVec4 operator*(const FVec4& v1, const FVec4& v2);

FVec4 operator/(const FVec4& v1, float v2);

FVec4 operator/(float v1, const FVec4& v2);

FVec4 operator/(const FVec4& v1, const FVec4& v2);

using Vec4 = FVec4;

class DVec4 {
public:
	double x = 0;
	double y = 0;
	double z = 0;
	double w = 0;
	
	DVec4() = default;
	
	DVec4(double x);
	
	DVec4(double x, double y, double z, double w);
	
	DVec4(const DVec2& v, double z, double w);
	
	DVec4(double x, const DVec2& v, double w);
	
	DVec4(double x, double y, const DVec2& v);
	
	DVec4(const DVec2& v1, const DVec2& v2);
	
	DVec4(const DVec3& v, double w);
	
	DVec4(double x, const DVec3& v);
	
	DVec4 operator-() const;
	
	bool operator==(const DVec4& v) const;
	
	void operator+=(double v);
	
	void operator+=(const DVec4& v);
	
	void operator-=(double v);
	
	void operator-=(const DVec4& v);
	
	void operator*=(double v);
	
	void operator*=(const DVec4& v);
	
	void operator/=(double v);
	
	void operator/=(const DVec4& v);
	
	double dot(const DVec4& v) const;
	
	double magnitude() const;
	
	double distance(const DVec4& v) const;
	
	DVec4 normalize() const;
	
	std::string to_string(int p = 2) const;
};

DVec4 operator+(const DVec4& v1, double v2);

DVec4 operator+(double v1, const DVec4& v2);

DVec4 operator+(const DVec4& v1, const DVec4& v2);

DVec4 operator-(const DVec4& v1, double v2);

DVec4 operator-(double v1, const DVec4& v2);

DVec4 operator-(const DVec4& v1, const DVec4& v2);

DVec4 operator*(const DVec4& v1, double v2);

DVec4 operator*(double v1, const DVec4& v2);

DVec4 operator*(const DVec4& v1, const DVec4& v2);

DVec4 operator/(const DVec4& v1, double v2);

DVec4 operator/(double v1, const DVec4& v2);

DVec4 operator/(const DVec4& v1, const DVec4& v2);

}

/* -------------------------------------------------------------------------- */
/* ---- ink/math/Matrix.h --------------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

template <int r, int c>
class FMat {
public:
	FMat();
	
	FMat(const FVec2& v);
	
	FMat(const FVec3& v);
	
	FMat(const FVec4& v);
	
	FMat(const std::initializer_list<float>& v);
	
	operator FVec2();
	
	operator FVec3();
	
	operator FVec4();
	
	float* operator[](size_t k);
	
	const float* operator[](size_t k) const;
	
	FMat<r, c> operator-() const;
	
	bool operator==(const FMat<r, c>& v) const;
	
	void operator+=(float v);
	
	void operator+=(const FMat<r, c>& v);
	
	void operator-=(float v);
	
	void operator-=(const FMat<r, c>& v);
	
	void operator*=(float v);
	
	void operator/=(float v);
	
	FMat<c, r> transpose() const;
	
	std::string to_string(int p = 2) const;
	
	static FMat<r, c> identity();
	
private:
	float m[r * c];
};

template <int r, int c>
FMat<r, c> operator+(const FMat<r, c>& v1, float v2);

template <int r, int c>
FMat<r, c> operator+(float v1, const FMat<r, c>& v2);

template <int r, int c>
FMat<r, c> operator+(const FMat<r, c>& v1, const FMat<r, c>& v2);

template <int r, int c>
FMat<r, c> operator-(const FMat<r, c>& v1, float v2);

template <int r, int c>
FMat<r, c> operator-(float v1, const FMat<r, c>& v2);

template <int r, int c>
FMat<r, c> operator-(const FMat<r, c>& v1, const FMat<r, c>& v2);

template <int r, int c>
FMat<r, c> operator*(const FMat<r, c>& v1, float v2);

template <int r, int c>
FMat<r, c> operator*(float v1, const FMat<r, c>& v2);

template <int l1, int l2, int l3>
FMat<l1, l3> operator*(const FMat<l1, l2>& v1, const FMat<l2, l3>& v2);

template <int r>
FMat<r, 1> operator*(const FMat<r, 2>& v1, const FVec2& v2);

template <int r>
FMat<r, 1> operator*(const FMat<r, 3>& v1, const FVec3& v2);

template <int r>
FMat<r, 1> operator*(const FMat<r, 4>& v1, const FVec4& v2);

template <int r, int c>
FMat<r, c> operator/(const FMat<r, c>& v1, float v2);

template <int r, int c>
FMat<r, c> operator/(float v1, const FMat<r, c>& v2);

template<int r, int c>
using Mat = FMat<r, c>;

using Mat2 = FMat<2, 2>;

using Mat3 = FMat<3, 3>;

using Mat4 = FMat<4, 4>;

float determinant_2x2(const Mat2& m);

float determinant_3x3(const Mat3& m);

float determinant_4x4(const Mat4& m);

Mat2 inverse_2x2(const Mat2& m);

Mat3 inverse_3x3(const Mat3& m);

Mat4 inverse_4x4(const Mat4& m);

template <int r, int c>
class DMat {
public:
	DMat();
	
	DMat(const DVec2& v);
	
	DMat(const DVec3& v);
	
	DMat(const DVec4& v);
	
	DMat(const std::initializer_list<double>& v);
	
	operator DVec2();
	
	operator DVec3();
	
	operator DVec4();
	
	double* operator[](size_t k);
	
	const double* operator[](size_t k) const;
	
	DMat<r, c> operator-() const;
	
	bool operator==(const DMat<r, c>& v) const;
	
	void operator+=(double v);
	
	void operator+=(const DMat<r, c>& v);
	
	void operator-=(double v);
	
	void operator-=(const DMat<r, c>& v);
	
	void operator*=(double v);
	
	void operator/=(double v);
	
	DMat<c, r> transpose() const;
	
	std::string to_string(int p = 2) const;
	
	static DMat<r, c> identity();
	
private:
	double m[r * c];
};

template <int r, int c>
DMat<r, c> operator+(const DMat<r, c>& v1, double v2);

template <int r, int c>
DMat<r, c> operator+(double v1, const DMat<r, c>& v2);

template <int r, int c>
DMat<r, c> operator+(const DMat<r, c>& v1, const DMat<r, c>& v2);

template <int r, int c>
DMat<r, c> operator-(const DMat<r, c>& v1, double v2);

template <int r, int c>
DMat<r, c> operator-(double v1, const DMat<r, c>& v2);

template <int r, int c>
DMat<r, c> operator-(const DMat<r, c>& v1, const DMat<r, c>& v2);

template <int r, int c>
DMat<r, c> operator*(const DMat<r, c>& v1, double v2);

template <int r, int c>
DMat<r, c> operator*(double v1, const DMat<r, c>& v2);

template <int l1, int l2, int l3>
DMat<l1, l3> operator*(const DMat<l1, l2>& v1, const DMat<l2, l3>& v2);

template <int r>
DMat<r, 1> operator*(const DMat<r, 2>& v1, const DVec2& v2);

template <int r>
DMat<r, 1> operator*(const DMat<r, 3>& v1, const DVec3& v2);

template <int r>
DMat<r, 1> operator*(const DMat<r, 4>& v1, const DVec4& v2);

template <int r, int c>
DMat<r, c> operator/(const DMat<r, c>& v1, double v2);

template <int r, int c>
DMat<r, c> operator/(double v1, const DMat<r, c>& v2);

using DMat2 = DMat<2, 2>;

using DMat3 = DMat<3, 3>;

using DMat4 = DMat<4, 4>;

double determinant_2x2(const DMat2& m);

double determinant_3x3(const DMat3& m);

double determinant_4x4(const DMat4& m);

DMat2 inverse_2x2(const DMat2& m);

DMat3 inverse_3x3(const DMat3& m);

DMat4 inverse_4x4(const DMat4& m);

template <int r, int c>
FMat<r, c>::FMat() {
	std::fill_n(m, r * c, 0);
}

template <int r, int c>
FMat<r, c>::FMat(const FVec2& v) {
	std::fill_n(m, r * c, 0);
	m[0] = v.x;
	m[1] = v.y;
}

template <int r, int c>
FMat<r, c>::FMat(const FVec3& v) {
	std::fill_n(m, r * c, 0);
	m[0] = v.x;
	m[1] = v.y;
	m[2] = v.z;
}

template <int r, int c>
FMat<r, c>::FMat(const FVec4& v) {
	std::fill_n(m, r * c, 0);
	m[0] = v.x;
	m[1] = v.y;
	m[2] = v.z;
	m[3] = v.w;
}

template <int r, int c>
FMat<r, c>::FMat(const std::initializer_list<float>& v) {
	std::fill_n(m, r * c, 0);
	std::copy(v.begin(), v.end(), m);
}

template <int r, int c>
FMat<r, c>::operator FVec2() {
	return {m[0], m[1]};
}

template <int r, int c>
FMat<r, c>::operator FVec3() {
	return {m[0], m[1], m[2]};
}

template <int r, int c>
FMat<r, c>::operator FVec4() {
	return {m[0], m[1], m[2], m[3]};
}

template <int r, int c>
float* FMat<r, c>::operator[](size_t k) {
	return m + k * c;
}

template <int r, int c>
const float* FMat<r, c>::operator[](size_t k) const {
	return m + k * c;
}

template <int r, int c>
FMat<r, c> FMat<r, c>::operator-() const {
	FMat<r, c> matrix;
	int i = r * c;
	while (i --> 0) matrix[0][i] = -m[i];
	return matrix;
}

template <int r, int c>
bool FMat<r, c>::operator==(const FMat<r, c>& v) const {
	int i = r * c;
	while (i --> 0) {
		if (m[i] != v[0][i]) return false;
	}
	return true;
}

template <int r, int c>
void FMat<r, c>::operator+=(float v) {
	int i = r * c;
	while (i --> 0) m[i] += v;
}

template <int r, int c>
void FMat<r, c>::operator+=(const FMat<r, c>& v) {
	int i = r * c;
	while (i --> 0) m[i] += v[0][i];
}

template <int r, int c>
void FMat<r, c>::operator-=(float v) {
	int i = r * c;
	while (i --> 0) m[i] -= v;
}

template <int r, int c>
void FMat<r, c>::operator-=(const FMat<r, c>& v) {
	int i = r * c;
	while (i --> 0) m[i] -= v[0][i];
}

template <int r, int c>
void FMat<r, c>::operator*=(float v) {
	int i = r * c;
	while (i --> 0) m[i] *= v;
}

template <int r, int c>
void FMat<r, c>::operator/=(float v) {
	int i = r * c;
	while (i --> 0) m[i] /= v;
}

template <int r, int c>
FMat<c, r> FMat<r, c>::transpose() const {
	FMat<c, r> matrix;
	for (int i = 0; i < r; ++i) {
		for (int j = 0; j < c; ++j) {
			matrix[j][i] = m[i * c + j];
		}
	}
	return matrix;
}

template <int r, int c>
std::string FMat<r, c>::to_string(int p) const {
	static std::stringstream stream;
	stream.setf(std::ios::fixed, std::ios::floatfield);
	stream.precision(p);
	stream.str(std::string());
	for (int i = 0; i < r; ++i) {
		stream << "[ ";
		for (int j = 0; j < c - 1; ++j) {
			stream << m[i * c + j] << ", ";
		}
		stream << m[i * c + c - 1] << " ]\n";
	}
	return stream.str();
}

template <int r, int c>
FMat<r, c> FMat<r, c>::identity() {
	FMat<r, c> matrix;
	int i = std::min(r, c);
	while (i --> 0) matrix[i][i] = 1;
	return matrix;
}

template <int r, int c>
FMat<r, c> operator+(const FMat<r, c>& v1, float v2) {
	FMat<r, c> matrix;
	int i = r * c;
	while (i --> 0) matrix[0][i] = v1[0][i] + v2;
	return matrix;
}

template <int r, int c>
FMat<r, c> operator+(float v1, const FMat<r, c>& v2) {
	FMat<r, c> matrix;
	int i = r * c;
	while (i --> 0) matrix[0][i] = v1 + v2[0][i];
	return matrix;
}

template <int r, int c>
FMat<r, c> operator+(const FMat<r, c>& v1, const FMat<r, c>& v2) {
	FMat<r, c> matrix;
	int i = r * c;
	while (i --> 0) matrix[0][i] = v1[0][i] + v2[0][i];
	return matrix;
}

template <int r, int c>
FMat<r, c> operator-(const FMat<r, c>& v1, float v2) {
	FMat<r, c> matrix;
	int i = r * c;
	while (i --> 0) matrix[0][i] = v1[0][i] - v2;
	return matrix;
}

template <int r, int c>
FMat<r, c> operator-(float v1, const FMat<r, c>& v2) {
	FMat<r, c> matrix;
	int i = r * c;
	while (i --> 0) matrix[0][i] = v1 - v2[0][i];
	return matrix;
}

template <int r, int c>
FMat<r, c> operator-(const FMat<r, c>& v1, const FMat<r, c>& v2) {
	FMat<r, c> matrix;
	int i = r * c;
	while (i --> 0) matrix[0][i] = v1[0][i] - v2[0][i];
	return matrix;
}

template <int r, int c>
FMat<r, c> operator*(const FMat<r, c>& v1, float v2) {
	FMat<r, c> matrix;
	int i = r * c;
	while (i --> 0) matrix[0][i] = v1[0][i] * v2;
	return matrix;
}

template <int r, int c>
FMat<r, c> operator*(float v1, const FMat<r, c>& v2) {
	FMat<r, c> matrix;
	int i = r * c;
	while (i --> 0) matrix[0][i] = v1 * v2[0][i];
	return matrix;
}

template <int l1, int l2, int l3>
FMat<l1, l3> operator*(const FMat<l1, l2>& v1, const FMat<l2, l3>& v2) {
	FMat<l1, l3> matrix;
	std::fill_n(matrix[0], l1 * l3, 0);
	for (int i = 0; i < l1; ++i) {
		for (int j = 0; j < l2; ++j) {
			for (int k = 0; k < l3; ++k) {
				matrix[i][k] += v1[i][j] * v2[j][k];
			}
		}
	}
	return matrix;
}

template <int r>
FMat<r, 1> operator*(const FMat<r, 2>& v1, const FVec2& v2) {
	FMat<r, 1> matrix;
	for (int i = 0; i < r; ++i) {
		matrix[i][0] = v1[i][0] * v2.x + v1[i][1] * v2.y;
	}
	return matrix;
}

template <int r>
FMat<r, 1> operator*(const FMat<r, 3>& v1, const FVec3& v2) {
	FMat<r, 1> matrix;
	for (int i = 0; i < r; ++i) {
		matrix[i][0] = v1[i][0] * v2.x + v1[i][1] * v2.y + v1[i][2] * v2.z;
	}
	return matrix;
}

template <int r>
FMat<r, 1> operator*(const FMat<r, 4>& v1, const FVec4& v2) {
	FMat<r, 1> matrix;
	for (int i = 0; i < r; ++i) {
		matrix[i][0] = v1[i][0] * v2.x + v1[i][1] * v2.y + v1[i][2] * v2.z + v1[i][3] * v2.w;
	}
	return matrix;
}

template <int r, int c>
FMat<r, c> operator/(float v1, const FMat<r, c>& v2) {
	FMat<r, c> matrix;
	int i = r * c;
	while (i --> 0) matrix[0][i] = v1 / v2[0][i];
	return matrix;
}

template <int r, int c>
FMat<r, c> operator/(const FMat<r, c>& v1, float v2) {
	FMat<r, c> matrix;
	int i = r * c;
	while (i --> 0) matrix[0][i] = v1[0][i] / v2;
	return matrix;
}

template <int r, int c>
DMat<r, c>::DMat() {
	std::fill_n(m, r * c, 0);
}

template <int r, int c>
DMat<r, c>::DMat(const DVec2& v) {
	std::fill_n(m, r * c, 0);
	m[0] = v.x;
	m[1] = v.y;
}

template <int r, int c>
DMat<r, c>::DMat(const DVec3& v) {
	std::fill_n(m, r * c, 0);
	m[0] = v.x;
	m[1] = v.y;
	m[2] = v.z;
}

template <int r, int c>
DMat<r, c>::DMat(const DVec4& v) {
	std::fill_n(m, r * c, 0);
	m[0] = v.x;
	m[1] = v.y;
	m[2] = v.z;
	m[3] = v.w;
}

template <int r, int c>
DMat<r, c>::DMat(const std::initializer_list<double>& v) {
	std::fill_n(m, r * c, 0);
	std::copy(v.begin(), v.end(), m);
}

template <int r, int c>
DMat<r, c>::operator DVec2() {
	return {m[0], m[1]};
}

template <int r, int c>
DMat<r, c>::operator DVec3() {
	return {m[0], m[1], m[2]};
}

template <int r, int c>
DMat<r, c>::operator DVec4() {
	return {m[0], m[1], m[2], m[3]};
}

template <int r, int c>
double* DMat<r, c>::operator[](size_t k) {
	return m + k * c;
}

template <int r, int c>
const double* DMat<r, c>::operator[](size_t k) const {
	return m + k * c;
}

template <int r, int c>
DMat<r, c> DMat<r, c>::operator-() const {
	DMat<r, c> matrix;
	int i = r * c;
	while (i --> 0) matrix[0][i] = -m[i];
	return matrix;
}

template <int r, int c>
bool DMat<r, c>::operator==(const DMat<r, c>& v) const {
	int i = r * c;
	while (i --> 0) {
		if (m[i] != v[0][i]) return false;
	}
	return true;
}

template <int r, int c>
void DMat<r, c>::operator+=(double v) {
	int i = r * c;
	while (i --> 0) m[i] += v;
}

template <int r, int c>
void DMat<r, c>::operator+=(const DMat<r, c>& v) {
	int i = r * c;
	while (i --> 0) m[i] += v[0][i];
}

template <int r, int c>
void DMat<r, c>::operator-=(double v) {
	int i = r * c;
	while (i --> 0) m[i] -= v;
}

template <int r, int c>
void DMat<r, c>::operator-=(const DMat<r, c>& v) {
	int i = r * c;
	while (i --> 0) m[i] -= v[0][i];
}

template <int r, int c>
void DMat<r, c>::operator*=(double v) {
	int i = r * c;
	while (i --> 0) m[i] *= v;
}

template <int r, int c>
void DMat<r, c>::operator/=(double v) {
	int i = r * c;
	while (i --> 0) m[i] /= v;
}

template <int r, int c>
DMat<c, r> DMat<r, c>::transpose() const {
	DMat<c, r> matrix;
	for (int i = 0; i < r; ++i) {
		for (int j = 0; j < c; ++j) {
			matrix[j][i] = m[i * c + j];
		}
	}
	return matrix;
}

template <int r, int c>
std::string DMat<r, c>::to_string(int p) const {
	static std::stringstream stream;
	stream.setf(std::ios::fixed, std::ios::floatfield);
	stream.precision(p);
	stream.str(std::string());
	for (int i = 0; i < r; ++i) {
		stream << "[ ";
		for (int j = 0; j < c - 1; ++j) {
			stream << m[i * c + j] << ", ";
		}
		stream << m[i * c + c - 1] << " ]\n";
	}
	return stream.str();
}

template <int r, int c>
DMat<r, c> DMat<r, c>::identity() {
	DMat<r, c> matrix;
	int i = std::min(r, c);
	while (i --> 0) matrix[i][i] = 1;
	return matrix;
}

template <int r, int c>
DMat<r, c> operator+(const DMat<r, c>& v1, double v2) {
	DMat<r, c> matrix;
	int i = r * c;
	while (i --> 0) matrix[0][i] = v1[0][i] + v2;
	return matrix;
}

template <int r, int c>
DMat<r, c> operator+(double v1, const DMat<r, c>& v2) {
	DMat<r, c> matrix;
	int i = r * c;
	while (i --> 0) matrix[0][i] = v1 + v2[0][i];
	return matrix;
}

template <int r, int c>
DMat<r, c> operator+(const DMat<r, c>& v1, const DMat<r, c>& v2) {
	DMat<r, c> matrix;
	int i = r * c;
	while (i --> 0) matrix[0][i] = v1[0][i] + v2[0][i];
	return matrix;
}

template <int r, int c>
DMat<r, c> operator-(const DMat<r, c>& v1, double v2) {
	DMat<r, c> matrix;
	int i = r * c;
	while (i --> 0) matrix[0][i] = v1[0][i] - v2;
	return matrix;
}

template <int r, int c>
DMat<r, c> operator-(double v1, const DMat<r, c>& v2) {
	DMat<r, c> matrix;
	int i = r * c;
	while (i --> 0) matrix[0][i] = v1 - v2[0][i];
	return matrix;
}

template <int r, int c>
DMat<r, c> operator-(const DMat<r, c>& v1, const DMat<r, c>& v2) {
	DMat<r, c> matrix;
	int i = r * c;
	while (i --> 0) matrix[0][i] = v1[0][i] - v2[0][i];
	return matrix;
}

template <int r, int c>
DMat<r, c> operator*(const DMat<r, c>& v1, double v2) {
	DMat<r, c> matrix;
	int i = r * c;
	while (i --> 0) matrix[0][i] = v1[0][i] * v2;
	return matrix;
}

template <int r, int c>
DMat<r, c> operator*(double v1, const DMat<r, c>& v2) {
	DMat<r, c> matrix;
	int i = r * c;
	while (i --> 0) matrix[0][i] = v1 * v2[0][i];
	return matrix;
}

template <int l1, int l2, int l3>
DMat<l1, l3> operator*(const DMat<l1, l2>& v1, const DMat<l2, l3>& v2) {
	DMat<l1, l3> matrix;
	std::fill_n(matrix[0], l1 * l3, 0);
	for (int i = 0; i < l1; ++i) {
		for (int j = 0; j < l2; ++j) {
			for (int k = 0; k < l3; ++k) {
				matrix[i][k] += v1[i][j] * v2[j][k];
			}
		}
	}
	return matrix;
}

template <int r>
DMat<r, 1> operator*(const DMat<r, 2>& v1, const DVec2& v2) {
	DMat<r, 1> matrix;
	for (int i = 0; i < r; ++i) {
		matrix[i][0] = v1[i][0] * v2.x + v1[i][1] * v2.y;
	}
	return matrix;
}

template <int r>
DMat<r, 1> operator*(const DMat<r, 3>& v1, const DVec3& v2) {
	DMat<r, 1> matrix;
	for (int i = 0; i < r; ++i) {
		matrix[i][0] = v1[i][0] * v2.x + v1[i][1] * v2.y + v1[i][2] * v2.z;
	}
	return matrix;
}

template <int r>
DMat<r, 1> operator*(const DMat<r, 4>& v1, const DVec4& v2) {
	DMat<r, 1> matrix;
	for (int i = 0; i < r; ++i) {
		matrix[i][0] = v1[i][0] * v2.x + v1[i][1] * v2.y + v1[i][2] * v2.z + v1[i][3] * v2.w;
	}
	return matrix;
}

template <int r, int c>
DMat<r, c> operator/(double v1, const DMat<r, c>& v2) {
	DMat<r, c> matrix;
	int i = r * c;
	while (i --> 0) matrix[0][i] = v1 / v2[0][i];
	return matrix;
}

template <int r, int c>
DMat<r, c> operator/(const DMat<r, c>& v1, double v2) {
	DMat<r, c> matrix;
	int i = r * c;
	while (i --> 0) matrix[0][i] = v1[0][i] / v2;
	return matrix;
}

}

/* -------------------------------------------------------------------------- */
/* ---- ink/math/Ray.h ------------------------------------------------------ */
/* -------------------------------------------------------------------------- */

namespace Ink {

class Ray {
public:
	Vec3 origin;		/**< the origin of ray */
	Vec3 direction;		/**< the direction of ray, must be normalized */
	
	/**
	 * Creates a new Ray object.
	 */
	explicit Ray() = default;
	
	/**
	 * Creates a new Ray object and initializes it with origin and direction.
	 *
	 * \param o the origin of ray
	 * \param d the direction of ray, must be normalized
	 */
	explicit Ray(const Vec3& o, const Vec3& d);
	
	/**
	 * Intersects the ray with the AABB box. Returns the distance to the
	 * intersection point if there is an intersection, returns -1 otherwise.
	 *
	 * \param l the lower boundary of box
	 * \param u the upper boundary of box
	 */
	float intersect_box(const Vec3& l, const Vec3& u) const;
	
	/**
	 * Intersects the ray with the plane. Returns the distance to the
	 * intersection point if there is an intersection, returns -1 otherwise.
	 *
	 * \param n the normal of plane, must be normalized
	 * \param d the distance from the origin to the plane
	 */
	float intersect_plane(const Vec3& n, float d) const;
	
	/**
	 * Intersects the ray with the sphere. Returns the distance to the
	 * intersection point if there is an intersection, returns -1 otherwise.
	 *
	 * \param c the center of sphere
	 * \param r the radius of sphere
	 */
	float intersect_shpere(const Vec3& c, float r) const;
	
	/**
	 * Intersects the ray with the triangle ABC. Returns the distance to the
	 * intersection point if there is an intersection, returns -1 otherwise.
	 *
	 * \param a the vertex A of triangle
	 * \param b the vertex B of triangle
	 * \param c the vertex C of triangle
	 */
	float intersect_triangle(const Vec3& a, const Vec3& b, const Vec3& c) const;
};

}

/* -------------------------------------------------------------------------- */
/* ---- ink/math/Euler.h ---------------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

enum EulerOrder {
	EULER_XYZ,
	EULER_XZY,
	EULER_YXZ,
	EULER_YZX,
	EULER_ZXY,
	EULER_ZYX,
};

class Euler {
public:
	float x = 0;                     /**< the rotation angle of X axis */
	float y = 0;                     /**< the rotation angle of Y axis */
	float z = 0;                     /**< the rotation angle of Z axis */
	EulerOrder order = EULER_XYZ;    /**< the order of rotations */
	
	/**
	 * Creates a new Euler object.
	 */
	explicit Euler() = default;
	
	/**
	 * Creates a new Euler object and initializes it with rotations and order.
	 *
	 * \param x the rotation angle of X axis
	 * \param y the rotation angle of Y axis
	 * \param z the rotation angle of Z axis
	 * \param o the order of rotations
	 */
	explicit Euler(float x, float y, float z, EulerOrder o = EULER_XYZ);
	
	/**
	 * Creates a new Euler object and initializes it with rotations and order.
	 *
	 * \param r the rotation vector
	 * \param o the order of rotations
	 */
	explicit Euler(Vec3 r, EulerOrder o = EULER_XYZ);
	
	/**
	 * Transforms the Euler angles to rotation matrix.
	 */
	Mat3 to_rotation_matrix() const;
};

}

/* -------------------------------------------------------------------------- */
/* ---- ink/math/Color.h ---------------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

class Color {
public:
	/**
	 * Converts the specified color from RGB color space to SRGB color space.
	 *
	 * \param c color
	 */
	static Vec3 rgb_to_srgb(const Vec3& c);
	
	/**
	 * Converts the specified color from SRGB color space to RGB color space.
	 *
	 * \param c color
	 */
	static Vec3 srgb_to_rgb(const Vec3& c);
	
	/**
	 * Converts the specified color from RGB color space to XYZ color space.
	 *
	 * \param c color
	 */
	static Vec3 rgb_to_xyz(const Vec3& c);
	
	/**
	 * Converts the specified color from XYZ color space to RGB color space.
	 *
	 * \param c color
	 */
	static Vec3 xyz_to_rgb(const Vec3& c);
	
	/**
	 * Converts the specified color from RGB color space to HSV color space.
	 *
	 * \param c color
	 */
	static Vec3 rgb_to_hsv(const Vec3& c);
	
	/**
	 * Converts the specified color from HSV color space to RGB color space.
	 *
	 * \param c color
	 */
	static Vec3 hsv_to_rgb(const Vec3& c);
	
	/**
	 * Converts the specified color from RGB color space to HSL color space.
	 *
	 * \param c color
	 */
	static Vec3 rgb_to_hsl(const Vec3& c);
	
	/**
	 * Converts the specified color from HSL color space to RGB color space.
	 *
	 * \param c color
	 */
	static Vec3 hsl_to_rgb(const Vec3& c);
	
	/**
	 * Converts the specified color from RGB color space to HCY color space.
	 *
	 * \param c color
	 */
	static Vec3 rgb_to_hcy(const Vec3& c);
	
	/**
	 * Converts the specified color from HCY color space to RGB color space.
	 *
	 * \param c color
	 */
	static Vec3 hcy_to_rgb(const Vec3& c);
	
private:
	static inline float saturate(float v);
	
	static inline float rgb_to_srgb(float v);
	
	static inline float srgb_to_rgb(float v);
	
	static inline Vec3 hue_to_rgb(float h);
	
	static inline Vec3 rgb_to_hcv(const Vec3& c);
};

}

/* -------------------------------------------------------------------------- */
/* ---- ink/objects/Enums.h ------------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

enum ColorConversion {
	COLOR_RGB_TO_BGR,
	COLOR_BGR_TO_RGB,
	COLOR_SRGB_TO_RGB,
	COLOR_RGB_TO_SRGB,
	COLOR_RGB_TO_XYZ,
	COLOR_XYZ_TO_RGB,
	COLOR_RGB_TO_HSV,
	COLOR_HSV_TO_RGB,
	COLOR_RGB_TO_HSL,
	COLOR_HSL_TO_RGB,
	COLOR_RGB_TO_HCY,
	COLOR_HCY_TO_RGB,
};

enum RenderSide {
	FRONT_SIDE,
	BACK_SIDE,
	DOUBLE_SIDE,
};

enum ImageType {
	IMAGE_UBYTE,
	IMAGE_BYTE,
	IMAGE_USHORT,
	IMAGE_SHORT,
	IMAGE_UINT,
	IMAGE_INT,
	IMAGE_HALF_FLOAT,
	IMAGE_FLOAT,
	IMAGE_UINT_24_8,
};

enum ImageFormat {
	IMAGE_COLOR,
	IMAGE_COLOR_INTEGER,
	IMAGE_DEPTH,
	IMAGE_STENCIL,
	IMAGE_DEPTH_STENCIL,
};

enum ComparisonFunc {
	FUNC_NEVER,
	FUNC_LESS,
	FUNC_EQUAL,
	FUNC_LEQUAL,
	FUNC_GREATER,
	FUNC_NOTEQUAL,
	FUNC_GEQUAL,
	FUNC_ALWAYS,
};

enum StencilOperation {
	STENCIL_ZERO,
	STENCIL_KEEP,
	STENCIL_REPLACE,
	STENCIL_INCR,
	STENCIL_DECR,
	STENCIL_INCR_WRAP,
	STENCIL_DECR_WRAP,
	STENCIL_INVERT,
};

enum BlendOperation {
	BLEND_ADD,
	BLEND_SUBTRACT,
	BLEND_REVERSE_SUBTRACT,
	BLEND_MIN,
	BLEND_MAX,
};

enum BlendFactor {
	FACTOR_ZERO,
	FACTOR_ONE,
	FACTOR_SRC_COLOR,
	FACTOR_ONE_MINUS_SRC_COLOR,
	FACTOR_DST_COLOR,
	FACTOR_ONE_MINUS_DST_COLOR,
	FACTOR_SRC_ALPHA,
	FACTOR_ONE_MINUS_SRC_ALPHA,
	FACTOR_DST_ALPHA,
	FACTOR_ONE_MINUS_DST_ALPHA,
};

enum TextureType {
	TEXTURE_1D,
	TEXTURE_2D,
	TEXTURE_3D,
	TEXTURE_CUBE,
	TEXTURE_1D_ARRAY,
	TEXTURE_2D_ARRAY,
	TEXTURE_CUBE_ARRAY,
};

enum TextureFormat {
	TEXTURE_R8_UNORM,
	TEXTURE_R8_SNORM,
	TEXTURE_R16_UNORM,
	TEXTURE_R16_SNORM,
	TEXTURE_R8G8_UNORM,
	TEXTURE_R8G8_SNORM,
	TEXTURE_R16G16_UNORM,
	TEXTURE_R16G16_SNORM,
	TEXTURE_R3G3B2_UNORM,
	TEXTURE_R4G4B4_UNORM,
	TEXTURE_R5G5B5_UNORM,
	TEXTURE_R8G8B8_UNORM,
	TEXTURE_R8G8B8_SNORM,
	TEXTURE_R10G10B10_UNORM,
	TEXTURE_R12G12B12_UNORM,
	TEXTURE_R16G16B16_UNORM,
	TEXTURE_R2G2B2A2_UNORM,
	TEXTURE_R4G4B4A4_UNORM,
	TEXTURE_R5G5B5A1_UNORM,
	TEXTURE_R8G8B8A8_UNORM,
	TEXTURE_R8G8B8A8_SNORM,
	TEXTURE_R10G10B10A2_UNORM,
	TEXTURE_R10G10B10A2_UINT,
	TEXTURE_R12G12B12A12_UNORM,
	TEXTURE_R16G16B16A16_UNORM,
	TEXTURE_R8G8B8_SRGB,
	TEXTURE_R8G8B8A8_SRGB,
	TEXTURE_R16_SFLOAT,
	TEXTURE_R16G16_SFLOAT,
	TEXTURE_R16G16B16_SFLOAT,
	TEXTURE_R16G16B16A16_SFLOAT,
	TEXTURE_R32_SFLOAT,
	TEXTURE_R32G32_SFLOAT,
	TEXTURE_R32G32B32_SFLOAT,
	TEXTURE_R32G32B32A32_SFLOAT,
	TEXTURE_R11G11B10_SFLOAT,
	TEXTURE_R9G9B9E5_UNORM,
	TEXTURE_R8_SINT,
	TEXTURE_R8_UINT,
	TEXTURE_R16_SINT,
	TEXTURE_R16_UINT,
	TEXTURE_R32_SINT,
	TEXTURE_R32_UINT,
	TEXTURE_R8G8_SINT,
	TEXTURE_R8G8_UINT,
	TEXTURE_R16G16_SINT,
	TEXTURE_R16G16_UINT,
	TEXTURE_R32G32_SINT,
	TEXTURE_R32G32_UINT,
	TEXTURE_R8G8B8_SINT,
	TEXTURE_R8G8B8_UINT,
	TEXTURE_R16G16B16_SINT,
	TEXTURE_R16G16B16_UINT,
	TEXTURE_R32G32B32_SINT,
	TEXTURE_R32G32B32_UINT,
	TEXTURE_R8G8B8A8_SINT,
	TEXTURE_R8G8B8A8_UINT,
	TEXTURE_R16G16B16A16_SINT,
	TEXTURE_R16G16B16A16_UINT,
	TEXTURE_R32G32B32A32_SINT,
	TEXTURE_R32G32B32A32_UINT,
	TEXTURE_D16_UNORM,
	TEXTURE_D24_UNORM,
	TEXTURE_D32_SFLOAT,
	TEXTURE_D24_UNORM_S8_UINT,
	TEXTURE_D32_SFLOAT_S8_UINT,
};

enum TextureWrappingMode {
	TEXTURE_REPEAT,
	TEXTURE_MIRRORED_REPEAT,
	TEXTURE_CLAMP_TO_EDGE,
	TEXTURE_CLAMP_TO_BORDER,
};

enum TextureFilter {
	TEXTURE_NEAREST,
	TEXTURE_LINEAR,
	TEXTURE_NEAREST_MIPMAP_NEAREST,
	TEXTURE_LINEAR_MIPMAP_NEAREST,
	TEXTURE_NEAREST_MIPMAP_LINEAR,
	TEXTURE_LINEAR_MIPMAP_LINEAR,
};

}

/* -------------------------------------------------------------------------- */
/* ---- ink/objects/Uniforms.h ---------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

class Uniforms {
public:
	/**
	 * Creates a new Uniforms object.
	 */
	explicit Uniforms() = default;
	
	/**
	 * Returns the number of uniform variables.
	 */
	size_t get_count() const;
	
	/**
	 * Returns the name of unifrom variable at the specified index.
	 *
	 * \param i the index of uniform
	 */
	std::string get_name(int i) const;
	
	/**
	 * Returns the type of unifrom variable at the specified index.
	 *
	 * \param i the index of uniform
	 */
	int get_type(int i) const;
	
	/**
	 * Returns the location of unifrom variable at the specified index.
	 *
	 * \param i the index of uniform
	 */
	int get_location(int i) const;
	
	/**
	 * Returns the data from the uniform data block.
	 */
	float* get_data();
	
	/**
	 * Returns the data from the uniform data block.
	 */
	const float* get_data() const;
	
	/**
	 * Sets the specified value for the specified uniform variable. The value
	 * will be passed to vertex, geometry and fragment shaders.
	 *
	 * \param n variable name
	 * \param v value
	 */
	void set_i(const std::string& n, int v);
	
	/**
	 * Sets the specified value for the specified uniform variable. The value
	 * will be passed to vertex, geometry and fragment shaders.
	 *
	 * \param n variable name
	 * \param v value
	 */
	void set_u(const std::string& n, unsigned int v);
	
	/**
	 * Sets the specified value for the specified uniform variable. The value
	 * will be passed to vertex, geometry and fragment shaders.
	 *
	 * \param n variable name
	 * \param v value
	 */
	void set_f(const std::string& n, float v);
	
	/**
	 * Sets the specified value for the specified uniform variable. The value
	 * will be passed to vertex, geometry and fragment shaders.
	 *
	 * \param n variable name
	 * \param v value
	 */
	void set_v2(const std::string& n, const Vec2& v);
	
	/**
	 * Sets the specified value for the specified uniform variable. The value
	 * will be passed to vertex, geometry and fragment shaders.
	 *
	 * \param n variable name
	 * \param v value
	 */
	void set_v3(const std::string& n, const Vec3& v);
	
	/**
	 * Sets the specified value for the specified uniform variable. The value
	 * will be passed to vertex, geometry and fragment shaders.
	 *
	 * \param n variable name
	 * \param v value
	 */
	void set_v4(const std::string& n, const Vec4& v);
	
	/**
	 * Sets the specified value for the specified uniform variable. The value
	 * will be passed to vertex, geometry and fragment shaders.
	 *
	 * \param n variable name
	 * \param v value
	 */
	void set_m2(const std::string& n, const Mat2& v);
	
	/**
	 * Sets the specified value for the specified uniform variable. The value
	 * will be passed to vertex, geometry and fragment shaders.
	 *
	 * \param n variable name
	 * \param v value
	 */
	void set_m3(const std::string& n, const Mat3& v);
	
	/**
	 * Sets the specified value for the specified uniform variable. The value
	 * will be passed to vertex, geometry and fragment shaders.
	 *
	 * \param n variable name
	 * \param v value
	 */
	void set_m4(const std::string& n, const Mat4& v);
	
	/**
	 * Clears all the data from the uniforms object.
	 */
	void clear();
	
private:
	std::vector<float> data;
	
	std::vector<std::tuple<std::string, int, int> > unifroms;
};

}

/* -------------------------------------------------------------------------- */
/* ---- ink/objects/Defines.h ----------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

class Defines {
public:
	/**
	 * Creates a new Defines object.
	 */
	explicit Defines() = default;
	
	/**
	 * Returns the specific string containing all the define directives.
	 */
	std::string get() const;
	
	/**
	 * Sets the defines object. These values will be defined in vertex, geometry
	 * and fragment shaders.
	 *
	 * \param d defines
	 */
	void set(const Defines& d);
	
	/**
	 * Sets the define directive consisted of a single name. These values will
	 * be defined in vertex, geometry and fragment shaders.
	 *
	 * \param n macro name
	 */
	void set(const std::string& n);
	
	/**
	 * Sets the define directive consisted of a name and a value. These values
	 * will be defined in vertex, geometry and fragment shaders.
	 *
	 * \param n macro name
	 * \param v value
	 */
	void set(const std::string& n, const std::string& v);
	
	/**
	 * Sets the define directive consisted of a name and a value. These values
	 * will be defined in vertex, geometry and fragment shaders.
	 *
	 * \param n macro name
	 * \param v value
	 */
	void set_i(const std::string& n, int v);
	
	/**
	 * Sets the define directive consisted of a name and a value. These values
	 * will be defined in vertex, geometry and fragment shaders.
	 *
	 * \param n macro name
	 * \param v value
	 */
	void set_l(const std::string& n, long v);
	
	/**
	 * Sets the define directive consisted of a name and a value. These values
	 * will be defined in vertex, geometry and fragment shaders.
	 *
	 * \param n macro name
	 * \param v value
	 */
	void set_ll(const std::string& n, long long v);
	
	/**
	 * Sets the define directive consisted of a name if the flag is true. These
	 * values will be defined in vertex, geometry and fragment shaders.
	 *
	 * \param n macro name
	 * \param f flag
	 */
	void set_if(const std::string& n, bool f);
	
	/**
	 * Clears all the data from the defines object.
	 */
	void clear();
	
private:
	std::string defines;
};

}

/* -------------------------------------------------------------------------- */
/* ---- ink/objects/Mesh.h -------------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

struct MeshGroup {
	std::string name;
	int position = 0;
	int length = 0;
};

class Mesh {
public:
	std::string name;                 /**< mesh name */
	
	std::vector<MeshGroup> groups;    /**< mesh groups */
	
	std::vector<Vec3> vertex;         /**< the position for each vertex */
	std::vector<Vec3> normal;         /**< the normal for each vertex */
	std::vector<Vec2> uv;             /**< the UV for each vertex */
	std::vector<Vec4> tangent;        /**< the tangent for each vertex */
	std::vector<Vec3> color;          /**< the color for each vertex */
	
	/**
	 * Creates a new Mesh object and initializes it with a name.
	 *
	 * \param n mesh name
	 */
	explicit Mesh(const std::string& n = "");
	
	/**
	 * Translates the mesh. This operation will modify the mesh data.
	 *
	 * \param x X coordinate
	 * \param y Y coordinate
	 * \param z Z coordinate
	 */
	void translate(float x, float y, float z);
	
	/**
	 * Translates the mesh. This operation will modify the mesh data.
	 *
	 * \param t translate vector
	 */
	void translate(const Vec3& t);
	
	/**
	 * Rotates the mesh about the X axis. This operation will modify the mesh
	 * data.
	 *
	 * \param a angle
	 */
	void rotate_x(float a);
	
	/**
	 * Rotates the mesh about the Y axis. This operation will modify the mesh
	 * data.
	 *
	 * \param a angle
	 */
	void rotate_y(float a);
	
	/**
	 * Rotates the mesh about the Z axis. This operation will modify the mesh
	 * data.
	 *
	 * \param a angle
	 */
	void rotate_z(float a);
	
	/**
	 * Rotates the mesh with Euler angle. This operation will modify the mesh
	 * data.
	 *
	 * \param e Euler angle
	 */
	void rotate(const Euler& e);
	
	/**
	 * Scales the mesh. This operation will modify the mesh data.
	 *
	 * \param x X coordinate
	 * \param y Y coordinate
	 * \param z Z coordinate
	 */
	void scale(float x, float y, float z);
	
	/**
	 * Scales the mesh. This operation will modify the mesh data.
	 *
	 * \param s scale vector
	 */
	void scale(const Vec3& s);
	
	/**
	 * Normalizes all the normal vectors in the mesh.
	 */
	void normalize();
	
	/**
	 * Calculates normals and adds them to the mesh. Normals are calculated by
	 * the vertex infomation.
	 */
	void create_normals();
	
	/**
	 * Calculates tangents and adds them to the mesh. Tangents are calculated by
	 * the vertex, normal and UV infomation.
	 */
	void create_tangents();
};

}

/* -------------------------------------------------------------------------- */
/* ---- ink/objects/Image.h ------------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

class Image {
public:
	int width = 0;                /**< the width of image */
	int height = 0;               /**< the height of image */
	int channel = 0;              /**< the channel of image */
	int bytes = 1;                /**< the bytes of image */
	
	std::vector<uint8_t> data;    /**< the data of image */
	
	/**
	 * Creates a new Image object.
	 */
	explicit Image() = default;
	
	/**
	 * Creates a new Image object and initializes it with the size, channel and
	 * bytes.
	 *
	 * \param w the width of image
	 * \param h the height of image
	 * \param c the channel of image
	 * \param b the bytes of image
	 */
	explicit Image(int w, int h, int c, int b = 1);
	
	/**
	 * Returns a sub-image sliced of the current image. The new image region is
	 * set from (x1, y1) to (x2, y2).
	 *
	 * \param x1 the x-coordinate of left upper corner
	 * \param y1 the y-coordinate of left upper corner
	 * \param x2 the x-coordinate of right lower corner
	 * \param y2 the y-coordinate of right lower corner
	 */
	Image subimage(int x1, int y1, int x2, int y2);
	
	/**
	 * Flips the image vertically.
	 */
	void flip_vertical();
	
	/**
	 * Flips the image horizontally. Notes that this function is slower than
	 * flip_vertical.
	 */
	void flip_horizontal();
	
	/**
	 * Returns a vector of the images split by channel.
	 */
	std::vector<Image> split() const;
	
	/**
	 * Converts colors in the image from one color space to another color space.
	 *
	 * \param c color conversion
	 */
	void convert(ColorConversion c);
	
	/**
	 * Converts colors in the image from RGB color space to BGR color space.
	 */
	template <typename Type>
	void convert_rgb_to_bgr();
	
	/**
	 * Converts colors in the image from BGR color space to RGB color space.
	 */
	template <typename Type>
	void convert_bgr_to_rgb();
	
	/**
	 * Converts colors in the image from RGB color space to SRGB color space.
	 */
	template <typename Type>
	void convert_rgb_to_srgb();
	
	/**
	 * Converts colors in the image from SRGB color space to RGB color space.
	 */
	template <typename Type>
	void convert_srgb_to_rgb();
	
	/**
	 * Converts colors in the image from RGB color space to XYZ color space.
	 */
	template <typename Type>
	void convert_rgb_to_xyz();
	
	/**
	 * Converts colors in the image from XYZ color space to RGB color space.
	 */
	template <typename Type>
	void convert_xyz_to_rgb();
	
	/**
	 * Converts colors in the image from RGB color space to HSV color space.
	 */
	template <typename Type>
	void convert_rgb_to_hsv();
	
	/**
	 * Converts colors in the image from HSV color space to RGB color space.
	 */
	template <typename Type>
	void convert_hsv_to_rgb();
	
	/**
	 * Converts colors in the image from RGB color space to HSL color space.
	 */
	template <typename Type>
	void convert_rgb_to_hsl();
	
	/**
	 * Converts colors in the image from HSL color space to RGB color space.
	 */
	template <typename Type>
	void convert_hsl_to_rgb();
	
	/**
	 * Converts colors in the image from RGB color space to HCY color space.
	 */
	template <typename Type>
	void convert_rgb_to_hcy();
	
	/**
	 * Converts colors in the image from HCY color space to RGB color space.
	 */
	template <typename Type>
	void convert_hcy_to_rgb();
	
private:
	template <typename Type>
	static inline float unpack(Type v);
	
	template <typename Type>
	static inline Type pack(float v);
	
	static inline float saturate(float v);
};

}

/* -------------------------------------------------------------------------- */
/* ---- ink/objects/Material.h ---------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

class Material {
public:
	std::string name;                     /**< material name */
	
	RenderSide side;                      /**< which side of faces will be rendered */
	
	RenderSide shadow_side;               /**< which side of faces will cast shadows */
	
	bool visible = true;                  /**< whether the material will be rendered */
	
	bool wireframe = false;               /**< whether to render mesh as wireframe */
	
	bool depth_test = true;               /**< whether to enable depth test in rendering */
	
	ComparisonFunc depth_func;            /**< which depth comparison function to use */
	
	bool stencil_test = false;            /**< whether to enable stencil test in rendering */
	
	int stencil_writemask = 0xFF;         /**< the mask when writing to stencil buffer */
	
	int stencil_ref = 0;                  /**< the reference value to be used in stencil comparison */
	
	int stencil_mask = 0xFF;              /**< the mask to be used in stencil comparison */
	
	ComparisonFunc stencil_func;          /**< which stencil comparison function to use */
	
	StencilOperation stencil_fail;        /**< the operation when the stencil test fails */
	
	StencilOperation stencil_zfail;       /**< the operation when the stencil test passes but depth test fails */
	
	StencilOperation stencil_zpass;       /**< the operation when both the stencil test and depth test pass */
	
	bool blending = false;                /**< whether to enable blending in rendering */
	
	BlendOperation blend_op_rgb;          /**< which RGB blend operation to use in blending */
	
	BlendOperation blend_op_alpha;        /**< which alpha blend operation to use in blending */
	
	BlendFactor blend_src_rgb;            /**< the RGB source blend factor in blending */
	
	BlendFactor blend_src_alpha;          /**< the alpha source blend factor in blending */
	
	BlendFactor blend_dst_rgb;            /**< the RGB destination blend factor in blending */
	
	BlendFactor blend_dst_alpha;          /**< the alpha destination blend factor in blending */
	
	float alpha_test = 0;                 /**< threshold of alpha test, pixels with lower alpha will be discarded */
	
	bool use_map_with_alpha = true;       /**< whether to use alpha channel from color map */
	
	bool use_vertex_color = false;        /**< whether to use vertex colors from mesh */
	
	bool use_tangent_space = true;        /**< whether the normal map defines in tangent space */
	
	float normal_scale = 1;               /**< how much the normal map affects the material */
	
	float displacement_scale = 1;         /**< how much the displacement map affects the mesh */
	
	Vec3 color = {1, 1, 1};               /**< the base color of material, default is white */
	
	float alpha = 1;                      /**< the opacity of material, range is 0 to 1 */
	
	float specular = 0.5;                 /**< how specular the material appears, range is 0 to 1 */
	
	float metalness = 0;                  /**< how metallic the material appears, range is 0 to 1 */
	
	float roughness = 1;                  /**< how rough the material appears, range is 0 to 1 */
	
	Vec3 emissive = {0, 0, 0};            /**< the emissive color of material, default is black */
	
	float emissive_intensity = 1;         /**< the emissive intensity of material, range is 0 to 1 */
	
	float ao_intensity = 1;               /**< the occlusion intensity of material, range is 0 to 1 */
	
	Image* normal_map = nullptr;          /**< the map determines the normals of mesh */
	
	Image* displacement_map = nullptr;    /**< the map determines the offsets of mesh's vertices */
	
	Image* color_map = nullptr;           /**< the map affects the base color of material */
	
	Image* alpha_map = nullptr;           /**< the map affects the alpha of material */
	
	Image* roughness_map = nullptr;       /**< the map affects how rough the material appears */
	
	Image* metalness_map = nullptr;       /**< the map affects how metallic the material appears */
	
	Image* specular_map = nullptr;        /**< the map affects how specular the material appears */
	
	Image* emissive_map = nullptr;        /**< the map affects the emissive color of material */
	
	Image* ao_map = nullptr;              /**< the map affects the ambient occlusion of material */
	
	Image* custom_maps[16];               /**< the custom maps of material */
	
	void* shader = nullptr;               /**< custom shader determines how lights affect material */
	
	void* reflection_probe = nullptr;     /**< reflection probe affects the environment light of material */
	
	Uniforms* uniforms = nullptr;         /**< the custom uniforms of material */
	
	/**
	 * Creates a new Material object and initializes it with a name.
	 *
	 * \param n material name
	 */
	explicit Material(const std::string& n = "");
};

}

/* -------------------------------------------------------------------------- */
/* ---- ink/objects/Instance.h ---------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

class Instance {
public:
	std::string name;              /**< instance name */
	
	bool visible = true;           /**< whether the instance will be rendered */
	
	bool cast_shadow = true;       /**< whether the instance will cast shadows */
	
	int priority = 0;              /**< the sorting priority in rendering */
	
	Vec3 position = {0, 0, 0};     /**< the position vector of instance */
	
	Vec3 scale = {1, 1, 1};        /**< the scaling vector of instance */
	
	Euler rotation;                /**< the rotation angles of instance */
	
	Mat4 matrix_local;             /**< the local transform matrix of instance */
	
	Mat4 matrix_global;            /**< the global transform matrix of instance */
	
	Mesh* mesh = nullptr;          /**< the linked mesh of instance */
	
	/**
	 * Creates a new Instance object, which is the minimum unit of rendering.
	 *
	 * \param n instance name
	 */
	explicit Instance(const std::string& n = "");
	
	/**
	 * Adds the specified instance as the child of the instance. The index
	 * starts from zero.
	 *
	 * \param i instance
	 */
	void add(Instance* i);
	
	/**
	 * Adds the specified instances as the children of the instance. The index
	 * starts from zero.
	 *
	 * \param l instance list
	 */
	void add(const std::initializer_list<Instance*>& l);
	
	/**
	 * Removes the specified instance as the child of the instance.
	 *
	 * \param i instance
	 */
	void remove(Instance* i);
	
	/**
	 * Removes the specified instances as the children of the instance.
	 *
	 * \param l instance list
	 */
	void remove(const std::initializer_list<Instance*>& l);
	
	/**
	 * Removes all the children from the instance.
	 */
	void clear();
	
	/**
	 * Returns the child at the specified index in the instance.
	 *
	 * \param i the index of child
	 */
	Instance* get_child(int i) const;
	
	/**
	 * Returns the first child matching the specified name in the instance.
	 *
	 * \param n the name of child
	 */
	Instance* get_child(const std::string& n) const;
	
	/**
	 * Returns the number of its children in the instance.
	 */
	size_t get_child_count() const;
	
	/**
	 * Returns the parent of the instance.
	 */
	Instance* get_parent() const;
	
	/**
	 * Sets the transform vectors (position, rotation and scaling) of the
	 * instance.
	 *
	 * \param p position vector
	 * \param r rotation angles
	 * \param s scaling vector
	 */
	void set_transform(const Vec3& p, const Euler& r, const Vec3& s);
	
	/**
	 * Updates the local transform matrix. This function is equivalent to
	 * "matrix_local = transform();".
	 */
	void update_matrix_local();
	
	/**
	 * Updates the global transform matrix. This function is equivalent to
	 * "matrix_global = transform_global();".
	 */
	void update_matrix_global();
	
	/**
	 * Converts the specified vector from global space to local space. This
	 * function only works when matrix_global is correct.
	 *
	 * \param v vector
	 */
	Vec3 global_to_local(const Vec3& v) const;
	
	/**
	 * Converts the specified vector from local space to global space. This
	 * function only works when matrix_global is correct.
	 *
	 * \param v vector
	 */
	Vec3 local_to_global(const Vec3& v) const;
	
	/**
	 * Returns a local transform matrix by calculating the position, rotation
	 * and scale of instance.
	 */
	Mat4 transform() const;
	
	/**
	 * Returns a global transform matrix by multiplying its ancestors' transform
	 * matrices recursively.
	 */
	Mat4 transform_global() const;
	
	/**
	 * Returns a transform matrix by calculating position, rotation and scale.
	 *
	 * \param p position vector
	 * \param r rotation angles
	 * \param s scale vector
	 */
	static Mat4 transform(const Vec3& p, const Euler& r, const Vec3& s);
	
protected:
	Instance* parent = nullptr;
	
	std::vector<Instance*> children;
};

}

/* -------------------------------------------------------------------------- */
/* ---- ink/loader/Loader.h ------------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

struct LoadObject {
	std::vector<Mesh> meshes;
	std::vector<Material> materials;
};

struct ObjOptions {
	bool vertex_color = false;
	std::string group = "g";
};

class Loader {
public:
	/**
	 * Loads the image data from the specified file into a image.
	 *
	 * \param p the path to the file
	 */
	static Image load_image(const std::string& p);
	
	/**
	 * Loads the image data from the specified file into a HDR image.
	 *
	 * \param p the path to the file
	 */
	static Image load_image_hdr(const std::string& p);
	
	/**
	 * Loads the material data from the specified MTL file into material vector.
	 *
	 * \param p the path to the file
	 */
	static LoadObject load_mtl(const std::string& p);
	
	/**
	 * Loads the mesh data from the specified OBJ file into mesh vector. The
	 * meshes are divided by the custom grouping keyword.
	 *
	 * \param p the path to the file
	 * \param o options when loading OBJ file
	 */
	static LoadObject load_obj(const std::string& p, const ObjOptions& o = {});
};

}

/* -------------------------------------------------------------------------- */
/* ---- ink/camera/Camera.h ------------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

class Camera {
public:
	float near = 0;     /**< distance to the nearer clipping plane */
	float far = 0;      /**< distance to the farther clipping plane */
	
	Vec3 position;      /**< the position of camera */
	Vec3 direction;     /**< the viewing direction of camera */
	Vec3 up;            /**< the view-up vector of camera */
	
	Mat4 viewing;       /**< the matrix of viewing transform */
	Mat4 projection;    /**< the matrix of projection transform */
	
	/**
	 * Creates a new Camera object.
	 */
	explicit Camera() = default;
	
	/**
	 * Returns true if this camera is perspective.
	 */
	bool is_perspective() const;
	
	/**
	 * Sets the position, viewing direction, view-up vector of camera. Updates
	 * the viewing matrix and projection matrix.
	 *
	 * \param p the position of camera
	 * \param d viewing direction (from object to camera)
	 * \param u view-up vector
	 */
	void lookat(const Vec3& p, const Vec3& d, const Vec3& u);
};

}

/* -------------------------------------------------------------------------- */
/* ---- ink/camera/PerspCamera.h -------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

class PerspCamera : public Camera {
public:
	float fov_y = 0;     /**< the field of view in y-axis */
	float aspect = 0;    /**< the aspect ratio of field of view */
	
	/**
	 * Creates a new PerspCamera object.
	 */
	explicit PerspCamera() = default;
	
	/**
	 * Creates a new PerspCamera and initializes it with FOV, aspect, near and
	 * far.
	 *
	 * \param fov the field of view in y-axis
	 * \param a the aspect ratio of field of view
	 * \param n distance to the nearer clipping plane
	 * \param f distance to the farther clipping plane
	 */
	explicit PerspCamera(float fov, float a, float n, float f);
	
	/**
	 * Sets the FOV, aspect, near and far. Updates the projection matrix of
	 * camera.
	 *
	 * \param fov the field of view in y-axis
	 * \param a the aspect ratio of field of view
	 * \param n distance to the nearer clipping plane
	 * \param f distance to the farther clipping plane
	 */
	void set(float fov, float a, float n, float f);
};

}

/* -------------------------------------------------------------------------- */
/* ---- ink/camera/OrthoCamera.h -------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

class OrthoCamera : public Camera {
public:
	float left = 0;     /**< the left vertical clipping plane */
	float right = 0;    /**< the right vertical clipping plane */
	float lower = 0;    /**< the lower horizontal clipping plane */
	float upper = 0;    /**< the upper horizontal clipping plane */
	
	/**
	 * Creates a new OrthoCamera object.
	 */
	explicit OrthoCamera() = default;
	
	/**
	 * Creates a new OrthoCamera and initializes it with left, right, lower,
	 * upper, near, far.
	 *
	 * \param lv the left vertical clipping plane
	 * \param rv the right vertical clipping plane
	 * \param lh the lower horizontal clipping plane
	 * \param uh the upper horizontal clipping plane
	 * \param n distance to the nearer clipping plane
	 * \param f distance to the farther clipping plane
	 */
	explicit OrthoCamera(float lv, float rv, float lh, float uh, float n, float f);
	
	/**
	 * Sets the left, right, lower, upper, near, far. Updates the projection
	 * matrix of camera.
	 *
	 * \param lv the left vertical clipping plane
	 * \param rv the right vertical clipping plane
	 * \param lh the lower horizontal clipping plane
	 * \param uh the upper horizontal clipping plane
	 * \param n distance to the nearer clipping plane
	 * \param f distance to the farther clipping plane
	 */
	void set(float lv, float rv, float lh, float uh, float n, float f);
};

}

/* -------------------------------------------------------------------------- */
/* ---- ink/meshes/BoxMesh.h ------------------------------------------------ */
/* -------------------------------------------------------------------------- */

namespace Ink {

class BoxMesh {
public:
	/**
	 * Returns a box shaped mesh with 1 unit width, 1 unit height and 1 unit
	 * depth. The box is centred on the origin.
	 */
	static Mesh create();
	
private:
	static std::vector<Vec3> vertex;
	static std::vector<Vec2> uv;
	static std::vector<Vec3> normal;
};

}

/* -------------------------------------------------------------------------- */
/* ---- ink/meshes/PlaneMesh.h ---------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

class PlaneMesh {
public:
	/**
	 * Returns a plane shaped mesh with 1 unit width and 1 unit height. The
	 * plane is centred on the origin.
	 */
	static Mesh create();
	
private:
	static std::vector<Vec3> vertex;
	static std::vector<Vec2> uv;
	static std::vector<Vec3> normal;
};

}

/* -------------------------------------------------------------------------- */
/* ---- ink/meshes/CylinderMesh.h ------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

class CylinderMesh {
public:
	/**
	 * Returns a cylinder shaped mesh with 1 unit height and 0.5 unit radius.
	 * The cylinder is centred on the origin.
	 */
	static Mesh create();
	
private:
	static std::vector<Vec3> vertex;
	static std::vector<Vec2> uv;
	static std::vector<Vec3> normal;
};

}

/* -------------------------------------------------------------------------- */
/* ---- ink/meshes/SphereMesh.h --------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

class SphereMesh {
public:
	/**
	 * Returns a sphere shaped mesh with 0.5 unit radius. The sphere is centred
	 * on the origin.
	 */
	static Mesh create();
	
private:
	static std::vector<Vec3> vertex;
	static std::vector<Vec2> uv;
	static std::vector<Vec3> normal;
};

}

/* -------------------------------------------------------------------------- */
/* ---- ink/graphics/Gpu.h -------------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink::Gpu {

class Rect {
public:
	int x = 0;         /**< the x-coordinate of left lower corner */
	int y = 0;         /**< the y-coordinate of left lower corner */
	int width = 0;     /**< the width of the rectangle */
	int height = 0;    /**< the height of the rectangle */
	
	/**
	 * Creates a new Rect object.
	 */
	explicit Rect() = default;
	
	/**
	 * Creates a new Rect object and initializes it with size.
	 *
	 * \param w the width of the rectangle
	 * \param h the height of the rectangle
	 */
	explicit Rect(int w, int h);
	
	/**
	 * Creates a new Rect object and initializes it with size and position.
	 *
	 * \param x the x-coordinate of left lower corner
	 * \param y the y-coordinate of left lower corner
	 * \param w the width of the rectangle
	 * \param h the height of the rectangle
	 */
	explicit Rect(int x, int y, int w, int h);
};

class State {
public:
	/**
	 * Returns a string describing the current device.
	 */
	static std::string get_device_info();
	
	/**
	 * Blocks until all GPU commands is complete.
	 */
	static void finish();
	
	/**
	 * Forces execution of GPU commands in finite time.
	 */
	static void flush();
	
	/**
	 * Prints all error information from OpenGL.
	 */
	static std::string get_error();
	
	/**
	 * Clears the color, depth or stencil drawing buffers. Initializes the color
	 * buffer to the current clear color value.
	 *
	 * \param c whether to clear the color buffer
	 * \param d whether to clear the depth buffer
	 * \param s whether to clear the stencil buffer
	 */
	static void clear(bool c = true, bool d = true, bool s = true);
	
	/**
	 * Returns the clear color (.xyz) and opacity (.w).
	 */
	static Vec4 get_clear_color();
	
	/**
	 * Sets the specified clear color and opacity. Default is (1, 1, 1) and 1.
	 *
	 * \param c clear color
	 * \param a clear opacity
	 */
	static void set_clear_color(const Vec3& c, float a);
	
	/**
	 * Sets the specified clear color (.xyz) and opacity (.w). Default is (1, 1,
	 * 1, 1).
	 *
	 * \param c clear color and opacity
	 */
	static void set_clear_color(const Vec4& c);
	
	/**
	 * Returns a vec4 value represents the red, green, blue and alpha components
	 * are enabled for writing.
	 */
	static Vec4 get_color_writemask();
	
	/**
	 * Determines whether the red, green, blue and alpha components are enabled
	 * for writing. Default is true, true, true, true.
	 *
	 * \param r red writemask
	 * \param g green writemask
	 * \param b blur writemask
	 * \param a alpha writemask
	 */
	static void set_color_writemask(bool r, bool g, bool b, bool a);
	
	/**
	 * Enables depth test. If enabled, do depth comparisons and update the depth
	 * buffer.
	 */
	static void enable_depth_test();
	
	/**
	 * Disables depth test. If enabled, do depth comparisons and update the
	 * depth buffer.
	 */
	static void disable_depth_test();
	
	/**
	 * Returns the clear value for depth buffer.
	 */
	static double get_clear_depth();
	
	/**
	 * Sets the specified clear value for depth buffer. Default is 1.
	 *
	 * \param d clear depth
	 */
	static void set_clear_depth(double d);
	
	/**
	 * Returns true if the depth buffer is enabled for writing in depth test.
	 */
	static bool get_depth_writemask();
	
	/**
	 * Determines whether the depth buffer is enabled for writing in depth test.
	 * Default is true.
	 *
	 * \param m depth writemask
	 */
	static void set_depth_writemask(bool m);
	
	/**
	 * Returns the comparison function in depth test.
	 */
	static ComparisonFunc get_depth_func();
	
	/**
	 * Sets the specified comparison function in depth test. Default is
	 * FUNC_LESS.
	 *
	 * \param f comparison function
	 */
	static void set_depth_func(ComparisonFunc f);
	
	/**
	 * Enables stencil test. If enabled, do stencil testing and update the
	 * stencil buffer.
	 */
	static void enable_stencil_test();
	
	/**
	 * Disables stencil test. If enabled, do stencil testing and update the
	 * stencil buffer.
	 */
	static void disable_stencil_test();
	
	/**
	 * Returns the clear value for stencil buffer.
	 */
	static int get_clear_stencil();
	
	/**
	 * Sets the specified clear value for stencil buffer. Default is 0.
	 *
	 * \param s clear stencil
	 */
	static void set_clear_stencil(int s);
	
	/**
	 * Returns true if the stencil buffer bits are enabled for writing in
	 * stencil test.
	 */
	static unsigned int get_stencil_writemask();
	
	/**
	 * Determines whether the stencil buffer bits are enabled for writing in
	 * stencil test. Default is all 1's.
	 *
	 * \param m stencil writemask
	 */
	static void set_stencil_writemask(unsigned int m);
	
	/**
	 * Returns the comparison function in stencil test.
	 */
	static ComparisonFunc get_stencil_func();
	
	/**
	 * Returns the reference value in stencil test.
	 */
	static int get_stencil_ref();
	
	/**
	 * Returns the mask in stencil test.
	 */
	static int get_stencil_mask();
	
	/**
	 * Sets the specified comparison function, reference value and mask in
	 * stencil test. Default is FUNC_ALWAYS, 0 and 1.
	 *
	 * \param f comparison function
	 * \param r reference value
	 * \param m mask
	 */
	static void set_stencil_func(ComparisonFunc f, int r, int m);
	
	/**
	 * Returns the operation when the stencil test fails.
	 */
	static StencilOperation get_stencil_fail();
	
	/**
	 * Returns the operation when the stencil test passes but depth test fails.
	 */
	static StencilOperation get_stencil_zfail();
	
	/**
	 * Returns the operation when both the stencil test and depth test pass.
	 */
	static StencilOperation get_stencil_zpass();
	
	/**
	 * Sets the specified operations for different cases in stencil test and
	 * depth test. Default is STENCIL_KEEP, STENCIL_KEEP, STENCIL_KEEP.
	 *
	 * \param f the operation when the stencil test fails
	 * \param zf the operation when the stencil test passes but depth test fails
	 * \param zp the operation when both the stencil test and depth test pass
	 */
	static void set_stencil_op(StencilOperation f,
							   StencilOperation zf,
							   StencilOperation zp);
	
	/**
	 * Enables blending. If enabled, blend the computed fragment color values
	 * with the values in the color buffers.
	 */
	static void enable_blending();
	
	/**
	 * Disables blending. If enabled, blend the computed fragment color values
	 * with the values in the color buffers.
	 */
	static void disable_blending();
	
	/**
	 * Returns the blend operation of RGB color components in blending.
	 */
	static BlendOperation get_blend_op_rgb();
	
	/**
	 * Returns the blend operation of alpha color component in blending.
	 */
	static BlendOperation get_blend_op_alpha();
	
	/**
	 * Sets the specified blend operation in blending. Default is BLEND_ADD.
	 *
	 * \param o blend operation
	 */
	static void set_blend_op(BlendOperation o);
	
	/**
	 * Sets the specified blend operation in blending. Default is BLEND_ADD,
	 * BLEND_ADD.
	 *
	 * \param rgb RGB blend operation
	 * \param a alpha blend operation
	 */
	static void set_blend_op(BlendOperation rgb, BlendOperation a);
	
	/**
	 * Returns the source blend factor of RGB color components in blending.
	 */
	static BlendFactor get_blend_src_rgb();
	
	/**
	 * Returns the source blend factor of alpha color component in blending.
	 */
	static BlendFactor get_blend_src_alpha();
	
	/**
	 * Returns the source destination factor of RGB color components in
	 * blending.
	 */
	static BlendFactor get_blend_dst_rgb();
	
	/**
	 * Returns the source destination factor of alpha color component in
	 * blending.
	 */
	static BlendFactor get_blend_dst_alpha();
	
	/**
	 * Sets the specified source and destination blend factors in blending.
	 * Default is FACTOR_ONE and FACTOR_ZERO.
	 *
	 * \param s source blend factor
	 * \param d destination blend factor
	 */
	static void set_blend_factor(BlendFactor s, BlendFactor d);
	
	/**
	 * Sets the specified source and destination blend factors in blending.
	 * Default is FACTOR_ONE, FACTOR_ZERO, FACTOR_ONE, FACTOR_ZERO.
	 *
	 * \param sr RGB source blend factor
	 * \param dr RGB destination blend factor
	 * \param sa alpha source blend factor
	 * \param da alpha destination blend factor
	 */
	static void set_blend_factor(BlendFactor sr, BlendFactor dr,
								 BlendFactor sa, BlendFactor da);
	
	/**
	 * Returns the viewport region.
	 */
	static Rect get_viewport();
	
	/**
	 * Sets the viewport region to render from (x, y) to (x + width, y +
	 * height).
	 *
	 * \param v viewport region
	 */
	static void set_viewport(const Rect& v);
	
	/**
	 * Enables scissor test. If enabled, discard fragments that are outside the
	 * scissor rectangle.
	 */
	static void enable_scissor_test();
	
	/**
	 * Disables scissor test. If enabled, discard fragments that are outside the
	 * scissor rectangle.
	 */
	static void disable_scissor_test();
	
	/**
	 * Returns the scissor region in scissor test.
	 */
	static Rect get_scissor();
	
	/**
	 * Sets the scissor region from (x, y) to (x + width, y + height) in scissor
	 * test.
	 *
	 * \param s scissor region
	 */
	static void set_scissor(const Rect& s);
	
	/**
	 * Enables wireframe. If enabled, boundary edges of the polygon are drawn as
	 * line segments.
	 */
	static void enable_wireframe();
	
	/**
	 * Disables wireframe. If enabled, boundary edges of the polygon are drawn
	 * as line segments.
	 */
	static void disable_wireframe();
	
	/**
	 * Enables face culling. If enabled, cull polygons based on their winding in
	 * window coordinates.
	 */
	static void enable_culling();
	
	/**
	 * Disables face culling. If enabled, cull polygons based on their winding
	 * in window coordinates.
	 */
	static void disable_culling();
	
	/**
	 * Determines which side of face will be culled.
	 */
	static RenderSide get_cull_side();
	
	/**
	 * Determines which side of face will be culled. Default is BACK_SIDE.
	 *
	 * \param s culling side
	 */
	static void set_cull_side(RenderSide s);
	
	/**
	 * Enables polygon offset. If enabled, and if the wireframe is disabled, an
	 * offset is added to depth values of a polygon's fragments before the depth
	 * comparison is performed.
	 */
	static void enable_polygon_offset();
	
	/**
	 * Disables polygon offset. If enabled, and if the wireframe is disabled, an
	 * offset is added to depth values of a polygon's fragments before the depth
	 * comparison is performed.
	 */
	static void disable_polygon_offset();
	
	/**
	 * Returns the polygon offset factor.
	 */
	static float get_polygon_offset_factor();
	
	/**
	 * Returns the polygon offset units.
	 */
	static float get_polygon_offset_units();
	
	/**
	 * Sets the specified polygon offset factor and units. Default is 0 and 0.
	 *
	 * \param f polygon offset factor
	 * \param u polygon offset units
	 */
	static void set_polygon_offset(float f, float u);
	
	/**
	 * Enables dithering. If enabled, dither color components or indices before
	 * they are written to the color buffer.
	 */
	static void enable_dithering();
	
	/**
	 * Disables dithering. If enabled, dither color components or indices before
	 * they are written to the color buffer.
	 */
	static void disable_dithering();
	
	/**
	 * Enables multisample anti-aliasing. If enabled, use multiple fragment
	 * samples in computing the final color of a pixel.
	 */
	static void enable_multisample();
	
	/**
	 * Disables multisample anti-aliasing. If enabled, use multiple fragment
	 * samples in computing the final color of a pixel.
	 */
	static void disable_multisample();
	
	/**
	 * Enables alpha to coverage. If enabled, compute a temporary coverage value
	 * where each bit is determined by the alpha value at the corresponding
	 * sample location.
	 */
	static void enable_alpha_to_coverage();
	
	/**
	 * Disables alpha to coverage. If enabled, compute a temporary coverage
	 * value where each bit is determined by the alpha value at the
	 * corresponding sample location.
	 */
	static void disable_alpha_to_coverage();
	
	/**
	 * Enables seamless cube texture accesses. If enabled, cubemap textures are
	 * sampled such that when linearly sampling from the border between two
	 * adjacent faces, texels from both faces are used to generate the final
	 * sample value.
	 */
	static void enable_texture_cube_seamless();
	
	/**
	 * Disables seamless cube texture accesses. If enabled, cubemap textures are
	 * sampled such that when linearly sampling from the border between two
	 * adjacent faces, texels from both faces are used to generate the final
	 * sample value.
	 */
	static void disable_texture_cube_seamless();
};

class MaterialState {
public:
	/**
	 * Applies the depth dependent settings for the specified material.
	 *
	 * \param m material
	 */
	static void set_depth(const Material& m);
	
	/**
	 * Applies the stencil dependent settings for the specified material.
	 *
	 * \param m material
	 */
	static void set_stencil(const Material& m);
	
	/**
	 * Applies the blending dependent settings for the specified material.
	 *
	 * \param m material
	 */
	static void set_blending(const Material& m);
	
	/**
	 * Applies the wireframe dependent settings for the specified material.
	 *
	 * \param m material
	 */
	static void set_wireframe(const Material& m);
	
	/**
	 * Applies the side dependent settings for the specified material.
	 *
	 * \param m material
	 */
	static void set_side(const Material& m);
	
	/**
	 * Applies the shadow side dependent settings for the specified material.
	 *
	 * \param m material
	 */
	static void set_shadow_side(const Material& m);
};

class Shader {
public:
	/**
	 * Creates a new Shader object.
	 */
	explicit Shader();
	
	/**
	 * Deletes this Shader object.
	 */
	~Shader();
	
	/**
	 * The copy constructor is deleted.
	 */
	Shader(const Shader&) = delete;
	
	/**
	 * The copy assignment operator is deleted.
	 */
	Shader& operator=(const Shader&) = delete;
	
	/**
	 * Loads the specified string of vertex shader content to the shader.
	 *
	 * \param s vertex shader
	 */
	void load_vert(const char* s);
	
	/**
	 * Loads the specified string of vertex shader content to the shader.
	 *
	 * \param s vertex shader
	 */
	void load_vert(const std::string& s);
	
	/**
	 * Loads the specified string of geometry shader content to the shader.
	 *
	 * \param s geometry shader
	 */
	void load_geom(const char* s);
	
	/**
	 * Loads the specified string of geometry shader content to the shader.
	 *
	 * \param s geometry shader
	 */
	void load_geom(const std::string& s);
	
	/**
	 * Loads the specified string of fragment shader content to the shader.
	 *
	 * \param s fragment shader
	 */
	void load_frag(const char* s);
	
	/**
	 * Loads the specified string of fragment shader content to the shader.
	 *
	 * \param s fragment shader
	 */
	void load_frag(const std::string& s);
	
	/**
	 * Loads the vertex shader from the specified GLSL file to the shader.
	 *
	 * \param p the path to vertex shader file
	 */
	void load_vert_file(const std::string& p);
	
	/**
	 * Loads the geometry shader from the specified GLSL file to the shader.
	 *
	 * \param p the path to geometry shader file
	 */
	void load_geom_file(const std::string& p);
	
	/**
	 * Loads the fragment shader from the specified GLSL file to the shader.
	 *
	 * \param p the path to fragment shader file
	 */
	void load_frag_file(const std::string& p);
	
	/**
	 * Compile the program of shader if the shader has changed.
	 */
	void compile() const;
	
	/**
	 * Use the program of the compiled shader.
	 */
	void use_program() const;
	
	/**
	 * Sets the define directives to the shader. These values will be defined in
	 * vertex, geometry and fragment shaders.
	 *
	 * \param d defines
	 */
	void set_defines(const Defines& d);
	
	/**
	 * Sets the specified value for the specified uniform variable. These values
	 * will be passed to vertex, geometry and fragment shaders.
	 *
	 * \param n variable name
	 * \param v value
	 */
	void set_uniform_i(const std::string& n, int v) const;
	
	/**
	 * Sets the specified value for the specified uniform variable. These values
	 * will be passed to vertex, geometry and fragment shaders.
	 *
	 * \param n variable name
	 * \param v value
	 */
	void set_uniform_u(const std::string& n, unsigned int v) const;
	
	/**
	 * Sets the specified value for the specified uniform variable. These values
	 * will be passed to vertex, geometry and fragment shaders.
	 *
	 * \param n variable name
	 * \param v value
	 */
	void set_uniform_f(const std::string& n, float v) const;
	
	/**
	 * Sets the specified value for the specified uniform variable. These values
	 * will be passed to vertex, geometry and fragment shaders.
	 *
	 * \param n variable name
	 * \param v value
	 */
	void set_uniform_v2(const std::string& n, const Vec2& v) const;
	
	/**
	 * Sets the specified value for the specified uniform variable. These values
	 * will be passed to vertex, geometry and fragment shaders.
	 *
	 * \param n variable name
	 * \param v value
	 */
	void set_uniform_v3(const std::string& n, const Vec3& v) const;
	
	/**
	 * Sets the specified value for the specified uniform variable. These values
	 * will be passed to vertex, geometry and fragment shaders.
	 *
	 * \param n variable name
	 * \param v value
	 */
	void set_uniform_v4(const std::string& n, const Vec4& v) const;
	
	/**
	 * Sets the specified value for the specified uniform variable. These values
	 * will be passed to vertex, geometry and fragment shaders.
	 *
	 * \param n variable name
	 * \param v value
	 */
	void set_uniform_m2(const std::string& n, const Mat2& v) const;
	
	/**
	 * Sets the specified value for the specified uniform variable. These values
	 * will be passed to vertex, geometry and fragment shaders.
	 *
	 * \param n variable name
	 * \param v value
	 */
	void set_uniform_m3(const std::string& n, const Mat3& v) const;
	
	/**
	 * Sets the specified value for the specified uniform variable. These values
	 * will be passed to vertex, geometry and fragment shaders.
	 *
	 * \param n variable name
	 * \param v value
	 */
	void set_uniform_m4(const std::string& n, const Mat4& v) const;
	
	/**
	 * Sets the specified uniforms object to the shader. These values will be
	 * passed to vertex, geometry and fragment shaders.
	 *
	 * \param u uniforms object
	 */
	void set_uniforms(const Uniforms& u) const;
	
	/**
	 * Returns the GLSL version of the shading language.
	 */
	static std::string get_glsl_version();
	
	/**
	 * Sets the GLSL version of the shading language. Default is "410".
	 *
	 * \param v GLSL version
	 */
	static void set_glsl_version(const std::string& v);

private:
	uint32_t program = 0;
	
	std::string defines;
	std::string vert_shader;
	std::string geom_shader;
	std::string frag_shader;
	
	static std::string glsl_version;
	
	uint32_t compile_shader(const std::string& s, int32_t t) const;
	
	void compile_shaders() const;
	
	std::string get_link_info() const;
	
	void resolve_defines(std::string& s) const;
	
	static void resolve_version(std::string& s);
	
	static std::string get_compile_info(uint32_t s, uint32_t t);
	
	static std::string get_error_info(const std::string& c, const std::string& s);
	
	friend class VertexObject;
};

class VertexObject {
public:
	/**
	 * Creates a new VertexObject object.
	 */
	explicit VertexObject();
	
	/**
	 * Deletes this VertexObject object.
	 */
	~VertexObject();
	
	/**
	 * The copy constructor is deleted.
	 */
	VertexObject(const VertexObject&) = delete;
	
	/**
	 * The copy assignment operator is deleted.
	 */
	VertexObject& operator=(const VertexObject&) = delete;
	
	/**
	 * Loads the specified mesh to this vertex object.
	 *
	 * \param m mesh
	 * \param g material group
	 */
	void load(const Mesh& m, const MeshGroup& g);
	
	/**
	 * Attaches vertex object to the target shader to automatically match the
	 * input data with the shader locations.
	 *
	 * \param s target shader
	 */
	void attach(const Shader& s) const;
	
	/**
	 * Renders the vertex object to the current render target.
	 */
	void render() const;
	
private:
	uint32_t id = 0;
	
	uint32_t buffer_id = 0;
	
	int length = 0;
	
	std::vector<std::string> names;
	
	std::vector<int> sizes;
	
	std::vector<int> locations;
};

class Texture {
public:
	/**
	 * Creates a new Texture object.
	 */
	explicit Texture();
	
	/**
	 * Deletes this Texture object.
	 */
	~Texture();
	
	/**
	 * The copy constructor is deleted.
	 */
	Texture(const Texture&) = delete;
	
	/**
	 * The copy assignment operator is deleted.
	 */
	Texture& operator=(const Texture&) = delete;
	
	/**
	 * Initializes the texture as 1d texture with empty data.
	 *
	 * \param w the width of texture
	 * \param f texture format
	 * \param t image data type
	 */
	void init_1d(int w, TextureFormat f, ImageType t = IMAGE_UBYTE);
	
	/**
	 * Initializes the texture as 2d texture with empty data.
	 *
	 * \param w the width of texture
	 * \param h the height of texture
	 * \param f texture format
	 * \param t image data type
	 */
	void init_2d(int w, int h, TextureFormat f, ImageType t = IMAGE_UBYTE);
	
	/**
	 * Initializes the texture as 2d texture with the specified image.
	 *
	 * \param i image
	 * \param f texture format
	 * \param t image data format
	 */
	void init_2d(const Image& i, TextureFormat f, ImageFormat t = IMAGE_COLOR);
	
	/**
	 * Initializes the texture as 3d texture with empty data.
	 *
	 * \param w the width of texture
	 * \param h the height of texture
	 * \param d the depth of texture
	 * \param f texture format
	 * \param t image data type
	 */
	void init_3d(int w, int h, int d, TextureFormat f, ImageType t = IMAGE_UBYTE);
	
	/**
	 * Initializes the texture as cube texture with empty data.
	 *
	 * \param w the width of texture
	 * \param h the height of texture
	 * \param f texture format
	 * \param t image data type
	 */
	void init_cube(int w, int h, TextureFormat f, ImageType t = IMAGE_UBYTE);
	
	/**
	 * Initializes the texture as cube texture with the specified images from
	 * different perspectives.
	 *
	 * \param px right (+X) side of cube image
	 * \param nx left  (-X) side of cube image
	 * \param py upper (+Y) side of cube image
	 * \param ny lower (-Y) side of cube image
	 * \param pz front (+Z) side of cube image
	 * \param nz back  (-Z) side of cube image
	 * \param f texture format
	 * \param t image data format
	 */
	void init_cube(const Image& px, const Image& nx, const Image& py, const Image& ny,
				   const Image& pz, const Image& nz, TextureFormat f, ImageFormat t = IMAGE_COLOR);
	
	/**
	 * Initializes the texture as 1D array texture with empty data.
	 *
	 * \param w the width of texture
	 * \param l the layer of texture
	 * \param f texture format
	 * \param t image data type
	 */
	void init_1d_array(int w, int l, TextureFormat f, ImageType t = IMAGE_UBYTE);
	
	/**
	 * Initializes the texture as 2D array texture with empty data.
	 *
	 * \param w the width of texture
	 * \param h the height of texture
	 * \param l the layer of texture
	 * \param f image format
	 * \param t image data type
	 */
	void init_2d_array(int w, int h, int l, TextureFormat f, ImageType t = IMAGE_UBYTE);
	
	/**
	 * Initializes the texture as cube array texture with empty data.
	 *
	 * \param w the width of texture
	 * \param h the height of texture
	 * \param l the layer of texture
	 * \param f texture format
	 * \param t image data type
	 */
	void init_cube_array(int w, int h, int l, TextureFormat f, ImageType t = IMAGE_UBYTE);
	
	/**
	 * Returns the width of texture if the texture is legal, returns 0
	 * otherwise.
	 */
	int get_width() const;
	
	/**
	 * Returns the height of texture if the texture is legal, returns 0
	 * otherwise.
	 */
	int get_height() const;
	
	/**
	 * Returns the depth of texture if the texture is legal, returns 0
	 * otherwise.
	 */
	int get_depth() const;
	
	/**
	 * Returns the layer of texture if the texture is legal, returns 0
	 * otherwise. The type of texture must be one of TEXTURE_1D_ARRAY,
	 * TEXTURE_2D_ARRAY, TEXTURE_CUBE_ARRAY.
	 */
	int get_layer() const;
	
	/**
	 * Returns the type of texture.
	 */
	TextureType get_type() const;
	
	/**
	 * Returns the format of texture.
	 */
	TextureFormat get_format() const;
	
	/**
	 * Copys texture to the specified image. The type of texture must be
	 * TEXTURE_2D.
	 */
	void copy_to_image(Image& i) const;
	
	/**
	 * Generates mipmap for the texture.
	 */
	void generate_mipmap() const;
	
	/**
	 * Sets the wrapping mode on S coordinate. Default is TEXTURE_REPEAT.
	 *
	 * \param m wrapping mode
	 */
	void set_wrap_s(TextureWrappingMode m) const;
	
	/**
	 * Sets the wrapping mode on T coordinate. Default is TEXTURE_REPEAT.
	 *
	 * \param m wrapping mode
	 */
	void set_wrap_t(TextureWrappingMode m) const;
	
	/**
	 * Sets the wrapping mode on R coordinate. Default is TEXTURE_REPEAT.
	 *
	 * \param m wrapping mode
	 */
	void set_wrap_r(TextureWrappingMode m) const;
	
	/**
	 * Sets the wrapping modes on S, T and R coordinates.
	 *
	 * \param m wrapping mode
	 */
	void set_wrap_all(TextureWrappingMode m) const;
	
	/**
	 * Sets the magnification and minification filters of texture. Default is
	 * TEXTURE_LINEAR and TEXTURE_NEAREST_MIPMAP_LINEAR.
	 *
	 * \param mag magnification filter
	 * \param min minification filter
	 */
	void set_filters(TextureFilter mag, TextureFilter min) const;
	
	/**
	 * Sets the border color used for border texels. Default is (0, 0, 0, 0).
	 *
	 * \param c border color
	 */
	void set_border_color(const Vec4& c) const;
	
	/**
	 * Sets the range of LOD for mipmap texture. Default is 0 and 1000.
	 *
	 * \param min lowest defined level
	 * \param max highest defines level
	 */
	void set_lod_range(int min, int max) const;
	
	/**
	 * Sets the bias of LOD for mipmap texture. Default is 0.
	 *
	 * \param b bias level
	 */
	void set_lod_bias(int b) const;
	
	/**
	 * Sets the texture active in the specified location.
	 *
	 * \param l the location of texture
	 */
	int activate(int l) const;
	
	/**
	 * Returns the default texture format for the specified channel and byte.
	 *
	 * \param c channel
	 * \param b byte
	 */
	static TextureFormat default_format(int c, int b);
	
	/**
	 * Returns the default texture format for the specified image.
	 *
	 * \param i image
	 */
	static TextureFormat default_format(const Image& i);
	
private:
	uint32_t id = 0;
	
	int width = 0;
	
	int height = 0;
	
	int depth = 0;
	
	TextureType type;
	
	TextureFormat format;
	
	void set_dimensions(int w, int h, int d);
	
	void set_parameters(TextureType t, TextureFormat f);
	
	friend class RenderTarget;
};

class RenderBuffer {
public:
	/**
	 * Creates a new RenderBuffer object.
	 */
	explicit RenderBuffer();
	
	/**
	 * Deletes this RenderBuffer object.
	 */
	~RenderBuffer();
	
	/**
	 * The copy constructor is deleted.
	 */
	RenderBuffer(const RenderBuffer&) = delete;
	
	/**
	 * The copy assignment operator is deleted.
	 */
	RenderBuffer& operator=(const RenderBuffer&) = delete;
	
	/**
	 * Initializes the render buffer with empty data, which is similar to 2D
	 * texture.
	 *
	 * \param w the width of render buffer
	 * \param h the height of render buffer
	 * \param f texture format
	 */
	void init(int w, int h, TextureFormat f) const;
	
private:
	uint32_t id = 0;
	
	friend class RenderTarget;
};

class RenderTarget {
public:
	/**
	 * Creates a new RenderTarget object.
	 */
	explicit RenderTarget();
	
	/**
	 * Deletes this RenderTarget object.
	 */
	~RenderTarget();
	
	/**
	 * The copy constructor is deleted.
	 */
	RenderTarget(const RenderTarget&) = delete;
	
	/**
	 * The copy assignment operator is deleted.
	 */
	RenderTarget& operator=(const RenderTarget&) = delete;
	
	/**
	 * Sets the 2D texture, cube texture, 2D texture array or 3D texture to the
	 * render target.
	 *
	 * \param t 2D texture, cube texture, 2D texture array or 3D texture
	 * \param i the index of texture
	 * \param l mipmap level
	 * \param p if texture is cube texture, specifies the face of cube texture
	 *          if texture is 2D texture array, specifies the layer of texture
	 *          if texture is 3D texture, specifies the layer of texture
	 */
	void set_texture(const Texture& t, unsigned int i, int l = 0, int p = 0) const;
	
	/**
	 * Sets the 2D texture or cube texture as the depth texture of render
	 * target.
	 *
	 * \param t 2D texture or cube texture
	 * \param l mipmap level
	 * \param p if texture is cube texture, specifies the face of cube texture
	 *          if texture is 2D texture array, specifies the layer of texture
	 */
	void set_depth_texture(const Texture& t, int l = 0, int p = 0) const;
	
	/**
	 * Sets the 2D texture or cube texture as the stencil texture of render
	 * target.
	 *
	 * \param t 2D texture or cube texture
	 * \param l mipmap level
	 * \param p if texture is cube texture, specifies the face of cube texture
	 *          if texture is 2D texture array, specifies the layer of texture
	 */
	void set_stencil_texture(const Texture& t, int l = 0, int p = 0) const;
	
	/**
	 * Sets the 2D texture or cube texture as the depth stencil te texture of
	 * render target.
	 *
	 * \param t 2D texture or cube texture
	 * \param l mipmap level
	 * \param p if texture is cube texture, specifies the face of cube texture
	 *          if texture is 2D texture array, specifies the layer of texture
	 */
	void set_depth_stencil_texture(const Texture& t, int l = 0, int p = 0) const;
	
	/**
	 * Sets the render buffer as the depth buffer of render target.
	 *
	 * \param r render buffer
	 */
	void set_depth_buffer(const RenderBuffer& r) const;
	
	/**
	 * Sets the render buffer as the stencil buffer of render target.
	 *
	 * \param r render buffer
	 */
	void set_stencil_buffer(const RenderBuffer& r) const;
	
	/**
	 * Sets the render buffer as the depth stencil buffer of render target.
	 *
	 * \param r render buffer
	 */
	void set_depth_stencil_buffer(const RenderBuffer& r) const;
	
	/**
	 * Sets the number of targets to be rendered into. Default is 1.
	 *
	 * \param n target number
	 */
	void set_target_number(int n) const;
	
	/**
	 * Activates the render target.
	 */
	void activate() const;
	
	/**
	 * Activates the specified render target. If render target is nullptr,
	 * activates the default render target.
	 *
	 * \param t render target
	 */
	static void activate(const RenderTarget* t);
	
private:
	uint32_t id = 0;
	
	std::vector<uint32_t> attachments;
	
	void set_framebuffer(const Texture& t, uint32_t a, int l, int p) const;
	
	void set_framebuffer(const RenderBuffer& r, uint32_t a) const;
};

}

/* -------------------------------------------------------------------------- */
/* ---- ink/shaders/ShaderCache.h ------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

class ShaderCache {
public:
	/**
	 * Loads the specified string of vertex shader content to the shader.
	 *
	 * \param n shader name
	 * \param s vertex shader
	 */
	static void load_vert(const std::string& n, const char* s);
	
	/**
	 * Loads the specified string of vertex shader content to the shader.
	 *
	 * \param n shader name
	 * \param s vertex shader
	 */
	static void load_vert(const std::string& n, const std::string& s);
	
	/**
	 * Loads the specified string of geometry shader content to the shader.
	 *
	 * \param n shader name
	 * \param s geometry shader
	 */
	static void load_geom(const std::string& n, const char* s);
	
	/**
	 * Loads the specified string of geometry shader content to the shader.
	 *
	 * \param n shader name
	 * \param s geometry shader
	 */
	static void load_geom(const std::string& n, const std::string& s);
	
	/**
	 * Loads the specified string of fragment shader content to the shader.
	 *
	 * \param n shader name
	 * \param s fragment shader
	 */
	static void load_frag(const std::string& n, const char* s);
	
	/**
	 * Loads the specified string of fragment shader content to the shader.
	 *
	 * \param n shader name
	 * \param s fragment shader
	 */
	static void load_frag(const std::string& n, const std::string& s);
	
	/**
	 * Loads the vertex shader from the specified GLSL file to the shader.
	 *
	 * \param n shader name
	 * \param p the path to vertex shader file
	 */
	static void load_vert_file(const std::string& n, const std::string& p);
	
	/**
	 * Loads the geometry shader from the specified GLSL file to the shader.
	 *
	 * \param n shader name
	 * \param p the path to geometry shader file
	 */
	static void load_geom_file(const std::string& n, const std::string& p);
	
	/**
	 * Loads the fragment shader from the specified GLSL file to the shader.
	 *
	 * \param n shader name
	 * \param p the path to fragment shader file
	 */
	static void load_frag_file(const std::string& n, const std::string& p);
	
	/**
	 * Returns true if the vertex shader is loaded to the specified shader.
	 *
	 * \param n shader name
	 */
	static bool has_vert(const std::string& n);
	
	/**
	 * Returns true if the geometry shader is loaded to the specified shader.
	 *
	 * \param n shader name
	 */
	static bool has_geom(const std::string& n);
	
	/**
	 * Returns true if the fragment shader is loaded to the specified shader.
	 *
	 * \param n shader name
	 */
	static bool has_frag(const std::string& n);
	
	/**
	 * Returns the shader with the specified name from shader cache.
	 *
	 * \param n shader name
	 */
	static const Gpu::Shader* fetch(const std::string& n);
	
	/**
	 * Returns the shader with the specified name and defines from shader cache.
	 *
	 * \param n shader name
	 * \param d defines
	 */
	static const Gpu::Shader* fetch(const std::string& n, const Defines& d);
	
	/**
	 * Clears the shader cache with the specified name.
	 *
	 * \param n shader name
	 */
	static void clear_cache(const std::string& n);
	
	/**
	 * Clears the shader cache with the specified name and defines.
	 *
	 * \param n shader name
	 * \param d defines
	 */
	static void clear_cache(const std::string& n, const Defines& d);
	
	/**
	 * Clears all the shader values from the shader cache.
	 */
	static void clear_caches();
	
	/**
	 * Returns the path to find the included shaders.
	 */
	static std::string get_include_path();
	
	/**
	 * Sets the path to find the included shaders. Default is "ink/shaders/
	 * include/".
	 *
	 * \param p include path
	 */
	static void set_include_path(const std::string& p);
	
private:
	static void resolve_includes(std::string& s);
	
	static std::string include_path;
	
	static std::unordered_map<std::string, std::string> vert_shaders;
	static std::unordered_map<std::string, std::string> geom_shaders;
	static std::unordered_map<std::string, std::string> frag_shaders;
	
	static std::unordered_map<std::string, std::unique_ptr<Gpu::Shader> > cache;
};

}

/* -------------------------------------------------------------------------- */
/* ---- ink/shaders/ShaderLib.h --------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

class ShaderLib {
public:
	/**
	 * Returns the shader with the specified name from shader cache. You can
	 * clear the cache in ShaderCache.
	 *
	 * \param n shader name
	 */
	static const Gpu::Shader* fetch(const std::string& n);
	
	/**
	 * Returns the shader with the specified name and defines from shader cache.
	 * You can clear the cache in ShaderCache.
	 *
	 * \param n shader name
	 * \param d defines
	 */
	static const Gpu::Shader* fetch(const std::string& n, const Defines& d);
	
	/**
	 * Returns the path to find the shaders.
	 */
	static std::string get_library_path();
	
	/**
	 * Sets the path to find the shaders. Default is "ink/shaders/library/".
	 *
	 * \param p library path
	 */
	static void set_library_path(const std::string& p);
	
private:
	static std::string library_path;
};

}

/* -------------------------------------------------------------------------- */
/* ---- ink/lights/LinearFog.h ---------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

class LinearFog {
public:
	bool visible = true;       /**< whether the fog is visible */
	Vec3 color = {1, 1, 1};    /**< the color of fog */
	float near = 1;            /**< the nearest distance of fog */
	float far = 1000;          /**< the farthest distance of fog */
	
	/**
	 * Creates a new Fog object and initializes it with color and distances.
	 *
	 * \param c the color of fog
	 * \param n the nearest distance of fog
	 * \param f the farthest distance of fog
	 */
	explicit LinearFog(const Vec3& c = {1, 1, 1}, float n = 1, float f = 1000);
};

}

/* -------------------------------------------------------------------------- */
/* ---- ink/lights/Exp2Fog.h ------------------------------------------------ */
/* -------------------------------------------------------------------------- */

namespace Ink {

class Exp2Fog {
public:
	bool visible = true;       /**< whether the fog is visible */
	Vec3 color = {1, 1, 1};    /**< the color of fog */
	float near = 1;            /**< the nearest distance of fog */
	float density = 0.001;     /**< the density of fog */
	
	/**
	 * Creates a new Exp2Fog object and initializes it with color, distance and
	 * density.
	 *
	 * \param c the color of fog
	 * \param n the nearest distance of fog
	 * \param d the density of fog
	 */
	explicit Exp2Fog(const Vec3& c = {1, 1, 1}, float n = 1, float d = 0.001);
};

}

/* -------------------------------------------------------------------------- */
/* ---- ink/lights/Shadow.h ------------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

enum ShadowType {
	SHADOW_HARD,
	SHADOW_PCF,
	SHADOW_PCSS,
};

class Shadow {
public:
	ShadowType type = SHADOW_PCF;    /**< the type of shadow */
	int map_id = 0;                  /**< the ID of shadow map */
	float bias = 0;                  /**< shadow bias, should be close to 0 */
	float normal_bias = 0;           /**< shadow bias along the object normal, range is 0 to 1 */
	float radius = 3;                /**< the radius of blurred edge, only used in PCF or PCSS */
	Camera camera;                   /**< light's view camera of shadow */
	
	/**
	 * Sets the resolution of shadow map and the max number of shadows.
	 * Initializes shadows.
	 *
	 * \param w the width of shadow map
	 * \param h the height of shadow map
	 * \param n the max number of shadows
	 */
	static void init(int w = 1024, int h = 1024, int n = 16);
	
	/**
	 * Creates a new Shadow object.
	 */
	explicit Shadow() = default;
	
	/**
	 * Returns the sample numbers when using PCF / PCSS shadow.
	 */
	static int get_samples();
	
	/**
	 * Sets the sample numbers when using PCF / PCSS shadow. Must be 16, 32 or
	 * 64. Default is 32.
	 *
	 * \param s sample numbers
	 */
	static void set_samples(int s);
	
	/**
	 * Returns the resolution of shadow texture (shadow map).
	 */
	static Vec2 get_resolution();
	
	/**
	 * Activates the shadow texture (shadow map) at the specified location.
	 *
	 * \param l the location of texture
	 */
	static int activate_texture(int l);
	
	/**
	 * Returns the render target of the shadow texture (shadow map).
	 */
	const Gpu::RenderTarget* get_target() const;
	
private:
	static int samples;
	
	static Vec2 resolution;
	
	static std::unique_ptr<Gpu::Texture> shadow_map;
	
	static std::unique_ptr<Gpu::RenderTarget> shadow_target;
};

}

/* -------------------------------------------------------------------------- */
/* ---- ink/lights/Light.h -------------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

class Light {
public:
	bool visible = true;       /**< whether the light is visible */
	float intensity = 1;       /**< the intensity of light */
	Vec3 color = {1, 1, 1};    /**< the color of light */
	
	/**
	 * Creates a new Light object and initializes it with color and intensity.
	 *
	 * \param c the color of light
	 * \param i the intensity of light
	 */
	explicit Light(const Vec3& c = {1, 1, 1}, float i = 1);
};

}

/* -------------------------------------------------------------------------- */
/* ---- ink/lights/PointLight.h --------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

class PointLight : public Light {
public:
	Vec3 position;         /**< the position where the light casts from */
	float decay = 1;       /**< the decay speed of light */
	float distance = 0;    /**< the maximum distance affected by the light */
	
	/**
	 * Creates a new PointLight object and initializes it with color and
	 * intensity.
	 *
	 * \param c the color of light
	 * \param i the intensity of light
	 */
	explicit PointLight(const Vec3& c = {1, 1, 1}, float i = 1);
};

}

/* -------------------------------------------------------------------------- */
/* ---- ink/lights/SpotLight.h ---------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

class SpotLight : public Light {
public:
	Vec3 position;               /**< the position where the light casts from */
	Vec3 direction;              /**< the direction where the light casts through */
	float angle = PI / 3;        /**< the maximum angle of light */
	float decay = 1;             /**< the decay speed of light */
	float distance = 0;          /**< the maximum distance affected by the light */
	float penumbra = 0;          /**< the attenuation of spot light cone */
	bool cast_shadow = false;    /**< whether the light will cast shadow */
	Shadow shadow;               /**< the shadow of light */
	
	/**
	 * Creates a new SpotLight object and initializes it with color and
	 * intensity.
	 *
	 * \param c the color of light
	 * \param i the intensity of light
	 */
	explicit SpotLight(const Vec3& c = {1, 1, 1}, float i = 1);
};

}

/* -------------------------------------------------------------------------- */
/* ---- ink/lights/DirectionalLight.h --------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

class DirectionalLight : public Light {
public:
	Vec3 position;               /**< the position where the light casts from */
	Vec3 direction;              /**< the direction where the light casts through */
	bool cast_shadow = false;    /**< whether the light will cast shadow */
	Shadow shadow;               /**< the shadow of light */
	
	/**
	 * Creates a new DirectionalLight object and initializes it with color and
	 * intensity.
	 *
	 * \param c the color of light
	 * \param i the intensity of light
	 */
	explicit DirectionalLight(const Vec3& c = {1, 1, 1}, float i = 1);
};

}

/* -------------------------------------------------------------------------- */
/* ---- ink/lights/HemisphereLight.h ---------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

class HemisphereLight : public Light {
public:
	Vec3 ground_color = {1, 1, 1};    /**< the ground color */
	Vec3 direction = {0, 1, 0};       /**< the direction from ground to sky */
	
	/**
	 * Creates a new HemisphereLight object and initializes it with colors and
	 * intensity.
	 *
	 * \param s the sky color
	 * \param g the ground color
	 * \param i the intensity of light
	 */
	explicit HemisphereLight(const Vec3& s = {1, 1, 1},
							 const Vec3& g = {1, 1, 1}, float i = 1);
};

}

/* -------------------------------------------------------------------------- */
/* ---- ink/probes/IBLFilter.h ---------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

class IBLFilter {
public:
	/**
	 * Loads a set of specified cube images to the prefiltered radiance
	 * environment map.
	 *
	 * \param px right (+X) side of cube image
	 * \param nx left  (-X) side of cube image
	 * \param py upper (+Y) side of cube image
	 * \param ny lower (-Y) side of cube image
	 * \param pz front (+Z) side of cube image
	 * \param nz back  (-Z) side of cube image
	 * \param m prefiltered radiance environment map
	 * \param s the size of radiance environment map
	 */
	static void load_cubemap(const Image& px, const Image& nx,
							 const Image& py, const Image& ny,
							 const Image& pz, const Image& nz,
							 Gpu::Texture& m, int s = 256);
	
	/**
	 * Loads the specified equirectangular image to the prefiltered radiance
	 * environment map.
	 *
	 * \param i equirectangular image
	 * \param m prefiltered radiance environment map
	 * \param s the size of radiance environment map
	 */
	static void load_equirect(const Image& i, Gpu::Texture& m, int s = 256);
	
	/**
	 * Loads a cube texture or 2D texture to the prefiltered radiance
	 * environment map.
	 *
	 * \param t source texture
	 * \param m prefiltered radiance environment map
	 * \param s the size of radiance environment map
	 */
	static void load_texture(const Gpu::Texture& t, Gpu::Texture& m, int s = 256);
	
private:
	static Vec3 axes[];
	
	static std::unique_ptr<Gpu::VertexObject> fullscreen_plane;
	
	static std::unique_ptr<Gpu::Texture> blur_map;
	
	static std::unique_ptr<Gpu::RenderTarget> cubemap_target;
	
	static std::unique_ptr<Gpu::RenderTarget> blur_target;
	
	static bool init_fullscreen_plane();
	
	static void gaussian_weights(float s, int n, float* w);
};

}

/* -------------------------------------------------------------------------- */
/* ---- ink/probes/ReflectionProbe.h ---------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

class ReflectionProbe {
public:
	int resolution = 256;    /**< the resolution of reflection probe */
	float intensity = 1;     /**< the intensity of reflection probe */
	Vec3 position;           /**< the position of reflection probe */
	
	/**
	 * Creates a new ReflectionProbe object and initializes it with intensity,
	 * resolution and position.
	 *
	 * \param i the intensity of reflection probe
	 * \param r the resolution of reflection probe
	 * \param p the position of reflection probe
	 */
	explicit ReflectionProbe(float i = 1, int r = 256, const Vec3& p = {0, 0, 0});
	
	/**
	 * Loads a set of specified cube images to the reflection probe.
	 *
	 * \param px right (+X) side of cube image
	 * \param nx left  (-X) side of cube image
	 * \param py upper (+Y) side of cube image
	 * \param ny lower (-Y) side of cube image
	 * \param pz front (+Z) side of cube image
	 * \param nz back  (-Z) side of cube image
	 */
	void load_cubemap(const Image& px, const Image& nx,
					  const Image& py, const Image& ny,
					  const Image& pz, const Image& nz);
	
	/**
	 * Loads the specified equirectangular image to the reflection probe.
	 *
	 * \param i equirectangular image
	 */
	void load_equirect(const Image& i);
	
	/**
	 * Loads a specified cube texture or 2D texture to the reflection probe.
	 *
	 * \param t cube texture
	 */
	void load_texture(const Gpu::Texture& t);
	
	/**
	 * Activates the texture at the specified location of reflection probe.
	 *
	 * \param l the location of texture
	 */
	int activate(int l) const;
	
private:
	std::unique_ptr<Gpu::Texture> reflection_map;
};

}

/* -------------------------------------------------------------------------- */
/* ---- ink/scene/Scene.h --------------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

class Scene : public Instance {
public:
	/**
	 * Creates a new Scene object and initializes it with a name.
	 *
	 * \param n scene name
	 */
	explicit Scene(const std::string& n = "");
	
	/**
	 * Returns the material matching the specified name.
	 *
	 * \param n material name
	 */
	Material* get_material(const std::string& n) const;
	
	/**
	 * Returns the material matching the specified name and linking with the
	 * specified mesh.
	 *
	 * \param n material name
	 * \param s specified mesh
	 */
	Material* get_material(const std::string& n, const Mesh& s) const;
	
	/**
	 * Returns the material matching the specified name and linking with the
	 * specified mesh.
	 *
	 * \param n material name
	 * \param s specified instance
	 */
	Material* get_material(const std::string& n, const Instance& s) const;
	
	/**
	 * Sets the specified material with name to the scene.
	 *
	 * \param n material name
	 * \param m material
	 */
	void set_material(const std::string& n, Material* m);
	
	/**
	 * Sets the specified material with name to the scene. Links it with the
	 * specified mesh.
	 *
	 * \param n material name
	 * \param s specified mesh
	 * \param m material
	 */
	void set_material(const std::string& n, const Mesh& s, Material* m);
	
	/**
	 * Sets the specified material with name to the scene. Links it with the
	 * specified mesh.
	 *
	 * \param n material name
	 * \param s specified instance
	 * \param m material
	 */
	void set_material(const std::string& n, const Instance& s, Material* m);
	
	/**
	 * Removes the specified material matching the specified name from the
	 * scene.
	 *
	 * \param n material name
	 */
	void remove_material(const std::string& n);
	
	/**
	 * Removes the specified material matching the specified name and linking
	 * with the specified mesh from the scene.
	 *
	 * \param n material name
	 * \param s specified mesh
	 */
	void remove_material(const std::string& n, const Mesh& s);
	
	/**
	 * Removes the specified material matching the specified name and linking
	 * with the specified mesh from the scene.
	 *
	 * \param n material name
	 * \param s specified instance
	 */
	void remove_material(const std::string& n, const Instance& s);
	
	/**
	 * Removes all the materials from the scene.
	 */
	void clear_materials();
	
	/**
	 * Returns all the materials in the material library.
	 */
	std::vector<Material*> get_materials() const;
	
	/**
	 * Returns the linear fog in the scene if there is, return nullptr
	 * otherwise.
	 */
	LinearFog* get_linear_fog() const;
	
	/**
	 * Sets the specified linear fog to the scene. Only one fog can be set in a
	 * scene.
	 *
	 * \param f linear fog
	 */
	void set_fog(LinearFog* f);
	
	/**
	 * Returns the exp square fog in the scene if there is, return nullptr
	 * otherwise.
	 */
	Exp2Fog* get_exp2_fog() const;
	
	/**
	 * Sets the specified exp square fog to the scene. Only one fog can be set
	 * in a scene.
	 *
	 * \param f exp square fog
	 */
	void set_fog(Exp2Fog* f);
	
	/**
	 * Adds the specified point light to the scene. The light number should not
	 * exceed the maximum number.
	 *
	 * \param l point light
	 */
	void add_light(PointLight* l);
	
	/**
	 * Removes the specified point light from the scene.
	 */
	void remove_light(PointLight* l);
	
	/**
	 * Returns the number of point lights in the scene.
	 */
	size_t get_point_light_count() const;
	
	/**
	 * Returns the point light at the specified index in the scene.
	 *
	 * \param i the index of light
	 */
	PointLight* get_point_light(int i) const;
	
	/**
	 * Adds the specified spot light to the scene. The light number should not
	 * exceed the maximum number.
	 *
	 * \param l spot light
	 */
	void add_light(SpotLight* l);
	
	/**
	 * Removes the specified spot light from the scene.
	 *
	 * \param l spot light
	 */
	void remove_light(SpotLight* l);
	
	/**
	 * Returns the number of spot lights in the scene.
	 */
	size_t get_spot_light_count() const;
	
	/**
	 * Returns the spot light at the specified index in the scene.
	 *
	 * \param i the index of light
	 */
	SpotLight* get_spot_light(int i) const;
	
	/**
	 * Adds the specified directional light to the scene. The light number
	 * should not exceed the maximum number.
	 *
	 * \param l directional light
	 */
	void add_light(DirectionalLight* l);
	
	/**
	 * Removes the specified directional light from the scene.
	 *
	 * \param l directional light
	 */
	void remove_light(DirectionalLight* l);
	
	/**
	 * Returns the number of directional lights in the scene.
	 */
	size_t get_directional_light_count() const;
	
	/**
	 * Returns the directional light at the specified index in the scene.
	 *
	 * \param i the index of light
	 */
	DirectionalLight* get_directional_light(int i) const;
	
	/**
	 * Adds the specified hemisphere light to the scene. The light number should
	 * not exceed the maximum number.
	 *
	 * \param l hemisphere light
	 */
	void add_light(HemisphereLight* l);
	
	/**
	 * Removes the specified hemisphere light from the scene.
	 *
	 * \param l hemisphere light
	 */
	void remove_light(HemisphereLight* l);
	
	/**
	 * Returns the number of hemisphere lights in the scene.
	 */
	size_t get_hemisphere_light_count() const;
	
	/**
	 * Returns the hemisphere light at the specified index in the scene.
	 *
	 * \param i the index of light
	 */
	HemisphereLight* get_hemisphere_light(int i) const;
	
	/**
	 * Removes all the point & spot & directional & hemisphere lights from the
	 * scene.
	 */
	void clear_lights();
	
	/**
	 * Updates the local and global matrices of all descendant instances.
	 */
	void update_instances();
	
	/**
	 * Returns an instance vector of all descendant instances.
	 */
	std::vector<const Instance*> to_instances() const;
	
	/**
	 * Returns an instance vector of all descendant visible instances.
	 */
	std::vector<const Instance*> to_visible_instances() const;
	
private:
	LinearFog* linear_fog = nullptr;
	Exp2Fog* exp2_fog = nullptr;
	
	std::vector<PointLight*> point_lights;
	std::vector<SpotLight*> spot_lights;
	std::vector<DirectionalLight*> directional_lights;
	std::vector<HemisphereLight*> hemisphere_lights;
	
	std::unordered_map<std::string, Material*> material_library;
};

}

/* -------------------------------------------------------------------------- */
/* ---- ink/renderer/Renderer.h --------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

enum RenderingMode {
	FORWARD_RENDERING,
	DEFERRED_RENDERING,
};

enum ToneMapMode {
	LINEAR_TONE_MAP,
	REINHARD_TONE_MAP,
	OPTIMIZED_TONE_MAP,
	ACES_FILMIC_TONE_MAP,
};

class Renderer {
public:
	using TextureCallback = std::function<void(Gpu::Texture&)>;
	
	/**
	 * Creates a new Renderer object.
	 */
	explicit Renderer() = default;
	
	/**
	 * Returns the clear color (.xyz) and opacity (.w).
	 */
	Vec4 get_clear_color() const;
	
	/**
	 * Sets the specified clear color (.xyz) and opacity (.w). Default is (0, 0,
	 * 0, 0).
	 *
	 * \param c clear color and opacity
	 */
	void set_clear_color(const Vec4& c);
	
	/**
	 * Clears the color, depth or stencil drawing buffers. Initializes the color
	 * buffer to the current clear color value.
	 *
	 * \param c whether to clear the color buffer
	 * \param d whether to clear the depth buffer
	 * \param s whether to clear the stencil buffer
	 */
	void clear(bool c = true, bool d = true, bool s = true) const;
	
	/**
	 * Returns the viewport region.
	 */
	Gpu::Rect get_viewport() const;
	
	/**
	 * Sets the viewport region to render from (x, y) to (x + width, y +
	 * height). Default is (0, 0, 0, 0).
	 *
	 * \param v viewport region
	 */
	void set_viewport(const Gpu::Rect& v);
	
	/**
	 * Returns true if scissor test is enabled.
	 */
	bool get_scissor_test() const;
	
	/**
	 * Determines whether to enable scissor test. Default is false.
	 *
	 * \param t enable scissor test
	 */
	void set_scissor_test(bool t);
	
	/**
	 * Returns the scissor region in scissor test.
	 */
	Gpu::Rect get_scissor() const;
	
	/**
	 * Sets the scissor region from (x, y) to (x + width, y + height) in scissor
	 * test. Default is (0, 0, 0, 0).
	 *
	 * \param s scissor region
	 */
	void set_scissor(const Gpu::Rect& s);
	
	/**
	 * Returns the mode in rendering. If mode is DEFERRED_RENDERING, the number
	 * of render targets should be 4.
	 */
	RenderingMode get_rendering_mode() const;
	
	/**
	 * Sets the specified mode in rendering. If mode is DEFERRED_RENDERING, the
	 * number of render targets should be 4. Default is DEFERRED_RENDERING.
	 *
	 * \param m rendering mode
	 */
	void set_rendering_mode(RenderingMode m);
	
	/**
	 * Returns the mode in tone mapping. This term will only be used when the
	 * rendering mode is FORWARD_RENDERING.
	 */
	ToneMapMode get_tone_map_mode() const;
	
	/**
	 * Returns the exposure in tone mapping. This term will only be used when
	 * the rendering mode is FORWARD_RENDERING.
	 */
	float get_tone_map_exposure() const;
	
	/**
	 * Sets the specified mode and exposure in tone mapping. This term will only
	 * be used in forward rendering or transparent object rendering. Default is
	 * LINEAR_TONE_MAP and 1.
	 *
	 * \param m tone mapping mode
	 * \param e tone mapping exposure
	 */
	void set_tone_map(ToneMapMode m, float e);
	
	/**
	 * Returns the current render target if there is, returns nullptr otherwise.
	 */
	const Gpu::RenderTarget* get_target() const;
	
	/**
	 * Sets the render target. When nullptr is given, the default render target
	 * is set as the render target.
	 *
	 * \param t render target
	 */
	void set_target(const Gpu::RenderTarget* t);
	
	/**
	 * Sets the texture callback which will be called when texture is created.
	 *
	 * \param f texture callback function
	 */
	void set_texture_callback(const TextureCallback& f);
	
	/**
	 * Returns the intensity of skybox.
	 */
	float get_skybox_intensity() const;
	
	/**
	 * Sets the intensity of skybox. Default is 1.
	 *
	 * \param i intensity
	 */
	void set_skybox_intensity(float i);
	
	/**
	 * Loads a set of specified images, one for each side of the skybox cubemap.
	 *
	 * \param px right (+X) side of skybox cubemap
	 * \param nx left  (-X) side of skybox cubemap
	 * \param py upper (+Y) side of skybox cubemap
	 * \param ny lower (-Y) side of skybox cubemap
	 * \param pz front (+Z) side of skybox cubemap
	 * \param nz back  (-Z) side of skybox cubemap
	 */
	void load_skybox(const Image& px, const Image& nx,
					 const Image& py, const Image& ny,
					 const Image& pz, const Image& nz);
	
	/**
	 * Loads the specified equirectangular image to skybox.
	 *
	 * \param i equirectangular image
	 */
	void load_skybox(const Image& i);
	
	/**
	 * Renders skybox using a camera. This function should be called in the
	 * first place. Otherwise it will overwrite the current render target.
	 *
	 * \param c camera
	 */
	void render_skybox(const Camera& c) const;
	
	/**
	 * Loads the specified mesh and creates corresponding vertex object.
	 *
	 * \param m mesh
	 */
	void load_mesh(const Mesh& m);
	
	/**
	 * Unloads the specified mesh and deletes corresponding vertex object.
	 */
	void unload_mesh(const Mesh& m);
	
	/**
	 * Clears all values from the image cache. The caches will be generated
	 * automatically when loading meshes.
	 */
	void clear_mesh_caches();
	
	/**
	 * Loads the specified image and creates corresponding texture. This function
	 * will invoke the texture callback.
	 *
	 * \param i image
	 */
	void load_image(const Image& i);
	
	/**
	 * Unloads the specified image and deletes corresponding texture.
	 *
	 * \param i image
	 */
	void unload_image(const Image& i);
	
	/**
	 * Clears all values from the image cache. The caches will be generated
	 * automatically when loading images.
	 */
	void clear_image_caches();
	
	/**
	 * Loads all the meshes and images in the scene.
	 *
	 * \param s scene
	 */
	void load_scene(const Scene& s);
	
	/**
	 * Unloads all the meshes and images in the scene.
	 *
	 * \param s scene
	 */
	void unload_scene(const Scene& s);
	
	/**
	 * Clears all values from the scene cache. The caches will be generated
	 * automatically when loading meshes, images or scenes.
	 */
	void clear_scene_caches();
	
	/**
	 * Renders a scene using a camera. The results will be rendered to the
	 * specified render target.
	 *
	 * \param s scene
	 * \param c camera
	 */
	void render(const Scene& s, const Camera& c) const;
	
	/**
	 * Renders the transparent objects in a scene using a camera. The results
	 * will be rendered to the specified render target.
	 *
	 * \param s scene
	 * \param c camera
	 */
	void render_transparent(const Scene& s, const Camera& c) const;
	
	/**
	 * Renders a scene using the camera of shadow. The results will be rendered
	 * to the shadow map.
	 *
	 * \param s scene
	 * \param t target shadow
	 */
	void render_shadow(const Scene& s, const Shadow& t) const;
	
	/**
	 * Updates the shadow of spot light. This function will update the
	 * parameters of shadow's camera and render shadow map.
	 *
	 * \param s scene
	 * \param l spot light
	 */
	void update_shadow(const Scene& s, SpotLight& l) const;
	
	/**
	 * Updates the shadow of directional light. This function will update the
	 * parameters shadow'camera and render shadow map.
	 *
	 * \param s scene
	 * \param l directional light
	 */
	void update_shadow(const Scene& s, DirectionalLight& l) const;
	
	/**
	 * Updates the reflection probe at its position. The scene will be rendered
	 * in forward rendering mode.
	 *
	 * \param s scene
	 * \param r reflection probe
	 */
	void update_probe(const Scene& s, ReflectionProbe& r) const;
	
	/**
	 * Updates all the descendant instances in the scene before rendering the
	 * scene.
	 *
	 * \param s scene
	 */
	static void update_scene(Scene& s);
	
	/**
	 * Sets the defines object for material. The object will be defined in
	 * vertex, geometry and fragment shaders.
	 *
	 * \param m material
	 * \param d defines
	 */
	static void set_material_defines(const Material& m, Defines& d);
	
	/**
	 * Sets the defines object for scene. The object will be defined in vertex,
	 * geometry and fragment shaders.
	 *
	 * \param s scene
	 * \param d defines
	 */
	static void set_scene_defines(const Scene& s, Defines& d);
	
	/**
	 * Sets the defines object for tone mapping. The object will be defined in
	 * vertex, geometry and fragment shaders.
	 *
	 * \param m tone mapping mode
	 * \param d defines
	 */
	static void set_tone_map_defines(int m, Defines& d);
	
	/**
	 * Returns the uniforms object of material. The object will be passed to
	 * vertex, geometry and fragment shaders.
	 *
	 * \param s scene
	 * \param shader shader
	 */
	static void set_light_uniforms(const Scene& s, const Gpu::Shader& shader);
	
private:
	Vec4 clear_color = {0, 0, 0, 0};
	
	bool scissor_test = false;
	
	Gpu::Rect scissor = Gpu::Rect(0, 0, 0, 0);
	
	Gpu::Rect viewport = Gpu::Rect(0, 0, 0, 0);
	
	const Gpu::RenderTarget* target = nullptr;
	
	TextureCallback texture_callback = [](Gpu::Texture& t) -> void {
		t.generate_mipmap(); /* generate mipmap for every texture */
	};
	
	RenderingMode rendering_mode = DEFERRED_RENDERING;
	
	ToneMapMode tone_map_mode = LINEAR_TONE_MAP;
	
	float tone_map_exposure = 1;
	
	float skybox_intensity = 1;
	
	std::unique_ptr<Gpu::Texture> skybox_map;
	
	std::unordered_map<const Mesh*, std::unique_ptr<Gpu::VertexObject[]> > mesh_cache;
	
	std::unordered_map<const Image*, std::unique_ptr<Gpu::Texture> > image_cache;
	
	static std::unique_ptr<Gpu::VertexObject> cube;
	
	static std::unique_ptr<Gpu::Texture> probe_map;
	
	static std::unique_ptr<Gpu::RenderBuffer> probe_buffer;
	
	static std::unique_ptr<Gpu::RenderTarget> probe_target;
	
	void render_skybox_to_buffer(const Camera& c, RenderingMode r) const;
	
	void render_to_buffer(const Scene& s, const Camera& c, RenderingMode r, bool t) const;
	
	void render_to_shadow(const Scene& s, const Camera& c) const;
	
	static bool init_cube();
	
	static void sort_instances(const Camera& c, std::vector<const Instance*>& l, bool t);
};

}

/* -------------------------------------------------------------------------- */
/* ---- ink/postprocess/RenderPass.h ---------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

class RenderPass {
public:
	/**
	 * Creates a new RenderPass object.
	 */
	explicit RenderPass() = default;
	
	/**
	 * Initializes the render pass and prepare the resources for rendering.
	 */
	virtual void init() = 0;
	
	/**
	 * Compiles the required shaders and renders to the render target.
	 */
	virtual void render() = 0;
	
	/**
	 * Returns the current render target if there is, returns nullptr otherwise.
	 */
	const Gpu::RenderTarget* get_target() const;
	
	/**
	 * Sets the render target. When nullptr is given, the default render target
	 * is set as the render target.
	 *
	 * \param t render target
	 */
	void set_target(const Gpu::RenderTarget* t);
	
	/**
	 * Returns the viewport region.
	 */
	static Gpu::Rect get_viewport();
	
	/**
	 * Sets the viewport region to render from (x, y) to (x + width, y +
	 * height).
	 *
	 * \param v viewport region
	 */
	static void set_viewport(const Gpu::Rect& v);
	
	/**
	 * Renders the full screen triangle with shader. The result will be rendered
	 * to the specified render target.
	 *
	 * \param s shader
	 * \param t render target
	 */
	static void render_to(const Gpu::Shader* s, const Gpu::RenderTarget* t);
	
protected:
	const Gpu::RenderTarget* target = nullptr;
	
	static Gpu::Rect viewport;
	
	static std::unique_ptr<Gpu::VertexObject> fullscreen_plane;
	
	static bool init_fullscreen_plane();
};

}

/* -------------------------------------------------------------------------- */
/* ---- ink/postprocess/CopyPass.h ------------------------------------------ */
/* -------------------------------------------------------------------------- */

namespace Ink {

class CopyPass : public RenderPass {
public:
	/**
	 * Creates a new CopyPass object.
	 */
	explicit CopyPass() = default;
	
	/**
	 * Initializes the render pass and prepares the resources for rendering.
	 */
	void init() override;
	
	/**
	 * Compiles the required shaders and renders to the render target.
	 */
	void render() override;
	
	/**
	 * Returns the 2D texture represents the input of rendering pass.
	 */
	const Gpu::Texture* get_texture() const;
	
	/**
	 * Sets the specified 2D texture as the input of rendering pass.
	 *
	 * \param t source texture
	 */
	void set_texture(const Gpu::Texture* t);
	
private:
	const Gpu::Texture* map = nullptr;
};

}

/* -------------------------------------------------------------------------- */
/* ---- ink/postprocess/BlendPass.h ----------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

class BlendPass : public RenderPass {
public:
	/**
	 * Creates a new BlendPass object.
	 */
	explicit BlendPass() = default;
	
	/**
	 * Initializes the render pass and prepares the resources for rendering.
	 */
	void init() override;
	
	/**
	 * Compiles the required shaders and renders to the render target.
	 */
	void render() override;
	
	/**
	 * Returns the 2D texture A represents the input of rendering pass.
	 */
	const Gpu::Texture* get_texture_a() const;
	
	/**
	 * Sets the specified 2D texture A as the input of rendering pass.
	 *
	 * \param t texture A
	 */
	void set_texture_a(const Gpu::Texture* t);
	
	/**
	 * Returns the 2D texture B represents the input of rendering pass.
	 */
	const Gpu::Texture* get_texture_b() const;
	
	/**
	 * Sets the specified 2D texture B as the input of rendering pass.
	 *
	 * \param t texture B
	 */
	void set_texture_b(const Gpu::Texture* t);
	
	/**
	 * Returns the operations when applying blending.
	 */
	std::string get_operation() const;
	
	/**
	 * Sets the operations when applying blending. Default is "a * b".
	 *
	 * \param o operation
	 */
	void set_operation(const std::string& o);
	
	/**
	 * Returns the swizzle of texture A as the multiplication factor in
	 * blending.
	 */
	std::string get_swizzle_a() const;
	
	/**
	 * Sets the swizzle of texture A as the multiplication factor in blending.
	 * Default is ".xyzw".
	 *
	 * \param s swizzle A
	 */
	void set_swizzle_a(const std::string& s);
	
	/**
	 * Returns the swizzle of texture B as the multiplication factor in
	 * blending.
	 */
	std::string get_swizzle_b() const;
	
	/**
	 * Sets the swizzle of texture B as the multiplication factor in blending.
	 * Default is ".xyzw".
	 *
	 * \param s swizzle B
	 */
	void set_swizzle_b(const std::string& s);
	
private:
	std::string operation = "a * b";
	std::string swizzle_a = ".xyzw";
	std::string swizzle_b = ".xyzw";
	
	const Gpu::Texture* map_a = nullptr;
	const Gpu::Texture* map_b = nullptr;
};

}

/* -------------------------------------------------------------------------- */
/* ---- ink/postprocess/BlurPass.h ------------------------------------------ */
/* -------------------------------------------------------------------------- */

namespace Ink {

enum BlurType {
	BLUR_BOX,
	BLUR_GAUSSIAN,
	BLUR_BILATERAL,
};

class BlurPass : public RenderPass {
public:
	BlurType type = BLUR_BOX;    /**< the type of the blur */
	int width = 0;               /**< the width of screen */
	int height = 0;              /**< the height of screen */
	int channel = 4;             /**< the channel of texture */
	int radius = 3;              /**< the radius of the blur */
	float sigma_s = 2.0;         /**< the sigma of space, for Gaussian or bilateral blur */
	float sigma_r = 0.25;        /**< the sigma of range, only for bilateral blur */
	
	/**
	 * Creates a new BlurPass object.
	 */
	explicit BlurPass() = default;
	
	/**
	 * Creates a new BlurPass and initializes it with the size of screen.
	 *
	 * \param w the width of screen
	 * \param h the height of screen
	 */
	explicit BlurPass(int w, int h);
	
	/**
	 * Initializes the render pass and prepares the resources for rendering.
	 */
	void init() override;
	
	/**
	 * Compiles the required shaders and renders to the render target.
	 */
	void render() override;
	
	/**
	 * Returns the 2D texture represents the input of rendering pass.
	 */
	const Gpu::Texture* get_texture() const;
	
	/**
	 * Sets the specified 2D texture as the input of rendering pass.
	 *
	 * \param t input texture
	 */
	void set_texture(const Gpu::Texture* t);
	
private:
	const Gpu::Texture* map = nullptr;
	
	std::unique_ptr<Gpu::Texture> blur_map_1;
	std::unique_ptr<Gpu::Texture> blur_map_2;
	
	std::unique_ptr<Gpu::RenderTarget> blur_target_1;
	std::unique_ptr<Gpu::RenderTarget> blur_target_2;
};

}

/* -------------------------------------------------------------------------- */
/* ---- ink/postprocess/LightPass.h ----------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

class LightPass : public RenderPass {
public:
	/**
	 * Creates a new LightPass object.
	 */
	explicit LightPass() = default;
	
	/**
	 * Initializes the render pass and prepares the resources for rendering.
	 */
	void init() override;
	
	/**
	 * Compiles the required shaders and renders to the render target.
	 */
	void render() override;
	
	/**
	 * Returns the mode in tone mapping.
	 */
	int get_tone_map_mode() const;
	
	/**
	 * Returns the exposure in tone mapping.
	 */
	float get_tone_map_exposure() const;
	
	/**
	 * Sets the specified mode and exposure in tone mapping. Default is
	 * LINEAR_TONE_MAP and 1.
	 *
	 * \param m tone mapping mode
	 * \param e tone mapping exposure
	 */
	void set_tone_map(int m, float e);
	
	/**
	 * Returns the scene represents the input of render pass.
	 */
	const Scene* get_scene() const;
	
	/**
	 * Sets the specified scene as the input of render pass.
	 *
	 * \param s scene
	 */
	void set_scene(const Scene* s);
	
	/**
	 * Returns the camera represents the input of render pass.
	 */
	const Camera* get_camera() const;
	
	/**
	 * Sets the specified camera as the input of render pass.
	 *
	 * \param c camera
	 */
	void set_camera(const Camera* c);
	
	/**
	 * Returns the 2D texture represents the base color buffer in G-Buffers.
	 */
	const Gpu::Texture* get_texture_color() const;
	
	/**
	 * Sets the specified 2D texture as the base color buffer in G-Buffers.
	 *
	 * \param t base color texture
	 */
	void set_texture_color(const Gpu::Texture* t);
	
	/**
	 * Returns the 2D texture represents the world normal buffer in G-Buffers.
	 */
	const Gpu::Texture* get_texture_normal() const;
	
	/**
	 * Sets the specified 2D texture as the world normal buffer in G-Buffers.
	 *
	 * \param t world normal texture
	 */
	void set_texture_normal(const Gpu::Texture* t);
	
	/**
	 * Returns the 2D texture represents the material data buffer in G-Buffers.
	 */
	const Gpu::Texture* get_texture_material() const;
	
	/**
	 * Sets the specified 2D texture as the material data buffer in G-Buffers.
	 *
	 * \param t material data texture
	 */
	void set_texture_material(const Gpu::Texture* t);
	
	/**
	 * Returns the 2D texture represents the indirect light buffer in G-Buffers.
	 */
	const Gpu::Texture* get_texture_light() const;
	
	/**
	 * Sets the specified 2D texture as the indirect light buffer in G-Buffers.
	 *
	 * \param t indirect light texture
	 */
	void set_texture_light(const Gpu::Texture* t);
	
	/**
	 * Returns the 2D texture represents the depth buffer / Z-Buffer.
	 */
	const Gpu::Texture* get_texture_depth() const;
	
	/**
	 * Sets the specified 2D texture as the depth buffer / Z-Buffer.
	 *
	 * \param d depth texture
	 */
	void set_texture_depth(const Gpu::Texture* d);
	
private:
	int tone_map_mode = LINEAR_TONE_MAP;
	
	float tone_map_exposure = 1;
	
	const Scene* scene = nullptr;
	
	const Camera* camera = nullptr;
	
	const Gpu::Texture* g_color = nullptr;
	
	const Gpu::Texture* g_normal = nullptr;
	
	const Gpu::Texture* g_material = nullptr;
	
	const Gpu::Texture* g_light = nullptr;
	
	const Gpu::Texture* z_map = nullptr;
};

}

/* -------------------------------------------------------------------------- */
/* ---- ink/postprocess/SSAOPass.h ------------------------------------------ */
/* -------------------------------------------------------------------------- */

namespace Ink {

class SSAOPass : public RenderPass {
public:
	int width = 0;            /**< the width of screen */
	int height = 0;           /**< the height of screen */
	int samples = 32;         /**< sample number, must be 16, 32 or 64 */
	float radius = 0;         /**< radius to search for occluders */
	float max_radius = 0;     /**< the maximum radius from occluder to pixel */
	float max_z = 100;        /**< the maximum depth to render ambient occlusion */
	float intensity = 1;      /**< the intensity of ambient occlusion, range is 0 to 1 */
	
	/**
	 * Creates a new SSAOPass (Screen Space Ambient Occlusion) object.
	 */
	explicit SSAOPass() = default;
	
	/**
	 * Creates a new SSAOPass (Screen Space Ambient Occlusion) object and
	 * initializes it with the specified parameters.
	 *
	 * \param w the width of screen
	 * \param h the height of screen
	 * \param r radius to search for occluders
	 * \param m the maximum radius from occluder to shading point
	 * \param i the intensity of ambient occlusion, range is 0 to 1
	 */
	explicit SSAOPass(int w, int h, float r, float m, float i = 1);
	
	/**
	 * Initializes the render pass and prepares the resources for rendering.
	 */
	void init() override;
	
	/**
	 * Compiles the required shaders and renders to the render target.
	 */
	void render() override;
	
	/**
	 * Returns the camera represents the input of rendering pass.
	 */
	const Camera* get_camera() const;
	
	/**
	 * Sets the specified camera as the input of rendering pass.
	 *
	 * \param c camera
	 */
	void set_camera(const Camera* c);
	
	/**
	 * Returns the 2D texture represents the input of rendering pass.
	 */
	const Gpu::Texture* get_texture() const;
	
	/**
	 * Sets the specified 2D texture as the input of rendering pass.
	 *
	 * \param t input texture
	 */
	void set_texture(const Gpu::Texture* t);
	
	/**
	 * Returns the 2D texture represents the world normal buffer in G-Buffers.
	 */
	const Gpu::Texture* get_texture_normal() const;
	
	/**
	 * Sets the specified 2D texture as the world normal buffer in G-Buffers.
	 *
	 * \param t world normal texture
	 */
	void set_texture_normal(const Gpu::Texture* t);
	
	/**
	 * Returns the 2D texture represents the depth buffer / Z-Buffer.
	 */
	const Gpu::Texture* get_texture_depth() const;
	
	/**
	 * Sets the specified 2D texture as the depth buffer / Z-Buffer.
	 *
	 * \param t depth texture
	 */
	void set_texture_depth(const Gpu::Texture* t);
	
private:
	const Camera* camera = nullptr;
	
	const Gpu::Texture* map = nullptr;
	const Gpu::Texture* g_normal = nullptr;
	const Gpu::Texture* z_map = nullptr;
	
	std::unique_ptr<Gpu::Texture> blur_map_1;
	std::unique_ptr<Gpu::Texture> blur_map_2;
	
	std::unique_ptr<Gpu::RenderTarget> blur_target_1;
	std::unique_ptr<Gpu::RenderTarget> blur_target_2;
};

}

/* -------------------------------------------------------------------------- */
/* ---- ink/postprocess/SSRPass.h ------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

class SSRPass : public RenderPass {
public:
	int width = 0;                /**< the width of screen */
	int height = 0;               /**< the height of screen */
	int max_steps = 50;           /**< the maximum steps of ray marching */
	float thickness = 0.02;       /**< the thickness of objects on screen */
	float intensity = 0.5;        /**< the intensity of reflections, range is 0 to 1 */
	float max_roughness = 0.8;    /**< the maximum roughness to apply the reflection */
	
	/**
	 * Creates a new SSRPass (Screen Space Reflection) object.
	 */
	explicit SSRPass() = default;
	
	/**
	 * Creates a new SSRPass (Screen Space Reflection) object and initializes it
	 * with parameters.
	 *
	 * \param w the width of screen
	 * \param h the height of screen
	 * \param t the thickness of objects on screen
	 * \param i the intensity of reflections, range is 0 to 1
	 */
	explicit SSRPass(int w, int h, float t = 0.02, float i = 0.5);
	
	/**
	 * Initializes the render pass and prepares the resources for rendering.
	 */
	void init() override;
	
	/**
	 * Compiles the required shaders and renders to the render target.
	 */
	void render() override;
	
	/**
	 * Returns the camera represents the input of rendering pass.
	 */
	const Camera* get_camera() const;
	
	/**
	 * Sets the specified camera as the input of rendering pass.
	 *
	 * \param c camera
	 */
	void set_camera(const Camera* c);
	
	/**
	 * Returns the 2D texture represents the input of rendering pass.
	 */
	const Gpu::Texture* get_texture() const;
	
	/**
	 * Sets the specified 2D texture as the input of rendering pass.
	 *
	 * \param t input texture
	 */
	void set_texture(const Gpu::Texture* t);
	
	/**
	 * Returns the 2D texture represents the world normal buffer in G-Buffers.
	 */
	const Gpu::Texture* get_texture_normal() const;
	
	/**
	 * Sets the specified 2D texture as the world normal buffer in G-Buffers.
	 *
	 * \param n world normal texture
	 */
	void set_texture_normal(const Gpu::Texture* n);
	
	/**
	 * Returns the 2D texture represents the material buffer in G-Buffers.
	 */
	const Gpu::Texture* get_texture_material() const;
	
	/**
	 * Sets the specified 2D texture as the material buffer in G-Buffers.
	 *
	 * \param t material data texture
	 */
	void set_texture_material(const Gpu::Texture* t);
	
	/**
	 * Returns the 2D texture represents the depth buffer / Z-Buffer.
	 */
	const Gpu::Texture* get_texture_depth() const;
	
	/**
	 * Sets the specified 2D texture as the depth buffer / Z-Buffer. Insures the
	 * texture is set to linear filtering.
	 *
	 * \param t depth buffer
	 */
	void set_texture_depth(const Gpu::Texture* t);
	
private:
	const Camera* camera = nullptr;
	
	const Gpu::Texture* map = nullptr;
	const Gpu::Texture* g_normal = nullptr;
	const Gpu::Texture* g_material = nullptr;
	const Gpu::Texture* z_map = nullptr;
};

}

/* -------------------------------------------------------------------------- */
/* ---- ink/postprocess/BloomPass.h ----------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

class BloomPass : public RenderPass {
public:
	int width = 0;            /**< the width of screen */
	int height = 0;           /**< the height of screen */
	float threshold = 1;      /**< the threshold of luminance to affect bloom */
	float intensity = 1;      /**< the intensity of bloom effect */
	float radius = 0.5;       /**< the radius of bloom effect, range is 0 to 1 */
	Vec3 tint = {1, 1, 1};    /**< the tint modify the bloom color */
	
	/**
	 * Creates a new BloomPass object.
	 */
	explicit BloomPass() = default;
	
	/**
	 * Creates a new BloomPass object and initializes it with the specified
	 * parameters.
	 *
	 * \param w the width of screen
	 * \param h the height of screen
	 * \param t the threshold of luminance to affect bloom
	 * \param i the intensity of bloom effect
	 * \param r the radius of bloom effect, range is 0 to 1
	 */
	explicit BloomPass(int w, int h, float t = 1, float i = 1, float r = 1);
	
	/**
	 * Initializes the render pass and prepares the resources for rendering.
	 */
	void init() override;
	
	/**
	 * Compiles the required shaders and renders to the render target.
	 */
	void render() override;
	
	/**
	 * Returns the 2D texture represents the input of rendering pass.
	 */
	const Gpu::Texture* get_texture() const;
	
	/**
	 * Sets the specified 2D texture as the input of rendering pass.
	 *
	 * \param t input texture
	 */
	void set_texture(const Gpu::Texture* t);
	
private:
	const Gpu::Texture* map = nullptr;
	
	std::unique_ptr<Gpu::Texture> bloom_map_1;
	std::unique_ptr<Gpu::Texture> bloom_map_2;
	
	std::unique_ptr<Gpu::RenderTarget> bloom_target;
};

}

/* -------------------------------------------------------------------------- */
/* ---- ink/postprocess/FXAAPass.h ------------------------------------------ */
/* -------------------------------------------------------------------------- */

namespace Ink {

class FXAAPass : public RenderPass {
public:
	/**
	 * Creates a new FXAAPass (Fast Approximate Anti-Aliasing) object.
	 */
	explicit FXAAPass() = default;
	
	/**
	 * Initializes the render pass and prepares the resources for rendering.
	 */
	void init() override;
	
	/**
	 * Compiles the required shaders and renders to the render target.
	 */
	void render() override;
	
	/**
	 * Returns the 2D texture represents the input of rendering pass.
	 */
	const Gpu::Texture* get_texture() const;
	
	/**
	 * Sets the specified 2D texture as the input of rendering pass. Insures the
	 * texture is set to linear filtering.
	 *
	 * \param t input texture
	 */
	void set_texture(const Gpu::Texture* t);
	
private:
	const Gpu::Texture* map = nullptr;
};

}

/* -------------------------------------------------------------------------- */
/* ---- ink/postprocess/GrainPass.h ----------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

class GrainPass : public RenderPass {
public:
	float intensity = 0.5;    /**< the intensity of grain */
	
	/**
	 * Creates a new GrainPass object.
	 */
	explicit GrainPass() = default;
	
	/**
	 * Initializes the render pass and prepares the resources for rendering.
	 */
	void init() override;
	
	/**
	 * Compiles the required shaders and renders to the render target.
	 */
	void render() override;
	
	/**
	 * Returns the 2D texture represents the input of rendering pass.
	 */
	const Gpu::Texture* get_texture() const;
	
	/**
	 * Sets the specified 2D texture as the input of rendering pass.
	 *
	 * \param t input texture
	 */
	void set_texture(const Gpu::Texture* t);
	
private:
	const Gpu::Texture* map = nullptr;
};

}

/* -------------------------------------------------------------------------- */
/* ---- ink/postprocess/ToneMapPass.h --------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

class ToneMapPass : public RenderPass {
public:
	int mode = LINEAR_TONE_MAP;    /**< tone mapping mode */
	float exposure = 1;            /**< tone mapping exposure */
	
	/**
	 * Creates a new ToneMappingPass object.
	 */
	explicit ToneMapPass() = default;
	
	/**
	 * Initializes the render pass and prepares the resources for rendering.
	 */
	void init() override;
	
	/**
	 * Compiles the required shaders and renders to the render target.
	 */
	void render() override;
	
	/**
	 * Returns the 2D texture represents the input of rendering pass.
	 */
	const Gpu::Texture* get_texture() const;
	
	/**
	 * Sets the specified 2D texture as the input of rendering pass.
	 *
	 * \param t input texture
	 */
	void set_texture(const Gpu::Texture* t);
	
private:
	const Gpu::Texture* map = nullptr;
};

}

/* -------------------------------------------------------------------------- */
/* ---- ink/postprocess/ColorGradePass.h ------------------------------------ */
/* -------------------------------------------------------------------------- */

namespace Ink {

class ColorGradePass : public RenderPass {
public:
	Vec3 saturation = {1, 1, 1};    /**< the color purity in color adjustments */
	Vec3 contrast = {1, 1, 1};      /**< the tonal range in color adjustments */
	Vec3 gamma = {1, 1, 1};         /**< the midtones in color adjustments */
	Vec3 gain = {1, 1, 1};          /**< the highlights in color adjustments */
	Vec3 offset = {0, 0, 0};        /**< the shadows in color adjustments */
	
	/**
	 * Creates a new ColorGradingPass object.
	 */
	explicit ColorGradePass() = default;
	
	/**
	 * Initializes the render pass and prepares the resources for rendering.
	 */
	void init() override;
	
	/**
	 * Compiles the required shaders and renders to the render target.
	 */
	void render() override;
	
	/**
	 * Returns the 2D texture represents the input of rendering pass.
	 */
	const Gpu::Texture* get_texture() const;
	
	/**
	 * Sets the specified 2D texture as the input of rendering pass.
	 *
	 * \param t input texture
	 */
	void set_texture(const Gpu::Texture* t);
	
private:
	const Gpu::Texture* map = nullptr;
};

}


/* -------------------------------------------------------------------------- */
/* ---- ink/audio/Audio.h --------------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

class Audio {
public:
	/**
	 * Initializes SDL audio system before using this class.
	 */
	static void init();
	
	/**
	 * Creates a new Audio object and loads audio data from the specified WAVE
	 * file into the audio.
	 *
	 * \param p the path to the file
	 */
	explicit Audio(const std::string& p);
	
	/**
	 * Deletes this Audio object and releases resources.
	 */
	~Audio();
	
	/**
	 * The copy constructor is deleted.
	 */
	Audio(const Audio&) = delete;
	
	/**
	 * The copy assignment operator is deleted.
	 */
	Audio& operator=(const Audio&) = delete;
	
	/**
	 * Starts the audio playback.
	 */
	void play() const;
	
	/**
	 * Pauses the audio playback.
	 */
	void pause() const;
	
	/**
	 * Stops the audio playback. The position of the audio will be set to 0.
	 */
	void stop();
	
	/**
	 * Returns the duration of the audio in seconds.
	 */
	float get_duration() const;
	
	/**
	 * Returns true if the audio must be replayed when the end of the audio is
	 * reached.
	 */
	bool get_loop() const;
	
	/**
	 * Determines whether the audio must be replayed when the end of the audio
	 * is reached. Default is false.
	 *
	 * \param l looping
	 */
	void set_loop(bool l);
	
	/**
	 * Returns the volume of the audio. Range is 0 to 1.
	 */
	float get_volume() const;
	
	/**
	 * Sets the specified volume of the audio. Range is 0 to 1. Default is 1.
	 *
	 * \param v volume
	 */
	void set_volume(float v);
	
	/**
	 * Returns the position indicates in seconds the current point that is being
	 * played in the audio.
	 */
	float get_position() const;
	
	/**
	 * Sets the position indicates in seconds the current point that is being
	 * played in the audio.
	 *
	 * \param p position
	 */
	void set_position(float p);
	
private:
	bool loop = false;
	float volume = 1;
	float ratio = 0;
	uint32_t position = 0;
	uint32_t length = 0;
	uint8_t* buffer = nullptr;
	
	SDL_AudioSpec spec;
	SDL_AudioDeviceID device;
};

}

/* -------------------------------------------------------------------------- */
/* ---- ink/window/Window.h ------------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

class Window {
public:
	using EventCallback = std::function<bool(const SDL_Event&)>;
	
	/**
	 * Initializes Window with title, positions, size and DPI.
	 *
	 * \param t the title of window
	 * \param x the x-coordinate of the window's position
	 * \param y the y-coordinate of the window's position
	 * \param w the width of window
	 * \param h the height of window
	 * \param d whether to enable high-dpi mode
	 */
	static void init(const std::string& t, int x, int y, int w, int h, bool d = false);
	
	/**
	 * Initializes OpenGL after the window is initialized.
	 *
	 * \param v vertical sync mode
	 * \param d the size of depth buffer
	 * \param s the size of stencil buffer
	 * \param m the number of samples in MSAA
	 * \param a whether to enable hardware acceleration
	 */
	static void init_opengl(int v = 1, int d = 24, int s = 8, int m = 0, bool a = true);
	
	/**
	 * Closes window and releases all resources.
	 */
	static void close();
	
	/**
	 * Handles the window's events and refreshes the display of window.
	 */
	static void update();
	
	/**
	 * Returns true if the window is opening.
	 */
	static bool is_open();
	
	/**
	 * Returns the number of milliseconds scene the window is initialized.
	 */
	static unsigned int get_time();
	
	/**
	 * Returns the title of the window.
	 */
	static std::string get_title();
	
	/**
	 * Sets the specified title of the window.
	 */
	static void set_title(const std::string& t);
	
	/**
	 * Returns the size of a window's client area.
	 */
	static std::pair<int, int> get_size();
	
	/**
	 * Returns the coordinates represents the position of the window.
	 */
	static std::pair<int, int> get_position();
	
	/**
	 * Sets the coordinates represents the position of the window.
	 *
	 * \param x the X coordinate of position
	 * \param y the Y coordinate of position
	 */
	static void set_position(int x, int y);
	
	/**
	 * Returns the interval time. If the time update function spends is less
	 * than interval time, the program will wait for the time difference.
	 */
	static uint32_t get_interval();
	
	/**
	 * Sets the specified interval time. If the time update function spends is
	 * less than interval time, the program will wait for the time difference.
	 *
	 * \param i interval time
	 */
	static void set_interval(uint32_t i);
	
	/**
	 * Sets the border state of the window.
	 *
	 * \param b borderless
	 */
	static void set_borderless(bool b);
	
	/**
	 * Sets the user-resizable state of the window.
	 *
	 * \param r resizable
	 */
	static void set_resizable(bool r);
	
	/**
	 * Sets the minimum size of the window's client area.
	 *
	 * \param w minimum width
	 * \param h minimum height
	 */
	static void set_min_size(int w, int h);
	
	/**
	 * Enters or exits fullscreen state. The fullscreen mode is desktop.
	 *
	 * \param f fullscreen state
	 */
	static void set_fullscreen(bool f);
	
	/**
	 * Makes a window as large as possible.
	 */
	static void maximize();
	
	/**
	 * Minimizes a window to an iconic representation.
	 */
	static void minimize();
	
	/**
	 * Returns the coordinates represents the position of cursor.
	 */
	static std::pair<int, int> get_cursor_position();
	
	/**
	 * Sets the coordinates represents the position of cursor.
	 *
	 * \param x the x coordinate of cursor
	 * \param y the y coordinate of cursor
	 */
	static void set_cursor_position(int x, int y);
	
	/**
	 * Sets the specified visibility of the cursor.
	 *
	 * \param v visibility
	 */
	static void set_cursor_visible(bool v);
	
	/**
	 * Sets the lock state of the cursor. The cursor will be fixed at the center
	 * of window if lock is enabled.
	 *
	 * \param l lock
	 */
	static void set_cursor_locked(bool l);
	
	/**
	 * Returns true if the specified key is down. The key code of left mouse
	 * button is 1 and right mouse button is 2.
	 *
	 * \param k key code
	 */
	static bool is_down(unsigned int k);
	
	/**
	 * Returns true if the specified key is pressed. The key code of left mouse
	 * button is 1 and right mouse button is 2.
	 *
	 * \param k key code
	 */
	static bool is_pressed(unsigned int k);
	
	/**
	 * Returns true if the specified key is released. The key code of left mouse
	 * button is 1 and right mouse button is 2.
	 *
	 * \param k key code
	 */
	static bool is_released(unsigned int k);
	
	/**
	 * Sets the event callback which will be called to handle window events.
	 * Window will discard events if callback returns true.
	 *
	 * \param f event callback function
	 */
	static void set_event_callback(const EventCallback& f);
	
private:
	static bool open;
	static int cursor_x;
	static int cursor_y;
	static bool cursor_locked;
	static bool ignore_cursor_motion;
	
	static uint32_t time;
	static uint32_t interval;
	
	static EventCallback event_callback;
	
	static SDL_Window* sdl_window;
	static SDL_GLContext context;
	
	static bool keydown[512];
	static bool keypressed[512];
	static bool keyreleased[512];
};

}

/* -------------------------------------------------------------------------- */
/* ---- ink/utils/ConvexHull.h ---------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

class ConvexHull {
public:
	/**
	 * Creates a new ConvexHull object.
	 */
	explicit ConvexHull() = default;
	
	/**
	 * Adds the specified vertex to convex hull.
	 *
	 * \param v vertex
	 */
	void add_vertex(const Vec3& v);
	
	/**
	 * Returns the number of vertices in convex hull.
	 */
	size_t get_vertex_count() const;
	
	/**
	 * Returns the vertex at the specified index in convex hull.
	 *
	 * \param i vertex index
	 */
	Vec3 get_vertex(int i) const;
	
	/**
	 * Returns the number of faces in convex hull.
	 */
	size_t get_face_count() const;
	
	/**
	 * Returns the face at the specified index in convex hull.
	 *
	 * \param i face index
	 */
	std::array<int, 3> get_face(int i) const;
	
	/**
	 * Starts the execution of convex hull algorithm.
	 */
	void compute();
	
private:
	std::vector<Vec3> vertices;
	std::vector<Vec3> normals;
	std::vector<std::array<int, 3> > faces;
	
	void insert_face(int a, int b, int c);
};

}

/* -------------------------------------------------------------------------- */
/* ---- ink/utils/Viewer.h -------------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

enum ViewerMode {
	VIEWER_WALK,
	VIEWER_FLY,
};

class Viewer {
public:
	ViewerMode mode = VIEWER_FLY;    /**< movement mode */
	float speed = 1;                 /**< moving speed */
	float sensitivity = 0.001;       /**< mouse sensitivity */
	
	int key_up = SDLK_w;             /**< the key controls camera move forward */
	int key_down = SDLK_s;           /**< the key controls camera move back */
	int key_left = SDLK_a;           /**< the key controls camera move left */
	int key_right = SDLK_d;          /**< the key controls camera move right */
	
	/**
	 * Creates a new Viewer object and initializes it with camera and moving
	 * speed.
	 *
	 * \param c camera
	 * \param s moving speed
	 */
	explicit Viewer(Camera* c = nullptr, float s = 1);
	
	/**
	 * Updates the viewing camera. This function should be called every frame.
	 *
	 * \param dt delta time
	 */
	void update(float dt);
	
	/**
	 * Returns the viewing camera. The camera will be updated after calling the
	 * update function.
	 */
	Camera* get_camera() const;
	
	/**
	 * Sets the specified viewing camera. All the parameters of camera will be
	 * updated.
	 *
	 * \param c camera
	 */
	void set_camera(Camera* c);
	
	/**
	 * Sets the specified position parameter of viewing camera.
	 *
	 * \param p position
	 */
	void set_position(const Vec3& p);
	
	/**
	 * Sets the specified direction parameter of viewing camera.
	 *
	 * \param d direction
	 */
	void set_direction(const Vec3& d);
	
private:
	float axis_y = 0;
	float axis_z = 0;
	
	Camera* camera = nullptr;
};

}

/* -------------------------------------------------------------------------- */
/* ---- ink/utils/ImageUtils.h ---------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

class ImageUtils {
public:
	/**
	 * Samples the image with UV coordinates by nearest interpolation.
	 *
	 * \param i image
	 * \param c channel
	 * \param u U coordinate
	 * \param v V coordinate
	 */
	static float nearest_sample(const Image& i, int c, float u, float v);
	
	/**
	 * Samples the image with UV coordinates by nearest interpolation.
	 *
	 * \param i image
	 * \param c channel
	 * \param uv UV coordinates
	 */
	static float nearest_sample(const Image& i, int c, const Vec2& uv);
	
	/**
	 * Samples the image with UV coordinates by linear interpolation.
	 *
	 * \param i image
	 * \param c channel
	 * \param u U coordinate
	 * \param v V coordinate
	 */
	static float linear_sample(const Image& i, int c, float u, float v);
	
	/**
	 * Samples the image with UV coordinates by linear interpolation.
	 *
	 * \param i image
	 * \param c channel
	 * \param uv UV coordinates
	 */
	static float linear_sample(const Image& i, int c, const Vec2& uv);
};

}

/* -------------------------------------------------------------------------- */
/* ---- ink/utils/ColorUtils.h ---------------------------------------------- */
/* -------------------------------------------------------------------------- */

namespace Ink {

class ColorUtils {
public:
	/**
	 * Transforms from hex color code to RGB color.
	 *
	 * \param c hex color code
	 */
	static Vec3 to_rgb(unsigned int c);
	
	/**
	 * Transforms from hex color code to RGBA color.
	 *
	 * \param c hex color code
	 */
	static Vec4 to_rgba(unsigned int c);
	
	/**
	 * Transforms from RGB color to hex color code.
	 *
	 * \param c RGB color
	 */
	static unsigned int to_hex(const Vec3& c);
	
	/**
	 * Transforms from RGBA color to hex color code.
	 *
	 * \param c RGBA color
	 */
	static unsigned int to_hex(const Vec4& c);
};

}
