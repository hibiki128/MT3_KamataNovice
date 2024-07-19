#include "Debug.h"
#include "Matrix/MyMath.h"
#include "Matrix4x4.h"
#include "Vector2.h"
#include "algorithm"
#include "imgui.h"
#include <Novice.h>
#include <cmath>

const char kWindowTitle[] = "LE2B_20_ハギワラ_ヒビキ";

struct Capsule {
	Segment segment;
	float radius;
};

Vector3 Reflect(const Vector3& input, const Vector3& normal) {
	float dotProduct = Dot(input, normal);
	Vector3 projection = normal * (2.0f * dotProduct);
	return input - projection;
}

bool IsCollision(const Capsule& capsule, const Plane& plane) {
	Vector3 end1 = capsule.segment.origin;
	Vector3 end2 = capsule.segment.origin + capsule.segment.diff;

	float distance1 = Dot(end1, plane.normal) - plane.distance;
	float distance2 = Dot(end2, plane.normal) - plane.distance;

	return (std::abs(distance1) <= capsule.radius || std::abs(distance2) <= capsule.radius);
}

void ReflectMove(Ball& ball, Plane& plane, float e) {
	ball.velocity += ball.acceleration * DELTA_TIME;
	ball.position += ball.velocity * DELTA_TIME;

	Capsule capsule;
	capsule.segment.origin = ball.position;
	capsule.segment.diff = ball.velocity * DELTA_TIME;
	capsule.radius = ball.radius;

	if (IsCollision(capsule, plane)) {
		Vector3 reflected = Reflect(ball.velocity, plane.normal);
		Vector3 planeToBall = ball.position - plane.normal * (Dot(ball.position, plane.normal) - plane.distance);

		if (Dot(ball.position - planeToBall, plane.normal) < 0) {
			ball.position = planeToBall;
		}

		ball.velocity = reflected * e;
	}
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	Vector3 cameraTranslate{0.0f, 1.9f, -6.49f};
	Vector3 cameraRotate{0.26f, 0.0f, 0.0f};
	Vector3 cameraPosition{0.0f, 1.0f, -5.0f};
	const int kWindowWidth = 1280;
	const int kWindowHeight = 720;
	Vector2Int ClickPosition = {};

	bool isStart = false;

	Plane plane;
	plane.normal = Normalize({-0.2f, 1.2f, -0.3f});
	plane.distance = 0.0f;

	Ball ball{};
	ball.position = {0.8f, 1.2f, 0.3f};
	ball.acceleration = {0.0f, -9.8f, 0.0f};
	ball.mass = 2.0f;
	ball.radius = 0.05f;
	ball.color = WHITE;

	float e = 0.7f;

	char keys[256] = {0};
	char preKeys[256] = {0};

	while (Novice::ProcessMessage() == 0) {
		Novice::BeginFrame();

		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		Matrix4x4 cameraMatrix = MakeAffineMatrix({1.0f, 1.0f, 1.0f}, cameraRotate, cameraPosition + cameraTranslate);
		Matrix4x4 viewMatrix = Inverse(cameraMatrix);
		Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(kWindowWidth) / float(kWindowHeight), 0.1f, 100.0f);
		Matrix4x4 viewProjectionMatrix = Multiply(viewMatrix, projectionMatrix);
		Matrix4x4 viewportMatrix = MakeViewPortMatrix(0, 0, float(kWindowWidth), float(kWindowHeight), 0.0f, 1.0f);

		CameraMove(cameraRotate, cameraTranslate, ClickPosition, keys, preKeys);

		if (isStart) {
			ReflectMove(ball, plane, e);
		}
		if (ball.position.y < -0.6f) {
			ball.position = {0.8f, 1.2f, 0.3f};
			ball.velocity = {0.0f, 0.0f, 0.0f};
		}

		ImGui::Begin("Window");
		if (ImGui::Button("Start")) {
			isStart = !isStart;
		}
		ImGui::End();

		DrawGrid(viewProjectionMatrix, viewportMatrix, 2.0f, 10);
		DrawPlane(plane, viewProjectionMatrix, viewportMatrix, WHITE);
		DrawBall(ball, viewProjectionMatrix, viewportMatrix, ball.color);

		Novice::EndFrame();

		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	Novice::Finalize();
	return 0;
}