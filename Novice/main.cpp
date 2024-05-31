#include "Debug.h"
#include "Matrix/M4x4.h"
#include "Matrix4x4.h"
#include "Vector2.h"
#include "imgui.h"
#include <Novice.h>
#include <algorithm>

const char kWindowTitle[] = "LE2B_20_ハギワラ_ヒビキ";

bool IsCollision(const AABB& aabb, const Sphere& sphere) {
	// 最近接点を求める
	Vector3 closestPoint{std::clamp(sphere.center.x, aabb.min.x, aabb.max.x), std::clamp(sphere.center.y, aabb.min.y, aabb.max.y), std::clamp(sphere.center.z, aabb.min.z, aabb.max.z)};

	// 最近接点と球の中心との距離を求める
	float distance = Length(closestPoint - sphere.center);

	// 距離が半径より小さければ衝突
	if (distance <= sphere.radius) {
		return true;
	} else {
		return false;
	}
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
	Sphere sphere;
	sphere.center = {1.0f, 1.0f, 1.0f};
	sphere.radius = 1.0f;
	AABB aabb;
	aabb.min = {-0.5f, -0.5f, -0.5f};
	aabb.max = {0.0f, 0.0f, 0.0f};
	const int kWindowWidth = 1280;
	const int kWindowHeight = 720;

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

		isHit = IsCollision(aabb, sphere);

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		ImGui::Begin("Window");
		ImGui::DragFloat3("aabb.min", &aabb.min.x, 0.01f);
		ImGui::DragFloat3("aabb.max", &aabb.max.x, 0.01f);
		ImGui::DragFloat3("sphere.center", &sphere.center.x, 0.01f);
		ImGui::DragFloat("sphere.radius", &sphere.radius, 0.01f);
		ImGui::End();
		if (isHit == true) {
			color = RED;
		} else {
			color = WHITE;
		}
		SetAABB(aabb);
		DrawGrid(viewProjectionMatrix, viewportMatrix, 5.0f, 26);
		DrawSphere(sphere, viewProjectionMatrix, viewportMatrix, WHITE);
		DrawAABB(aabb, viewProjectionMatrix, viewportMatrix, color);

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
