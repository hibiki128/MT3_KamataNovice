#pragma once
#define NOMINMAX 
#define DELTA_TIME (1.0f / 60.0f)
#include "cmath"
#include "Matrix/MyMath.h"
#include "numbers"
#include "Vector2.h"
#include "Vector3.h"
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

struct OBB {
	Vector3 center;          //!< 中心点
	Vector3 orientations[3]; //!< 座標軸。正規化・直行必須
	Vector3 size;            //!< 座標軸方向の長さの半分。中心から面までの距離
};

struct Spring {
	Vector3 anchor;
	float naturalLength;
	float stiffness;
	float dampingCoefficient;
};

struct Ball {
	Vector3 position;
	Vector3 velocity;
	Vector3 acceleration;
	float mass;
	float radius;
	unsigned int color;
};

struct Pendulum {
	Vector3 anchor;
	float length;
	float angle;
	float angularVelocity;
	float angularAcceleration;
};

struct ConicalPendulum {
	Vector3 anchor;        // アンカーポイント。固定された端の位置
	float length;          // 紐の長さ
	float halfApexAngle;   // 円錐の直角の半分
	float angle;           // 現在の角度
	float angularVelocity; // 角速度ω
};

// グリッド描画
void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, const float GridHalfWidth, const uint32_t Subdivision);

// 球描画
void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMarix, uint32_t color);

// 球描画
void DrawBall(const Ball& ball, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMarix, uint32_t color);

// 平面描画
void DrawPlane(const Plane& plane, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);

// 三角形描画
void DrawTriangle(const Triangle& triangle, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);

// 線の描画(XYの場合)
void DrawLineXY(const Vector3& start, const Vector3& end, uint32_t color);

// AABBの描画
void DrawAABB(const AABB& aabb, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);

// OBBの描画
void DrawOBB(const OBB& obb, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);

// ベジェ曲線の描画
void DrawBezier(const Vector3& controlPoint0, const Vector3& controlPoint1, const Vector3& controlPoint2, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);

// 線を描画する関数
void DrawLineBetweenSpheres(const Vector3& point1, const Vector3& point2, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);

void SetAABB(AABB& aabb);

Vector3 Project(const Vector3& v1, const Vector3& v2);

Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t);

// 最近接点
Vector3 ClosestPoint(const Vector3& point, const Segment& segment);

Vector3 Perpendicular(const Vector3& vector);

// 衝突判定関数の実装
Matrix4x4 MakeOBBWorldMatrix(const OBB& obb, const Matrix4x4& rotateMatrix);

AABB ConvertOBBToAABB(const OBB& obb);
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

// AABBと線分の衝突判定
bool IsCollision(const AABB& aabb, const Segment& segment);

// OBBと球体の当たり判定
bool IsCollision(const OBB& obb, const Sphere& sphere, const Matrix4x4& rotateMatrix);

// OBBと線の衝突判定
bool IsCollision(const OBB& obb, const Segment& segment, const Matrix4x4& rotateMatrix);

// ばねの動き
void SpringMove(Spring& spring, Ball& ball, const Vector3& Gravity);

// 円運動
void CircularMotion(Vector3& p, Vector3& c, const float& r, float& angularVelocity, float& angle);

// 振り子運動
void PendulumMovement(Pendulum& pendulum, Vector3& p);

// 円錐振り子運動
void ConicalPendulumMove(ConicalPendulum& conicalPendulum, Ball& ball);