#include "Matrix/M4x4.h"
#include "Matrix4x4.h"
#include <Novice.h>
#include"imgui.h"
#define _USE_DEFINE_MATH
#include "math.h"

const char kWindowTitle[] = "LE2B_20_ハギワラ_ヒビキ";

struct Sphere {
	Vector3 center; // !< 中心点
	float radius; // !< 半径
};

void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {
	const float kGridHalfWidth = 2.0f;                                      // グリッドの半分の幅
	const uint32_t kSubdivision = 10;                                       // 分割数
	const float kGridEvery = (kGridHalfWidth * 2.0f) / float(kSubdivision); // 1つの長さ

	// 奥から手前への線を順々に引いていく
	for (uint32_t xIndex = 0; xIndex <= kSubdivision; ++xIndex) {
		// ワールド座標系上の始点と終点を求める
		Vector3 worldStartPos = {-kGridHalfWidth + xIndex * kGridEvery, 0.0f, -kGridHalfWidth};
		Vector3 worldEndPos = {-kGridHalfWidth + xIndex * kGridEvery, 0.0f, kGridHalfWidth};

		// ビュープロジェクションマトリックスを使ってクリップ座標系に変換
		Vector3 clipStartPos = Transform(worldStartPos, viewProjectionMatrix);
		Vector3 clipEndPos = Transform(worldEndPos, viewProjectionMatrix);

		// クリップ座標系からスクリーン座標系に変換
		Vector3 screenStartPos = Transform(clipStartPos, viewportMatrix);
		Vector3 screenEndPos = Transform(clipEndPos, viewportMatrix);

		 // 真ん中の線を黒で描画
		if (xIndex == kSubdivision / 2 && kSubdivision % 2 == 0) {
			Novice::DrawLine(int(screenStartPos.x), int(screenStartPos.y), int(screenEndPos.x), int(screenEndPos.y), 0x000000FF); // 黒色で描画
		} else {
			Novice::DrawLine(int(screenStartPos.x), int(screenStartPos.y), int(screenEndPos.x), int(screenEndPos.y), 0xAAAAAAFF); // グレーで描画
		}
	}

	// 左右の線を引くためにyIndexとzIndexのループも同様に処理
	for (uint32_t yIndex = 0; yIndex <= kSubdivision; ++yIndex) {
		// ワールド座標系上の始点と終点を求める
		Vector3 worldStartPos = {-kGridHalfWidth, 0.0f, -kGridHalfWidth + yIndex * kGridEvery};
		Vector3 worldEndPos = {kGridHalfWidth, 0.0f, -kGridHalfWidth + yIndex * kGridEvery};

		// ビュープロジェクションマトリックスを使ってクリップ座標系に変換
		Vector3 clipStartPos = Transform(worldStartPos, viewProjectionMatrix);
		Vector3 clipEndPos = Transform(worldEndPos, viewProjectionMatrix);

		// クリップ座標系からスクリーン座標系に変換
		Vector3 screenStartPos = Transform(clipStartPos, viewportMatrix);
		Vector3 screenEndPos = Transform(clipEndPos, viewportMatrix);

		 // 真ん中の線を黒で描画
		if (yIndex == kSubdivision / 2 && kSubdivision % 2 == 0) {
			Novice::DrawLine(int(screenStartPos.x), int(screenStartPos.y), int(screenEndPos.x), int(screenEndPos.y), 0x000000FF); // 黒色で描画
		} else {
			Novice::DrawLine(int(screenStartPos.x), int(screenStartPos.y), int(screenEndPos.x), int(screenEndPos.y), 0xAAAAAAFF); // グレーで描画
		}
	}

	for (uint32_t zIndex = 0; zIndex <= kSubdivision; ++zIndex) {
		// ワールド座標系上の始点と終点を求める
		Vector3 worldStartPos = {-kGridHalfWidth + zIndex * kGridEvery, 0.0f, -kGridHalfWidth};
		Vector3 worldEndPos = {-kGridHalfWidth + zIndex * kGridEvery, 0.0f, kGridHalfWidth};

		// ビュープロジェクションマトリックスを使ってクリップ座標系に変換
		Vector3 clipStartPos = Transform(worldStartPos, viewProjectionMatrix);
		Vector3 clipEndPos = Transform(worldEndPos, viewProjectionMatrix);

		// クリップ座標系からスクリーン座標系に変換
		Vector3 screenStartPos = Transform(clipStartPos, viewportMatrix);
		Vector3 screenEndPos = Transform(clipEndPos, viewportMatrix);

		 // 真ん中の線を黒で描画
		if (zIndex == kSubdivision / 2 && kSubdivision % 2 == 0) {
			Novice::DrawLine(int(screenStartPos.x), int(screenStartPos.y), int(screenEndPos.x), int(screenEndPos.y), 0x000000FF); // 黒色で描画
		} else {
			Novice::DrawLine(int(screenStartPos.x), int(screenStartPos.y), int(screenEndPos.x), int(screenEndPos.y), 0xAAAAAAFF); // グレーで描画
		}
	}
}

void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMarix, uint32_t color) { 
	const uint32_t kSubdivision = 10; // 分割数
	const float kLonEvery = 0; // 経度分割1つ分の角度
	const float kLatEvery = 0; // 緯度分割1つ分の角度
	// 緯度の方向に分割　-π/2 ～ π/2
	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
		float lat = -float(M_PI)
	}
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	Vector3 cameraRotate{};
	Vector3 cameraTranslate{};
	Vector3 cameraPosition{0.0f, 1.0f, -5.0f};
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

		Matrix4x4 worldMatrix = MakeAffineMatrix({1.0f, 1.0f, 1.0f}, cameraRotate, cameraTranslate);
		Matrix4x4 cameraMatrix = MakeAffineMatrix({1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, cameraPosition);
		Matrix4x4 viewMatrix = Inverse(cameraMatrix);
		Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(kWindowWidth) / float(kWindowHeight), 0.1f, 100.0f);
		// WVPMatrixを作る
		Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
		// ViewportMatrixを作る
		Matrix4x4 viewportMatrix = MakeViewPortMatrix(0, 0, float(kWindowWidth), float(kWindowHeight), 0.0f, 1.0f);

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		ImGui::Begin("Window");
		ImGui::DragFloat3("CameraTranslate", &cameraTranslate.x, 0.01f);
		ImGui::DragFloat3("CameraRotate", &cameraRotate.x, 0.01f);
		ImGui::End();
		DrawGrid(worldViewProjectionMatrix, viewportMatrix);

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
