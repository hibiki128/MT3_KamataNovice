#define NOMINMAX
#include "Debug.h"
#include "Matrix/M4x4.h"
#include "Matrix4x4.h"
#include "Vector2.h"
#include "algorithm"
#include "imgui.h"
#include <Novice.h>

const char kWindowTitle[] = "LE2B_20_ハギワラ_ヒビキ";

bool IsCollision(const AABB& aabb, const Segment& segment) {
	Vector3 invDir = Vector3(
	    segment.diff.x != 0 ? 1.0f / segment.diff.x : std::numeric_limits<float>::infinity(), segment.diff.y != 0 ? 1.0f / segment.diff.y : std::numeric_limits<float>::infinity(),
	    segment.diff.z != 0 ? 1.0f / segment.diff.z : std::numeric_limits<float>::infinity());

	Vector3 tNear, tFar;

	// x軸の交差点を計算
	if (invDir.x >= 0) {
		tNear.x = (aabb.min.x - segment.origin.x) * invDir.x;
		tFar.x = (aabb.max.x - segment.origin.x) * invDir.x;
	} else {
		tNear.x = (aabb.max.x - segment.origin.x) * invDir.x;
		tFar.x = (aabb.min.x - segment.origin.x) * invDir.x;
	}

	// y軸の交差点を計算
	if (invDir.y >= 0) {
		tNear.y = (aabb.min.y - segment.origin.y) * invDir.y;
		tFar.y = (aabb.max.y - segment.origin.y) * invDir.y;
	} else {
		tNear.y = (aabb.max.y - segment.origin.y) * invDir.y;
		tFar.y = (aabb.min.y - segment.origin.y) * invDir.y;
	}

	// z軸の交差点を計算
	if (invDir.z >= 0) {
		tNear.z = (aabb.min.z - segment.origin.z) * invDir.z;
		tFar.z = (aabb.max.z - segment.origin.z) * invDir.z;
	} else {
		tNear.z = (aabb.max.z - segment.origin.z) * invDir.z;
		tFar.z = (aabb.min.z - segment.origin.z) * invDir.z;
	}

	// AABBとの衝突点(貫通点)のtが小さい方
	float tmin = std::max(std::max(tNear.x, tNear.y), tNear.z);
	float tmax = std::min(std::min(tFar.x, tFar.y), tFar.z);

	// tminがtmax以下、かつtmaxが0以上でtminが1以下の場合、衝突している
	if (tmin <= tmax && tmax >= 0.0f && tmin <= 1.0f) {
		return true;
	} else {
		return false;
	}
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);
	Vector3 cameraTranslate{0.0f, 1.9f, -6.49f};
	Vector3 cameraRotate{0.26f, 0.0f, 0.0f};
	Vector3 cameraPosition{0.0f, 1.0f, -5.0f};
	uint32_t color = WHITE;
	bool isHit = false;
	Vector2Int ClickPosition = {};
	Segment line;
	line.origin = {-0.7f, 0.3f, 0.0f};
	line.diff = {2.0f, -0.5f, 0.0f};
	AABB aabb;
	aabb.min = {-0.5f, -0.5f, -0.5f};
	aabb.max = {0.5f, 0.5f, 0.5f};
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

		isHit = IsCollision(aabb, line);

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		ImGui::Begin("Window");
		ImGui::DragFloat3("aabb.min", &aabb.min.x, 0.01f);
		ImGui::DragFloat3("aabb.max", &aabb.max.x, 0.01f);
		ImGui::DragFloat3("Segment.Origin", &line.origin.x, 0.01f);
		ImGui::DragFloat3("Segment.Diff", &line.diff.x, 0.01f);
		ImGui::End();
		if (isHit == true) {
			color = RED;
		} else {
			color = WHITE;
		}
		SetAABB(aabb);
		DrawGrid(viewProjectionMatrix, viewportMatrix, 5.0f, 26);
		Novice::DrawLine(int(start.x), int(start.y), int(end.x), int(end.y), WHITE);
		DrawAABB(aabb, viewProjectionMatrix, viewportMatrix, color);

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
