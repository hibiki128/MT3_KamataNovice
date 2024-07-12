#include "Debug.h"
#include "Matrix/MyMath.h"
#include "Matrix4x4.h"
#include "Vector2.h"
#include "algorithm"
#include "imgui.h"
#include <Novice.h>

const char kWindowTitle[] = "LE2B_20_ハギワラ_ヒビキ";

struct Pendulum {
	Vector3 anchor;
	float length;
	float angle;
	float angularVelocity;
	float angularAcceleration;
};

void PendulumMovement(Pendulum& pendulum, Vector3& p) {
	pendulum.angularAcceleration = -(9.8f / pendulum.length) * std::sin(pendulum.angle);
	pendulum.angularVelocity += pendulum.angularAcceleration * DELTA_TIME;
	pendulum.angle += pendulum.angularVelocity * DELTA_TIME;
	// pは振り子の先端の位置。取り付けたいものを取り付ければいい
	p.x = pendulum.anchor.x + std::sin(pendulum.angle) * pendulum.length;
	p.y = pendulum.anchor.y - std::cos(pendulum.angle) * pendulum.length;
	p.z = pendulum.anchor.z;
};

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

	Pendulum pendulum;
	pendulum.anchor = {0.0f, 1.0f, 0.0f};
	pendulum.length = 0.8f;
	pendulum.angle = 0.7f;
	pendulum.angularVelocity = 0.0f;
	pendulum.angularAcceleration = 0.0f;

	Sphere sphere{};
	sphere.center = {0.516f, 0.388f, 0.0f};
	sphere.radius = 0.05f;

	Line line{};
	line.origin = {0.0f, 4.0f, 0.0f};
	line.diff = sphere.center;

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
			PendulumMovement(pendulum, sphere.center);
		}

		Vector3 start = Transform(Transform(line.origin, viewportMatrix),viewProjectionMatrix));
		Vector3 end = Transform(Transform(line.origin + line.diff, viewportMatrix ), viewProjectionMatrix);

		line.diff = sphere.center - line.origin;

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
		ImGui::DragFloat3("center", &sphere.center.x, 0.01f);
		ImGui::End();

		DrawGrid(viewProjectionMatrix, viewportMatrix, 2.0f, 10);
		DrawSphere(sphere, viewProjectionMatrix, viewportMatrix, WHITE);
		Novice::DrawLine(int(start.x), int(start.y), int(end.x), int(end.y), WHITE);

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