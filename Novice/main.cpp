#include "Debug.h"
#include "Matrix/MyMath.h"
#include "Matrix4x4.h"
#include "Vector2.h"
#include "algorithm"
#include "imgui.h"
#include <Novice.h>

const char kWindowTitle[] = "LE2B_20_ハギワラ_ヒビキ";

struct ConicalPendulum {
	Vector3 anchor;         // アンカーポイント。固定された端の位置
	float length;          // 紐の長さ
	float halfApexAngle;   // 円錐の直角の半分
	float angle;           // 現在の角度
	float angularVelocity; // 角速度ω
};

void ConicalPendulumMove(ConicalPendulum& conicalPendulum, Ball& ball) { 
	conicalPendulum.angularVelocity = std::sqrt(9.8f / (conicalPendulum.length * std::cos(conicalPendulum.halfApexAngle)));
	conicalPendulum.angle += conicalPendulum.angularVelocity * DELTA_TIME;
	float radius = std::sin(conicalPendulum.halfApexAngle) * conicalPendulum.length;
	float height = std::cos(conicalPendulum.halfApexAngle) * conicalPendulum.length;
	ball.position.x = conicalPendulum.anchor.x + std::cos(conicalPendulum.angle) * radius;
	ball.position.y = conicalPendulum.anchor.y - height;
	ball.position.z = conicalPendulum.anchor.z - std::sin(conicalPendulum.angle) * radius;
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
	
	ConicalPendulum conicalPendulum;
	conicalPendulum.anchor = {0.0f, 1.0f, 0.0f};
	conicalPendulum.length = 0.8f;
	conicalPendulum.halfApexAngle = 0.7f;
	conicalPendulum.angle = 0.0f;
	conicalPendulum.angularVelocity = 0.0f;

	Ball ball{};
	ball.position = {0.8f, 0.5f, 0.0f};
	ball.radius = 0.05f;

	Sphere sphere{};
	sphere.center = ball.position;
	sphere.radius = ball.radius;

	Line line{};
	line.origin = {0.0f, 2.0f, 0.0f};
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
			ConicalPendulumMove(conicalPendulum, ball);
		}
		sphere.center = ball.position;
		line.diff = sphere.center - line.origin;

		Vector3 start = Transform(Transform(line.origin, viewProjectionMatrix), viewportMatrix);
		Vector3 end = Transform(Transform(line.origin + line.diff, viewProjectionMatrix), viewportMatrix);

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
		ImGui::SliderFloat("Length", &conicalPendulum.length, 0.5f, 10);
		ImGui::SliderFloat("HalfApexAngle", &conicalPendulum.halfApexAngle, 0, 1.5f);
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