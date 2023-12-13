#pragma once
#include <math.h>
#include <string>


const float EPSILON = 0.00001;
uint32_t state = 727;

float random_value(){
	state = state * 747796405 + 2891336453;
	uint32_t result = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
	result = (result >> 22) ^ result;
	return ((float)result) / UINT32_MAX;
}


struct Vector3{
    struct{
        float x, y, z;
    };

	constexpr inline float& operator[](int ind){
        if (ind == 0){
			return x;
		}
		else if (ind == 1){
			return y;
		}
		return z;
    }

    constexpr inline const float& operator[](int ind) const {
        if (ind == 0){
			return x;
		}
		else if (ind == 1){
			return y;
		}
		return z;
    }

    inline Vector3() : x(0.0f), y(0.0f), z(0.0f){}
	inline Vector3(float f) : x(f), y(f), z(f){}
	inline Vector3(float x, float y, float z) : x(x), y(y), z(z){}
	inline Vector3(const Vector3& vector) : x(vector.x), y(vector.y), z(vector.z){}

    inline static float length_squared(const Vector3 & vector) {
		return dot(vector, vector);
	}

	inline static float length(const Vector3 & vector) {
		return sqrtf(length_squared(vector));
	}

	inline static Vector3 normalize(const Vector3 & vector) {
		float inv_length = 1.0f / length(vector);
		return Vector3(
			vector.x * inv_length,
			vector.y * inv_length,
			vector.z * inv_length
		);
	}

	inline static float dot(const Vector3& left, const Vector3& right) {
		return left.x * right.x + left.y * right.y + left.z * right.z;
	}

	inline static Vector3 cross(const Vector3& left, const Vector3& right) {
		return Vector3(
			left.y * right.z - left.z * right.y,
			left.z * right.x - left.x * right.z,
			left.x * right.y - left.y * right.x
		);
	}

	inline static Vector3 min(const Vector3& left, const Vector3& right) {
		return Vector3(
			left.x < right.x ? left.x : right.x,
			left.y < right.y ? left.y : right.y,
			left.z < right.z ? left.z : right.z
		);
	}

	inline static Vector3 max(const Vector3& left, const Vector3& right) {
		return Vector3(
			left.x > right.x ? left.x : right.x,
			left.y > right.y ? left.y : right.y,
			left.z > right.z ? left.z : right.z
		);
	}

	inline static Vector3 power(const Vector3& v, float e){
		return Vector3(pow(v.x, e),pow(v.y, e), pow(v.z, e));
	}

	inline static Vector3 invert(const Vector3& v){
		return Vector3(1/v.x,1/v.y,1/v.z);
	}


	template<typename Func>
	inline static Vector3 apply(const Vector3& v, const Func& f = {}) {
		return Vector3(f(v.x), f(v.y), f(v.z));
	}

	inline static Vector3 to_colour(std::string hexstring){
		float r = ((float)std::stoi(hexstring.substr(1, 2), nullptr, 16))/255;
		float g = ((float)std::stoi(hexstring.substr(3, 2), nullptr, 16))/255;
		float b = ((float)std::stoi(hexstring.substr(5, 2), nullptr, 16))/255;
		return Vector3(r, g, b);
	}

	inline static float min_component(const Vector3& v){
		return fmin(fmin(v.x, v.y), v.z);
	}

	inline static float max_component(const Vector3& v){
		return fmax(fmax(v.x, v.y), v.z);
	}

	inline static Vector3 abs(const Vector3& v){
		return Vector3(fabs(v.x), fabs(v.y), fabs(v.z));
	}



	inline Vector3 operator+=(const Vector3 & vector) { x += vector.x; y += vector.y; z += vector.z; return *this; }
	inline Vector3 operator-=(const Vector3 & vector) { x -= vector.x; y -= vector.y; z -= vector.z; return *this; }
	inline Vector3 operator*=(const Vector3 & vector) { x *= vector.x; y *= vector.y; z *= vector.z; return *this; }
	inline Vector3 operator/=(const Vector3 & vector) { x /= vector.x; y /= vector.y; z /= vector.z; return *this; }

