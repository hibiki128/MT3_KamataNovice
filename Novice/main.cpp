#include "Matrix/M4x4.h"
#include "Matrix4x4.h"
#include <Novice.h>

const char kWindowTitle[] = "LE2B_20_ハギワラ_ヒビキ";

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	Vector3 v1{1.2f, -3.9f, 2.5f};
	Vector3 v2{2.8f, 0.4f, -1.3f};
	Vector3 cross = Cross(v1, v2);
	Vector3 rotate{};
	Vector3 translate{};
	Vector3 cameraPosition{0.0f, 0.0f, -10.0f};
	const int kWindowWidth = 1280;
	const int kWindowHeight = 720;
	Vector3 kLocalVertices[3] = {
	    {0.0f,  0.5f,  0.0f},
        {-0.5f, -0.5f, 0.0f},
        {0.5f,  -0.5f, 0.0f}
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

		if (keys[DIK_W]) {
			translate.z += 0.05f;
		}
		if (keys[DIK_S]) {
			translate.z -= 0.05f;
		}
		if (keys[DIK_A]) {
			translate.x -= 0.05f;
		}
		if (keys[DIK_D]) {
			translate.x += 0.05f;
		}
		rotate.y += 0.05f;

		Matrix4x4 worldMatrix = MakeAffineMatrix({1.0f, 1.0f, 1.0f}, rotate, translate);
		Matrix4x4 cameraMatrix = MakeAffineMatrix({1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, cameraPosition);
		Matrix4x4 viewMatrix = Inverse(cameraMatrix);
		Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(kWindowWidth) / float(kWindowHeight), 0.1f, 100.0f);
		// WVPMatrixを作る
		Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
		// ViewportMatrixを作る
		Matrix4x4 viewportMatrix = MakeViewPortMatrix(0, 0, float(kWindowWidth), float(kWindowHeight), 0.0f, 1.0f);
		// Screen空間へと頂点を変換する
		Vector3 screenVertices[3];
		for (uint32_t i = 0; i < 3; ++i) {
			// NDCまで変換、Transformを使うと同次座標系->デカルト座標系の処理が行われ、結果的にZDivideが行われることになる
			Vector3 ndcVertex = Transform(kLocalVertices[i], worldViewProjectionMatrix);
			// Viewport変換を行ってScreen空間へ
			screenVertices[i] = Transform(ndcVertex, viewportMatrix);
		}

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		VectorScreenPrintf(0, 0, cross, "Cross");
		Novice::DrawTriangle(
		    int(screenVertices[0].x), int(screenVertices[0].y), int(screenVertices[1].x), int(screenVertices[1].y),
			int(screenVertices[2].x), int(screenVertices[2].y), RED, kFillModeSolid);

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
