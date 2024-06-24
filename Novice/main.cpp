#include "Debug.h"
#include "Matrix/MyMath.h"
#include "Matrix4x4.h"
#include "Vector2.h"
#include "algorithm"
#include "imgui.h"
#include <Novice.h>

const char kWindowTitle[] = "LE2B_20_ハギワラ_ヒビキ";

// 軸に対するOBBの投影範囲を計算する関数
void projectOBB(const OBB& obb, const Vector3& axis, float& min, float& max) {
	float centerProjection = Dot(obb.center, axis);
	float radius = std::abs(Dot(obb.orientations[0], axis)) * obb.size.x + std::abs(Dot(obb.orientations[1], axis)) * obb.size.y + std::abs(Dot(obb.orientations[2], axis)) * obb.size.z;

	min = centerProjection - radius;
	max = centerProjection + radius;
}

// 軸に投影するための関数
bool testAxis(const Vector3& axis, const OBB& obb1, const OBB& obb2) {
	float min1, max1, min2, max2;
	projectOBB(obb1, axis, min1, max1);
	projectOBB(obb2, axis, min2, max2);

	float sumSpan = (max1 - min1) + (max2 - min2);
	float longSpan = std::max(max1, max2) - std::min(min1, min2);

	return sumSpan >= longSpan;
}

// OBB同士の衝突判定
bool IsCollision(const OBB& obb1, const OBB& obb2) {
	Vector3 axes[15] = {
	    obb1.orientations[0],
	    obb1.orientations[1],
	    obb1.orientations[2],
	    obb2.orientations[0],
	    obb2.orientations[1],
	    obb2.orientations[2],
	    Cross(obb1.orientations[0], obb2.orientations[0]),
	    Cross(obb1.orientations[0], obb2.orientations[1]),
	    Cross(obb1.orientations[0], obb2.orientations[2]),
	    Cross(obb1.orientations[1], obb2.orientations[0]),
	    Cross(obb1.orientations[1], obb2.orientations[1]),
	    Cross(obb1.orientations[1], obb2.orientations[2]),
	    Cross(obb1.orientations[2], obb2.orientations[0]),
	    Cross(obb1.orientations[2], obb2.orientations[1]),
	    Cross(obb1.orientations[2], obb2.orientations[2]),
	};

	for (const Vector3& axis : axes) {
		if (Length(axis) > 0.0001f && !testAxis(Normalize(axis), obb1, obb2)) {
			return false;
		}
	}

	return true;
}
void MakeOBBOrientations(OBB& obb, Vector3& rotate) {
	Matrix4x4 rotateMatrix = MakeRotateXMatrix(rotate.x) * (MakeRotateYMatrix(rotate.y) * MakeRotateZMatrix(rotate.z));
	obb.orientations[0].x = rotateMatrix.m[0][0];
	obb.orientations[0].y = rotateMatrix.m[0][1];
	obb.orientations[0].z = rotateMatrix.m[0][2];

	obb.orientations[1].x = rotateMatrix.m[1][0];
	obb.orientations[1].y = rotateMatrix.m[1][1];
	obb.orientations[1].z = rotateMatrix.m[1][2];

	obb.orientations[2].x = rotateMatrix.m[2][0];
	obb.orientations[2].y = rotateMatrix.m[2][1];
	obb.orientations[2].z = rotateMatrix.m[2][2];
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);
	Vector3 cameraTranslate{0.0f, 1.9f, -6.49f};
	Vector3 cameraRotate{0.26f, 0.0f, 0.0f};
	Vector3 cameraPosition{0.0f, 1.0f, -5.0f};
	uint32_t color = WHITE;
	bool isHit = false;
	Vector2Int ClickPosition = {};
	const int kWindowWidth = 1280;
	const int kWindowHeight = 720;

	Vector3 rotate1{0.0f, 0.0f, 0.0f};
	Vector3 rotate2{-0.05f, -2.49f, 0.15f};
	OBB obb1{
	    .center{0.0f,               0.0f,               0.0f              },
        .orientations = {{1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        .size{0.83f,              0.26f,              0.24f             }
    };
	OBB obb2{
	    .center{0.9f,               0.66f,              0.78f             },
        .orientations = {{1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        .size{0.5f,               0.37f,              0.5f              }
    };

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///

		Matrix4x4 cameraMatrix = MakeAffineMatrix({1.0f, 1.0f, 1.0f}, cameraRotate, cameraPosition + cameraTranslate);
		Matrix4x4 viewMatrix = Inverse(cameraMatrix);
		Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(kWindowWidth) / float(kWindowHeight), 0.1f, 100.0f);
		Matrix4x4 viewProjectionMatrix = Multiply(viewMatrix, projectionMatrix);
		Matrix4x4 viewportMatrix = MakeViewPortMatrix(0, 0, float(kWindowWidth), float(kWindowHeight), 0.0f, 1.0f);

		CameraMove(cameraRotate, cameraTranslate, ClickPosition, keys, preKeys);

		MakeOBBOrientations(obb1, rotate1);
		MakeOBBOrientations(obb2, rotate2);

		isHit = IsCollision(obb1, obb2);

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		ImGui::Begin("Window");
		if (ImGui::TreeNode("OBB1")) {
			ImGui::DragFloat3("obb.center", &obb1.center.x, 0.01f);
			ImGui::SliderAngle("rotateX", &rotate1.x);
			ImGui::SliderAngle("rotateY", &rotate1.y);
			ImGui::SliderAngle("rotateZ", &rotate1.z);
			ImGui::DragFloat3("obb.orientations", &obb1.orientations[0].x, 0.01f);
			ImGui::DragFloat3("obb.orientations", &obb1.orientations[1].x, 0.01f);
			ImGui::DragFloat3("obb.orientations", &obb1.orientations[2].x, 0.01f);
			ImGui::DragFloat3("obb.size", &obb1.size.x, 0.01f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("OBB2")) {
			ImGui::DragFloat3("obb.center", &obb2.center.x, 0.01f);
			ImGui::SliderAngle("rotateX", &rotate2.x);
			ImGui::SliderAngle("rotateY", &rotate2.y);
			ImGui::SliderAngle("rotateZ", &rotate2.z);
			ImGui::DragFloat3("obb.orientations", &obb2.orientations[0].x, 0.01f);
			ImGui::DragFloat3("obb.orientations", &obb2.orientations[1].x, 0.01f);
			ImGui::DragFloat3("obb.orientations", &obb2.orientations[2].x, 0.01f);
			ImGui::DragFloat3("obb.size", &obb2.size.x, 0.01f);
			ImGui::TreePop();
		}
		ImGui::End();

		color = isHit ? RED : WHITE;

		DrawGrid(viewProjectionMatrix, viewportMatrix, 5.0f, 26);
		DrawOBB(obb1, viewProjectionMatrix, viewportMatrix, color);
		DrawOBB(obb2, viewProjectionMatrix, viewportMatrix, WHITE);

		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}
