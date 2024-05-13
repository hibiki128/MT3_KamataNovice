#pragma once
#include"Vector3.h"
#include"Matrix/M4x4.h"
#include <Novice.h>
#include "cmath"
#include"numbers"

struct Sphere {
	Vector3 center; // !< 中心点
	float radius;   // !< 半径
};

void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix);

void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMarix, uint32_t color);