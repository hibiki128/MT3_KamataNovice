#include "Matrix4x4.h"
#include "math.h"
#include <Novice.h>

float cotf(float theta) { return 1.0f / tanf(theta); }

// 1,透視投影行列
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip) {
	return {
		1.0f / aspectRatio * cotf(fovY / 2.0f), 0, 0, 0,
		0, cotf(fovY / 2.0f), 0, 0,
		0, 0, farClip / (farClip - nearClip), 1.0f,
		0, 0, -nearClip * farClip / (farClip - nearClip), 0};
};

// 2,正射影行列
Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip) {
	return {
		2.0f / (right - left), 0, 0, 0,
		0, 2.0f / (top - bottom), 0,0,
		0, 0, 1.0f / (farClip - nearClip),0,
		(left + right) / (left - right), (top + bottom) / (bottom - top), nearClip / (nearClip - farClip), 1.0f};
};

// 3,ビューポート変換行列
Matrix4x4 MakeViewPortMatrix(float left, float top, float width, float height, float minDepth, float maxDepth) {
	return {
		width / 2.0f, 0, 0, 0, 
		0, -height / 2.0f, 0, 0,
		0, 0, maxDepth - minDepth, 0, 
		left + width / 2.0f, top + height / 2.0f, minDepth, 1.0f};
};

const int kColumnWidth = 60;
const int kRowHeight = 20;
void MatrixScreenPrintf(int x, int y, const Matrix4x4& matrix, const char* label) {
	Novice::ScreenPrintf(x, y, "%s", label);
	for (int row = 0; row < 4; ++row) {
		for (int column = 0; column < 4; ++column) {
			Novice::ScreenPrintf(x + column * kColumnWidth, y + (1 + row) * kRowHeight, "%6.02f", matrix.m[row][column]);
		}
	}
}

const char kWindowTitle[] = "LE2B_20_ハギワラ_ヒビキ";

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	Matrix4x4 orthographicMatrix = MakeOrthographicMatrix(-160.f, 160.f, 200.0f, 300.0f, 0.0f, 1000.0f);
	Matrix4x4 perspectiveFovMatrix = MakePerspectiveFovMatrix(0.63f, 1.33f, 0.1f, 1000.0f);
	Matrix4x4 viewportMatrix = MakeViewPortMatrix(100.0f, 200.0f, 600.0f, 300.0f, 0.0f, 1.0f);

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

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		MatrixScreenPrintf(0, 0, orthographicMatrix, "orthographicMatrix");
		MatrixScreenPrintf(0, kRowHeight * 5, perspectiveFovMatrix, "perspectiveFovMatrix");
		MatrixScreenPrintf(0, kRowHeight * 10, viewportMatrix, "viewportMatrix");

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
