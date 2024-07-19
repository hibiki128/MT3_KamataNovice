#include "Debug.h"
#include "Matrix/MyMath.h"
#include "Matrix4x4.h"
#include "Vector2.h"
#include "algorithm"
#include "imgui.h"
#include <Novice.h>

const char kWindowTitle[] = "LE2B_20_ハギワラ_ヒビキ";

Vector3 Reflect(const Vector3& input, const Vector3& normal) {
	// i・n を計算
	float dotProduct = Dot(input, normal);

	// 2(i・n)n を計算
	Vector3 projection = normal * (2.0f * dotProduct);

	// r = i - 2(i・n)n
	Vector3 reflection = input - projection;

	return reflection;
}

void ReflectMove(Ball& ball, Plane& plane, const float& e) {
	ball.velocity += ball.acceleration * DELTA_TIME;
	ball.position += ball.velocity * DELTA_TIME;
	if (IsCollision(Sphere{ball.position, ball.radius}, plane)) {
		Vector3 reflected = Reflect(ball.velocity, plane.normal);
		Vector3 projectToNormal = Project(reflected, plane.normal);
		Vector3 movingDirection = reflected - projectToNormal;
		ball.velocity = projectToNormal * e + movingDirection;
	}
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// カメラの初期設定
	Vector3 cameraTranslate{0.0f, 1.9f, -6.49f};
	Vector3 cameraRotate{0.26f, 0.0f, 0.0f};
	Vector3 cameraPosition{0.0f, 1.0f, -5.0f};
	const int kWindowWidth = 1280;
	const int kWindowHeight = 720;
	Vector2Int ClickPosition = {};

	bool isStart = false;

	Plane plane;
	plane.normal = Normalize({-0.2f, 0.9f, -0.3f});
	plane.distance = 0.0f;

	Ball ball{};
	ball.position = {0.8f, 1.2f, 0.3f};
	ball.acceleration = {0.0f, -9.8f, 0.0f};
	ball.mass = 2.0f;
	ball.radius = 0.05f;
	ball.color = WHITE;

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

		// カメラの計算
		Matrix4x4 cameraMatrix = MakeAffineMatrix({1.0f, 1.0f, 1.0f}, cameraRotate, cameraPosition + cameraTranslate);
		Matrix4x4 viewMatrix = Inverse(cameraMatrix);
		Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(kWindowWidth) / float(kWindowHeight), 0.1f, 100.0f);
		Matrix4x4 viewProjectionMatrix = Multiply(viewMatrix, projectionMatrix);
		Matrix4x4 viewportMatrix = MakeViewPortMatrix(0, 0, float(kWindowWidth), float(kWindowHeight), 0.0f, 1.0f);

		CameraMove(cameraRotate, cameraTranslate, ClickPosition, keys, preKeys);

		if (isStart) {
			ReflectMove(ball, plane, 0.7f);
		} 
		if (ball.position.y < -3) {
			ball.position = {0.8f, 1.2f, 0.3f};
			ball.velocity = {0.0f, 0.0f, 0.0f};
		}

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		ImGui::Begin("Window");
		if (ImGui::Button("Start")) {
			isStart = !isStart;
		}
		ImGui::Text("isStart : %d", isStart);
		ImGui::End();

		DrawGrid(viewProjectionMatrix, viewportMatrix, 2.0f, 10);
		DrawPlane(plane, viewProjectionMatrix, viewportMatrix, WHITE);
		DrawBall(ball, viewProjectionMatrix, viewportMatrix, ball.color);

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