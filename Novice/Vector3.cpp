#include"Vector3.h"

// 加算
Vector3 Vector3::operator+(const Vector3& obj) const { return Vector3(x + obj.x, y + obj.y, z + obj.z); }

// 減算
Vector3 Vector3::operator-(const Vector3& obj) const { return Vector3(x - obj.x, y - obj.y, z - obj.z); }

// 乗算
Vector3 Vector3::operator*(const Vector3& obj) const { return Vector3(x * obj.x, y * obj.y, z * obj.z); }

// 乗算(スカラー倍)
Vector3 Vector3::operator*(const float& scalar) const { return Vector3(x * scalar, y * scalar, z * scalar); }

// +=
Vector3& Vector3::operator+=(const Vector3& other) {
	x += other.x;
	y += other.y;
	z += other.z;
	return *this;
}

// 内積
float Vector3::Dot(const Vector3& v1, const Vector3& v2) { return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z; }

// ベクトルの長さ
float Vector3::Length() const { return std::sqrt(x * x + y * y + z * z); }

// 正規化
Vector3 Vector3::Normalize() const {
	float length = Length();
	if (length != 0) {
		return *this * (1.0f / length);
	}
	return *this;
}

// 外積
Vector3 Vector3::Cross(const Vector3& v1, const Vector3& v2) { return Vector3(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x); }