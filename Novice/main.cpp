#include "Debug.h"
#include "Matrix/M4x4.h"
#include "Matrix4x4.h"
#include "Vector2.h"
#include "imgui.h"
#include <Novice.h>
#include <algorithm>

const char kWindowTitle[] = "LE2B_20_ハギワラ_ヒビキ";

bool IsCollision(const Triangle& triangle, const Segment& segment) {
	Vector3 edge01 = triangle.vertices[1] - triangle.vertices[0];
	Vector3 edge12 = triangle.vertices[2] - triangle.vertices[1];
	Vector3 edge20 = triangle.vertices[0] - triangle.vertices[2];

	Vector3 normal = Cross(edge01, edge12);

	Vector3 cross01 = Cross(edge01, segment.origin - triangle.vertices[0]);
	Vector3 cross12 = Cross(edge12, segment.origin - triangle.vertices[1]);
	Vector3 cross20 = Cross(edge20, segment.origin - triangle.vertices[2]);

	// すべての小三角形のクロス積と法線が同じ方向を向いていたら衝突
	if (Dot(cross01, normal) >= 0.0f && Dot(cross12, normal) >= 0.0f && Dot(cross20, normal) >= 0.0f) {
		// 衝突
		return true;
	}

	// 衝突しない
	return false;
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);
	Vector3 cameraTranslate{0.0f, 1.9f, -6.49f};
	Vector3 cameraRotate{0.26f, 0.0f, 0.0f};
	Vector3 cameraPosition{0.0f, 1.0f, -5.0f};
	Segment line;
	line.origin = {-0.45f, 0.75f, 0.0f};
	line.diff = {1.0f, 0.5f, 0.0f};
	Triangle triangle;
	triangle.vertices[0] = {-1.0f, 0.0f, 0.0f};
	triangle.vertices[1] = {0.0f, 1.0f, 0.0f};
	triangle.vertices[2] = {1.0f, 0.0f, 0.0f};
	uint32_t color = WHITE;
	bool isHit = false;
	Vector2Int ClickPosition = {};

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

		Vector3 start = Transform(Transform(line.origin, viewProjectionMatrix), viewportMatrix);
		Vector3 end = Transform(Transform(line.origin + line.diff, viewProjectionMatrix), viewportMatrix);

		isHit = IsCollision(triangle, line);

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		ImGui::Begin("Window");
		ImGui::DragFloat3("Triangle.v0", &triangle.vertices[0].x, 0.01f);
		ImGui::DragFloat3("Triangle.v1", &triangle.vertices[1].x, 0.01f);
		ImGui::DragFloat3("Triangle.v2", &triangle.vertices[2].x, 0.01f);
		ImGui::DragFloat3("Segment.Origin", &line.origin.x, 0.01f);
		ImGui::DragFloat3("Segment.Diff", &line.diff.x, 0.01f);
		ImGui::End();
		if (isHit == true) {
			color = RED;
		} else {
			color = WHITE;
		}
		DrawGrid(viewProjectionMatrix, viewportMatrix);
		Novice::DrawLine(int(start.x), int(start.y), int(end.x), int(end.y), color);
		DrawTriangle(triangle, viewProjectionMatrix, viewportMatrix, WHITE);

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