	inline Vector3 operator+=(float scalar) {                                   x += scalar;     y += scalar;     z += scalar;     return *this; }
	inline Vector3 operator-=(float scalar) {                                   x -= scalar;     y -= scalar;     z -= scalar;     return *this; }
	inline Vector3 operator*=(float scalar) {                                   x *= scalar;     y *= scalar;     z *= scalar;     return *this; }
	inline Vector3 operator/=(float scalar) { float inv_scalar = 1.0f / scalar; x *= inv_scalar; y *= inv_scalar; z *= inv_scalar; return *this; }
};

inline Vector3 operator-(const Vector3 & vector) { return Vector3(-vector.x, -vector.y, -vector.z); }

inline Vector3 operator+(const Vector3 & left, const Vector3 & right) { return Vector3(left.x + right.x, left.y + right.y, left.z + right.z); }
inline Vector3 operator-(const Vector3 & left, const Vector3 & right) { return Vector3(left.x - right.x, left.y - right.y, left.z - right.z); }
inline Vector3 operator*(const Vector3 & left, const Vector3 & right) { return Vector3(left.x * right.x, left.y * right.y, left.z * right.z); }
inline Vector3 operator/(const Vector3 & left, const Vector3 & right) { return Vector3(left.x / right.x, left.y / right.y, left.z / right.z); }

inline Vector3 operator+(const Vector3 & vector, float scalar) {                                   return Vector3(vector.x + scalar,     vector.y + scalar,     vector.z + scalar); }
inline Vector3 operator-(const Vector3 & vector, float scalar) {                                   return Vector3(vector.x - scalar,     vector.y - scalar,     vector.z - scalar); }
inline Vector3 operator*(const Vector3 & vector, float scalar) {                                   return Vector3(vector.x * scalar,     vector.y * scalar,     vector.z * scalar); }
inline Vector3 operator/(const Vector3 & vector, float scalar) { float inv_scalar = 1.0f / scalar; return Vector3(vector.x * inv_scalar, vector.y * inv_scalar, vector.z * inv_scalar); }

inline Vector3 operator+(float scalar, const Vector3 & vector) { return Vector3(scalar + vector.x, scalar + vector.y, scalar + vector.z); }
inline Vector3 operator-(float scalar, const Vector3 & vector) { return Vector3(scalar - vector.x, scalar - vector.y, scalar - vector.z); }
inline Vector3 operator*(float scalar, const Vector3 & vector) { return Vector3(scalar * vector.x, scalar * vector.y, scalar * vector.z); }
inline Vector3 operator/(float scalar, const Vector3 & vector) { return Vector3(scalar / vector.x, scalar / vector.y, scalar / vector.z); }

inline bool operator==(const Vector3 & left, const Vector3 & right) { return left.x == right.x && left.y == right.y && left.z == right.z; }
inline bool operator!=(const Vector3 & left, const Vector3 & right) { return left.x != right.x || left.y != right.y || left.z != right.z; }



inline Vector3 random_unit_vector(){
	Vector3 ret = Vector3(0);
	while(true){
		ret.x = random_value() * 2 - 1;
		ret.y = random_value() * 2 - 1; 
		ret.z = random_value() * 2 - 1;
		float length = Vector3::length_squared(ret);
		if (length < 1){
			return ret / sqrtf(length);
		}
	}
}

inline Vector3 random_hemisphere_vector(Vector3 normal){
	Vector3 dir = random_unit_vector();
	return (Vector3::dot(dir, normal) < 0) ? dir * -1: dir;
}

inline std::ostream & operator<<(std::ostream & stream, const Vector3 & vector) {
	stream << "(" << vector.x << ", " << vector.y << ", " << vector.z << ")";
	return stream;
}