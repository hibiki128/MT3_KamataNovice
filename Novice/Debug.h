#pragma once
#include "Matrix/M4x4.h"
#include "Vector2.h"
#include "Vector3.h"
#include "cmath"
#include "numbers"
#include <Novice.h>

struct Line {
	Vector3 origin; //!< 始点
	Vector3 diff;   //!< 終点への差分ベクトル
};

struct Ray {
	Vector3 origin; //!< 始点
	Vector3 diff;   //!< 終点への差分ベクトル
};

struct Segment {
	Vector3 origin; //!< 始点
	Vector3 diff;   //!< 終点への差分ベクトル
};

struct Sphere {
	Vector3 center; // !< 中心点
	float radius;   // !< 半径
};

struct Plane {
	Vector3 normal; //!< 法線
	float distance; //!< 距離
};

struct Triangle {
	Vector3 vertices[3]; //!< 頂点
};

struct AABB {
	Vector3 min; //!< 最小点
	Vector3 max; //!< 最大点
};

// グリッド描画
void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, const float GridHalfWidth, const uint32_t Subdivision);

// 球描画
void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMarix, uint32_t color);

// 平面描画
void DrawPlane(const Plane& plane, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);

// 三角形描画
void DrawTriangle(const Triangle& triangle, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);

// 線の描画(XYの場合)
void DrawLineXY(const Vector3& start, const Vector3& end, uint32_t color);

// AABBの描画
void DrawAABB(const AABB& aabb, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);

void SetAABB(AABB& aabb);

Vector3 Project(const Vector3& v1, const Vector3& v2);

// 最近接点
Vector3 ClosestPoint(const Vector3& point, const Segment& segment);

Vector3 Perpendicular(const Vector3& vector);

// カメラ移動
void CameraMove(Vector3& cameraRotate, Vector3& cameraTranslate, Vector2Int& clickPosition, char* keys, char* preKeys);

// 球同士の衝突判定
bool IsCollision(const Sphere& s1, const Sphere& s2);

// 球と平面の衝突判定
bool IsCollision(const Sphere& sphere, const Plane& plane);

// 線分と平面の衝突判定
bool IsCollision(Segment& line, Plane& plane);

// 三角形と線分の衝突判定
bool IsCollision(const Triangle& triangle, const Segment& segment);

// AABB同士の衝突判定
bool IsCollision(const AABB& aabb1, const AABB& aabb2);

// AABBと球体の衝突判定
bool IsCollision(const AABB& aabb, const Sphere& sphere);
