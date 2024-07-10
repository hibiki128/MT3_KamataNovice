#include "Debug.h"
#include "Matrix/MyMath.h"
#include "Matrix4x4.h"
#include "Vector2.h"
#include "algorithm"
#include "imgui.h"
#include <Novice.h>

const char kWindowTitle[] = "LE2B_20_ハギワラ_ヒビキ";

Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t) { return Vector3(v1.x + t * (v2.x - v1.x), v1.y + t * (v2.y - v1.y), v1.z + t * (v2.z - v1.z)); }

Vector3 CatmullRom(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, float t) {
	float t2 = t * t;
	float t3 = t2 * t;

	Vector3 v0 = (p2 - p0) * 0.5f;
	Vector3 v1 = (p3 - p1) * 0.5f;

	Vector3 a = 2.0f * p1 - 2.0f * p2 + v0 + v1;
	Vector3 b = -3.0f * p1 + 3.0f * p2 - 2.0f * v0 - v1;
	Vector3 c = v0;
	Vector3 d = p1;

	return a * t3 + b * t2 + c * t + d;
}

void DrawBezier(const Vector3& controlPoint0, const Vector3& controlPoint1, const Vector3& controlPoint2, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	const int numSegments = 100; // 曲線を描画するセグメント数
	Vector3 prevPoint = controlPoint0;

	for (int i = 1; i <= numSegments; ++i) {
		float t = static_cast<float>(i) / numSegments;

		// ベジェ曲線の現在の点を計算
		Vector3 p0 = Lerp(controlPoint0, controlPoint1, t);
		Vector3 p1 = Lerp(controlPoint1, controlPoint2, t);
		Vector3 currentPoint = Lerp(p0, p1, t);

		// 変換行列を適用
		Vector3 transformedPrevPoint = Transform(prevPoint, viewProjectionMatrix);
		transformedPrevPoint = Transform(transformedPrevPoint, viewportMatrix);

		Vector3 transformedCurrentPoint = Transform(currentPoint, viewProjectionMatrix);
		transformedCurrentPoint = Transform(transformedCurrentPoint, viewportMatrix);

		// ラインを描画
		Novice::DrawLine(
		    static_cast<int>(transformedPrevPoint.x), static_cast<int>(transformedPrevPoint.y), static_cast<int>(transformedCurrentPoint.x), static_cast<int>(transformedCurrentPoint.y), color);

		prevPoint = currentPoint;
	}

	// 制御点を描画
	Sphere sphere;
	sphere.radius = 0.01f;

	sphere.center = controlPoint0;
	DrawSphere(sphere, viewProjectionMatrix, viewportMatrix, BLACK);

	sphere.center = controlPoint1;
	DrawSphere(sphere, viewProjectionMatrix, viewportMatrix, BLACK);

	sphere.center = controlPoint2;
	DrawSphere(sphere, viewProjectionMatrix, viewportMatrix, BLACK);
}

void DrawCatmullRom(
    const Vector3& controlPoint0, const Vector3& controlPoint1, const Vector3& controlPoint2, const Vector3& controlPoint3, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix,
    uint32_t color) {

	const int numSegments = 100; // 曲線を描画するセグメント数

	for (int i = 0; i < numSegments; ++i) {
		float t0 = static_cast<float>(i) / numSegments;
		float t1 = static_cast<float>(i + 1) / numSegments;

		// Catmull-Rom曲線の現在の点を計算
		Vector3 p0 = CatmullRom(controlPoint0, controlPoint1, controlPoint2, controlPoint3, t0);
		Vector3 p1 = CatmullRom(controlPoint0, controlPoint1, controlPoint2, controlPoint3, t1);

		// 変換行列を適用
		Vector3 transformedP0 = Transform(p0, viewProjectionMatrix);
		transformedP0 = Transform(transformedP0, viewportMatrix);

		Vector3 transformedP1 = Transform(p1, viewProjectionMatrix);
		transformedP1 = Transform(transformedP1, viewportMatrix);

		// ラインを描画
		Novice::DrawLine(static_cast<int>(transformedP0.x), static_cast<int>(transformedP0.y), static_cast<int>(transformedP1.x), static_cast<int>(transformedP1.y), color);
	}

	// 制御点を描画
	Sphere sphere;
	sphere.radius = 0.01f;

	sphere.center = controlPoint0;
	DrawSphere(sphere, viewProjectionMatrix, viewportMatrix, BLACK);

	sphere.center = controlPoint1;
	DrawSphere(sphere, viewProjectionMatrix, viewportMatrix, BLACK);

	sphere.center = controlPoint2;
	DrawSphere(sphere, viewProjectionMatrix, viewportMatrix, BLACK);

	sphere.center = controlPoint3;
	DrawSphere(sphere, viewProjectionMatrix, viewportMatrix, BLACK);
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);
	Vector3 cameraTranslate{0.0f, 1.9f, -6.49f};
	Vector3 cameraRotate{0.26f, 0.0f, 0.0f};
	Vector3 cameraPosition{0.0f, 1.0f, -5.0f};
	uint32_t color = BLUE;
	/*bool isHit = false;*/
	Vector2Int ClickPosition = {};
	const int kWindowWidth = 1280;
	const int kWindowHeight = 720;

	Vector3 controlPoints[4] = {
	    {-0.8f,  0.58f,  1.0f  },
	    {1.76f,  1.0f,   -0.3f },
	    {0.94f,  -0.7f,  2.3f  },
	    {-0.53f, -0.26f, -0.15f},
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

		Matrix4x4 cameraMatrix = MakeAffineMatrix({1.0f, 1.0f, 1.0f}, cameraRotate, cameraPosition + cameraTranslate);
		Matrix4x4 viewMatrix = Inverse(cameraMatrix);
		Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(kWindowWidth) / float(kWindowHeight), 0.1f, 100.0f);
		Matrix4x4 viewProjectionMatrix = Multiply(viewMatrix, projectionMatrix);
		Matrix4x4 viewportMatrix = MakeViewPortMatrix(0, 0, float(kWindowWidth), float(kWindowHeight), 0.0f, 1.0f);

		CameraMove(cameraRotate, cameraTranslate, ClickPosition, keys, preKeys);

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		ImGui::Begin("Window");
		ImGui::DragFloat3("controlPoints[0]", &controlPoints[0].x, 0.01f);
		ImGui::DragFloat3("controlPoints[1]", &controlPoints[1].x, 0.01f);
		ImGui::DragFloat3("controlPoints[2]", &controlPoints[2].x, 0.01f);
		ImGui::DragFloat3("controlPoints[3]", &controlPoints[3].x, 0.01f);
		ImGui::End();

		DrawGrid(viewProjectionMatrix, viewportMatrix, 5.0f, 26);
		DrawCatmullRom(controlPoints[0], controlPoints[1], controlPoints[2], controlPoints[3], viewProjectionMatrix, viewportMatrix, color);
		DrawCatmullRom(controlPoints[1], controlPoints[2], controlPoints[3], controlPoints[0], viewProjectionMatrix, viewportMatrix, color);
		DrawCatmullRom(controlPoints[3], controlPoints[0], controlPoints[1], controlPoints[2], viewProjectionMatrix, viewportMatrix, color);
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
