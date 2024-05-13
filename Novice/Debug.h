#pragma once
#include"Vector3.h"
#include"Matrix/M4x4.h"
#include <Novice.h>
#include "cmath"
#include"numbers"

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

void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix);

void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMarix, uint32_t color);

Vector3 Project(const Vector3& v1, const Vector3& v2);
// 線分と点の最近接点を計算する関数
Vector3 ClosestPoint(const Vector3& point, const Segment& segment);