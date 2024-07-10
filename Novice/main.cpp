#include "Debug.h"
#include "Matrix/MyMath.h"
#include "Matrix4x4.h"
#include "Vector2.h"
#include "algorithm"
#include "imgui.h"
#include <Novice.h>

const char kWindowTitle[] = "LE2B_20_ハギワラ_ヒビキ";

// 線を描画する関数
void DrawLineBetweenSpheres(const Vector3& point1, const Vector3& point2, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	// 3D座標を2Dスクリーン座標に変換
	Vector3 screenPoint1 = Transform(point1, viewProjectionMatrix);
	screenPoint1 = Transform(screenPoint1, viewportMatrix);
	Vector3 screenPoint2 = Transform(point2, viewProjectionMatrix);
	screenPoint2 = Transform(screenPoint2, viewportMatrix);

	// 線を描画
	Novice::DrawLine(int(screenPoint1.x), int(screenPoint1.y), int(screenPoint2.x), int(screenPoint2.y), color);
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

	// 各部位の初期情報
	Vector3 translates[3] = {
	    {0.2f, 1.0f, 0.0f},
	    {0.4f, 0.0f, 0.0f},
	    {0.3f, 0.0f, 0.0f},
	};

	Vector3 rotates[3] = {
	    {0.0f, 0.0f, -6.8f},
	    {0.0f, 0.0f, -1.4f},
	    {0.0f, 0.0f, 0.0f },
	};

	Vector3 scales[3] = {
	    {1.0f, 1.0f, 1.0f},
	    {1.0f, 1.0f, 1.0f},
	    {1.0f, 1.0f, 1.0f},
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

		// カメラの計算
		Matrix4x4 cameraMatrix = MakeAffineMatrix({1.0f, 1.0f, 1.0f}, cameraRotate, cameraPosition + cameraTranslate);
		Matrix4x4 viewMatrix = Inverse(cameraMatrix);
		Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(kWindowWidth) / float(kWindowHeight), 0.1f, 100.0f);
		Matrix4x4 viewProjectionMatrix = Multiply(viewMatrix, projectionMatrix);
		Matrix4x4 viewportMatrix = MakeViewPortMatrix(0, 0, float(kWindowWidth), float(kWindowHeight), 0.0f, 1.0f);

		// 更新処理
		CameraMove(cameraRotate, cameraTranslate, ClickPosition, keys, preKeys);

		// 肩のワールド変換行列を計算
		Matrix4x4 shoulderScaleMatrix = MakeScaleMatrix(scales[0]);
		Matrix4x4 shoulderRotateMatrix = MakeRotateXYZMatrix(rotates[0]);
		Matrix4x4 shoulderTranslateMatrix = MakeTranslateMatrix(translates[0]);
		Matrix4x4 shoulderWorldMatrix = Multiply(shoulderScaleMatrix, Multiply(shoulderRotateMatrix, shoulderTranslateMatrix));

		// ひじのワールド変換行列を計算
		Matrix4x4 elbowScaleMatrix = MakeScaleMatrix(scales[1]);
		Matrix4x4 elbowRotateMatrix = MakeRotateXYZMatrix(rotates[1]);
		Matrix4x4 elbowTranslateMatrix = MakeTranslateMatrix(translates[1]);
		Matrix4x4 elbowLocalMatrix = Multiply(elbowScaleMatrix, Multiply(elbowRotateMatrix, elbowTranslateMatrix));
		Matrix4x4 elbowWorldMatrix = Multiply(elbowLocalMatrix, shoulderWorldMatrix);

		// 手のワールド変換行列を計算
		Matrix4x4 handScaleMatrix = MakeScaleMatrix(scales[2]);
		Matrix4x4 handRotateMatrix = MakeRotateXYZMatrix(rotates[2]);
		Matrix4x4 handTranslateMatrix = MakeTranslateMatrix(translates[2]);
		Matrix4x4 handLocalMatrix = Multiply(handScaleMatrix, Multiply(handRotateMatrix, handTranslateMatrix));
		Matrix4x4 handWorldMatrix = Multiply(handLocalMatrix, elbowWorldMatrix);

		// 肩、ひじ、手の球を定義
		Sphere shoulderSphere{
		    {shoulderWorldMatrix.m[3][0], shoulderWorldMatrix.m[3][1], shoulderWorldMatrix.m[3][2]},
            0.1f
        };
		Sphere elbowSphere{
		    {elbowWorldMatrix.m[3][0], elbowWorldMatrix.m[3][1], elbowWorldMatrix.m[3][2]},
            0.1f
        };
		Sphere handSphere{
		    {handWorldMatrix.m[3][0], handWorldMatrix.m[3][1], handWorldMatrix.m[3][2]},
            0.1f
        };

		// 描画処理
		ImGui::Begin("Window");
		for (int i = 0; i < 3; ++i) {
			char labelTranslate[20];
			char labelRotate[20];
			char labelScale[20];
			sprintf_s(labelTranslate, "translates[%d]", i);
			sprintf_s(labelRotate, "rotates[%d]", i);
			sprintf_s(labelScale, "scales[%d]", i);

			ImGui::DragFloat3(labelTranslate, &translates[i].x, 0.01f);
			ImGui::DragFloat3(labelRotate, &rotates[i].x, 0.01f);
			ImGui::DragFloat3(labelScale, &scales[i].x, 0.01f);
		}
		ImGui::End();

		DrawGrid(viewProjectionMatrix, viewportMatrix, 2.0f, 10);
		// 球を描画
		DrawSphere(shoulderSphere, viewProjectionMatrix, viewportMatrix, RED);
		DrawSphere(elbowSphere, viewProjectionMatrix, viewportMatrix, GREEN);
		DrawSphere(handSphere, viewProjectionMatrix, viewportMatrix, BLUE);

		// 線を描画
		DrawLineBetweenSpheres(shoulderSphere.center, elbowSphere.center, viewProjectionMatrix, viewportMatrix, WHITE);
		DrawLineBetweenSpheres(elbowSphere.center, handSphere.center, viewProjectionMatrix, viewportMatrix, WHITE);

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