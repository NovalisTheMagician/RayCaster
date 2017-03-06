#ifndef _VECTOR_H
#define _VECTOR_H

#include <math.h>

struct vec2_t
{
	float x;
	float y;

	vec2_t() : x(0), y(0) {}
	vec2_t(float x, float y) : x(x), y(y) {}
	vec2_t(float s) : x(s), y(s) {}
	vec2_t(const vec2_t& rhs)
	{
		x = rhs.x;
		y = rhs.y;
	}

	inline vec2_t& operator=(const vec2_t& rhs)
	{
		if(this == &rhs)
			return *this;

		x = rhs.x;
		y = rhs.y;
		return *this;
	}

	inline vec2_t& operator+=(const vec2_t& rhs)
	{
		x += rhs.x;
		y += rhs.y;
		return *this;
	}

	inline vec2_t& operator-=(const vec2_t& rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
		return *this;
	}

	inline vec2_t& operator*=(const float& rhs)
	{
		x *= rhs;
		y *= rhs;
		return *this;
	}

	inline vec2_t& operator/=(const float& rhs)
	{
		x /= rhs;
		y /= rhs;
		return *this;
	}

	inline float dot(const vec2_t& other) const
	{
		return (x * other.x + y * other.y);
	}

	inline float length() const
	{
		return std::sqrt(x * x + y * y);
	}

	inline vec2_t perp() const
	{
		vec2_t res;
		res.x = -y;
		res.y = x;
		return res;
	}

};

struct vec3_t
{
	float x;
	float y;
	float z;

	vec3_t() : x(0), y(0), z(0) {}
	vec3_t(float x, float y, float z) : x(x), y(y), z(z) {}
	vec3_t(float s) : x(s), y(s), z(s) {}
	vec3_t(const vec3_t& rhs)
	{
		x = rhs.x;
		y = rhs.y;
		z = rhs.z;
	}

	inline vec3_t& operator=(const vec3_t& rhs)
	{
		if(this == &rhs)
			return *this;

		x = rhs.x;
		y = rhs.y;
		return *this;
	}

	inline vec3_t& operator+=(const vec3_t& rhs)
	{
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
		return *this;
	}

	inline vec3_t& operator-=(const vec3_t& rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;
		return *this;
	}

	inline vec3_t& operator*=(const float& rhs)
	{
		x *= rhs;
		y *= rhs;
		z *= rhs;
		return *this;
	}

	inline vec3_t& operator/=(const float& rhs)
	{
		x /= rhs;
		y /= rhs;
		z /= rhs;
		return *this;
	}

	inline float dot(const vec3_t& other) const
	{
		return (x * other.x + y * other.y + z * other.z);
	}

	inline float length() const
	{
		return std::sqrt(x * x + y * y + z * z);
	}

	inline vec3_t cross(const vec3_t& other) const
	{
		vec3_t res;
		res.x = y * other.z - z * other.y;
		res.y = z * other.x - x * other.z;
		res.z = x * other.y - y * other.x;
		return res;
	}

};

inline vec3_t operator+(const vec3_t& rhs, const vec3_t& lhs)
{
	vec3_t res;
	res.x = rhs.x + lhs.x;
	res.y = rhs.y + lhs.y;
	res.z = rhs.z + lhs.z;
	return res;
}

inline vec3_t operator-(const vec3_t& rhs, const vec3_t& lhs)
{
	vec3_t res;
	res.x = rhs.x + lhs.x;
	res.y = rhs.y + lhs.y;
	res.z = rhs.z + lhs.z;
	return res;
}

inline vec3_t operator*(const vec3_t& rhs, float lhs)
{
	vec3_t res;
	res.x = rhs.x * lhs;
	res.y = rhs.y * lhs;
	res.z = rhs.z * lhs;
	return res;
}

inline vec3_t operator/(const vec3_t& rhs, float lhs)
{
	vec3_t res;
	res.x = rhs.x / lhs;
	res.y = rhs.y / lhs;
	res.z = rhs.z / lhs;
	return res;
}

inline vec2_t operator+(const vec2_t& rhs, const vec2_t& lhs)
{
	vec2_t res;
	res.x = rhs.x + lhs.x;
	res.y = rhs.y + lhs.y;
	return res;
}

inline vec2_t operator-(const vec2_t& rhs, const vec2_t& lhs)
{
	vec2_t res;
	res.x = rhs.x + lhs.x;
	res.y = rhs.y + lhs.y;
	return res;
}

inline vec2_t operator*(const vec2_t& rhs, float lhs)
{
	vec2_t res;
	res.x = rhs.x * lhs;
	res.y = rhs.y * lhs;
	return res;
}

inline vec2_t operator/(const vec2_t& rhs, float lhs)
{
	vec2_t res;
	res.x = rhs.x / lhs;
	res.y = rhs.y / lhs;
	return res;
}

#endif
