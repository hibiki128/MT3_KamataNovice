#include "Debug.h"
#include "Matrix/MyMath.h"
#include "Matrix4x4.h"
#include "Vector2.h"
#include "algorithm"
#include "imgui.h"
#include <Novice.h>

const char kWindowTitle[] = "LE2B_20_ハギワラ_ヒビキ";

// 衝突判定関数の実装
Matrix4x4 MakeOBBWorldMatrix(const OBB& obb, const Matrix4x4& rotateMatrix) {
	Matrix4x4 translationMatrix = MakeTranslateMatrix(obb.center);
	return Multiply(rotateMatrix, translationMatrix); // rotateMatrixを使用
}

AABB ConvertOBBToAABB(const OBB& obb) {
	AABB aabb;
	aabb.min = {-obb.size.x, -obb.size.y, -obb.size.z};
	aabb.max = {obb.size.x, obb.size.y, obb.size.z};
	return aabb;
}

bool IsCollision(const OBB& obb,const Sphere& sphere,const Matrix4x4& rotateMatrix) {

	// OBBのWorldMatrixを作成
	Matrix4x4 obbWorldMatrix = MakeOBBWorldMatrix(obb, rotateMatrix);

	// OBBのWorldMatrixの逆行列を取得
	Matrix4x4 obbWorldMatrixInverse = Inverse(obbWorldMatrix);

	// Sphereの中心点をOBBのローカル空間に変換
	Vector3 centerInOBBLocalSpace = Transform(sphere.center, obbWorldMatrixInverse);

	// OBBからAABBを作成
	AABB aabbOBBLocal = ConvertOBBToAABB(obb);

	// ローカル空間でのSphere
	Sphere sphereOBBLocal{centerInOBBLocalSpace, sphere.radius};

	Vector3 closestPoint = {
	    std::max(aabbOBBLocal.min.x, std::min(sphereOBBLocal.center.x, aabbOBBLocal.max.x)), std::max(aabbOBBLocal.min.y, std::min(sphereOBBLocal.center.y, aabbOBBLocal.max.y)),
	    std::max(aabbOBBLocal.min.z, std::min(sphereOBBLocal.center.z, aabbOBBLocal.max.z))};
	Vector3 distance = closestPoint - sphereOBBLocal.center;
	return Dot(distance, distance) <= (sphereOBBLocal.radius * sphereOBBLocal.radius);
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

	Vector3 rotate{0.0f, 0.0f, 0.0f};
	OBB obb{
	    .center{-1.0f,              0.0f,               0.0f              },
        .orientations = {{1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        .size{0.5f,               0.5f,               0.5f              }
    };

	Sphere sphere{
	    .center{0.0f, 0.0f, 0.0f},
        .radius{0.5f}
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

		// ローカル空間で衝突判定
		isHit = IsCollision(obb,sphere,rotateMatrix);

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		ImGui::Begin("Window");
		ImGui::DragFloat3("obb.center", &obb.center.x, 0.01f);
		ImGui::SliderAngle("rotateX", &rotate.x);
		ImGui::SliderAngle("rotateY", &rotate.y);
		ImGui::SliderAngle("rotateZ", &rotate.z);
		ImGui::DragFloat3("obb.orientations", &obb.orientations[0].x, 0.01f);
		ImGui::DragFloat3("obb.orientations", &obb.orientations[1].x, 0.01f);
		ImGui::DragFloat3("obb.orientations", &obb.orientations[2].x, 0.01f);
		ImGui::DragFloat3("obb.size", &obb.size.x, 0.01f);
		ImGui::DragFloat3("sphere.center", &sphere.center.x, 0.01f);
		ImGui::DragFloat("sphere.radius", &sphere.radius, 0.01f);
		ImGui::End();

		color = isHit ? RED : WHITE;

		DrawGrid(viewProjectionMatrix, viewportMatrix, 5.0f, 26);
		DrawSphere(sphere, viewProjectionMatrix, viewportMatrix, WHITE);
		DrawOBB(obb, viewProjectionMatrix, viewportMatrix, color);

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
