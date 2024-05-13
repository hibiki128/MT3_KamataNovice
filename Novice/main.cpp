#include "Debug.h"
#include "Matrix/M4x4.h"
#include "Matrix4x4.h"
#include "Vector2.h"
#include "imgui.h"
#include <Novice.h>
#include <algorithm>

const char kWindowTitle[] = "LE2B_20_ハギワラ_ヒビキ";

bool IsCollision(const Sphere& s1, const Sphere& s2) {
	float distance = Length(s2.center - s1.center);

	if (distance <= s1.radius + s2.radius) {
		// 当たったとき
		return true;
	} else {
		// 当たっていないとき
		return false;
	}
};

struct Vector2Int {
	int x;
	int y;
};

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	Vector3 cameraTranslate{0.0f, 1.9f, -6.49f};
	Vector3 cameraRotate{0.26f, 0.0f, 0.0f};
	Vector3 cameraPosition{0.0f, 1.0f, -5.0f};
	Sphere sphere[2];
	sphere[0].center = {0.0f, 0.0f, 0.0f};
	sphere[0].radius = 1.0f;
	sphere[1].center = {0.8f, 0.0f, 1.0f};
	sphere[1].radius = 1.0f;
	uint32_t color = WHITE;
	bool isHit = false;
	// マウスのクリック位置を保存する変数
	Vector2Int clickPosition = {0, 0};
	bool isLeftClicked = false;
	bool isWheelClicked = false;
	// カーソルを動かすときの感度
	float mouseSensitivity = 0.001f;
	// カメラの移動速度
	float moveSpeed = 0.001f;

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
		Matrix4x4 ViewProjectionMatrix = Multiply(viewMatrix, projectionMatrix);
		Matrix4x4 viewportMatrix = MakeViewPortMatrix(0, 0, float(kWindowWidth), float(kWindowHeight), 0.0f, 1.0f);

		/// ========カメラ操作========

		// カメラの回転を更新する
		if (Novice::IsPressMouse(0) == 1) {
			if (!isLeftClicked) {
				// マウスがクリックされたときに現在のマウス位置を保存する
				Novice::GetMousePosition(&clickPosition.x, &clickPosition.y);
				isLeftClicked = true;
			} else {
				// マウスがクリックされている間はカメラの回転を更新する
				Vector2Int currentMousePos;
				Novice::GetMousePosition(&currentMousePos.x, &currentMousePos.y);

				float deltaX = static_cast<float>(currentMousePos.x - clickPosition.x);
				float deltaY = static_cast<float>(currentMousePos.y - clickPosition.y);

				cameraRotate.x += deltaY * mouseSensitivity;
				cameraRotate.y += deltaX * mouseSensitivity;

				// 現在のマウス位置を保存する
				clickPosition = currentMousePos;
			}
		} else {
			// マウスがクリックされていない場合はフラグをリセットする
			isLeftClicked = false;
		}

		// マウスホイールの移動量を取得する
		int wheelDelta = Novice::GetWheel();

		// カメラの位置を更新する
		if (Novice::IsPressMouse(2) == 1) {
			if (!isWheelClicked) {
				// マウスがクリックされたときに現在のマウス位置を保存する
				Novice::GetMousePosition(&clickPosition.x, &clickPosition.y);
				isWheelClicked = true;
			} else {
				// マウスがクリックされている間はカメラの位置を更新する
				Vector2Int currentMousePos;
				Novice::GetMousePosition(&currentMousePos.x, &currentMousePos.y);

				float deltaX = static_cast<float>(currentMousePos.x - clickPosition.x);
				float deltaY = static_cast<float>(currentMousePos.y - clickPosition.y);

				cameraTranslate.x -= deltaX * mouseSensitivity;
				cameraTranslate.y += deltaY * mouseSensitivity;

				// 現在のマウス位置を保存する
				clickPosition = currentMousePos;
			}
		} else {
			// マウスがクリックされていない場合はフラグをリセットする
			isWheelClicked = false;
		}
		// マウスホイールの移動量に応じてカメラの移動を更新する
		cameraTranslate.z += wheelDelta * moveSpeed;
		/// =====================

		isHit = IsCollision(sphere[0], sphere[1]);

		if (isHit == true) {
			color = RED;
		} else {
			color = WHITE;
		}

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		ImGui::Begin("Window");
		ImGui::DragFloat3("Sphere[0].Center", &sphere[0].center.x, 0.01f);
		ImGui::DragFloat("Sphere[0].Radius", &sphere[0].radius, 0.01f);
		ImGui::DragFloat3("Sphere[1].Center", &sphere[1].center.x, 0.01f);
		ImGui::DragFloat("Sphere[1].Radius", &sphere[1].radius, 0.01f);
		ImGui::End();
		DrawGrid(ViewProjectionMatrix, viewportMatrix);
		DrawSphere(sphere[0], ViewProjectionMatrix, viewportMatrix, color);
		DrawSphere(sphere[1], ViewProjectionMatrix, viewportMatrix, WHITE);

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
