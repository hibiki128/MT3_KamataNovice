#include "Debug.h"
#include "imgui.h"
#include <algorithm>

void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, const float GridHalfWidth, const uint32_t Subdivision) {
	const float kGridHalfWidth = GridHalfWidth;                             // グリッドの半分の幅
	const uint32_t kSubdivision = Subdivision;                              // 分割数
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

	// 左右の線を引くためにzIndexのループも同様に処理
	for (uint32_t zIndex = 0; zIndex <= kSubdivision; ++zIndex) {
		// ワールド座標系上の始点と終点を求める
		Vector3 worldStartPos = {-kGridHalfWidth, 0.0f, -kGridHalfWidth + zIndex * kGridEvery};
		Vector3 worldEndPos = {kGridHalfWidth, 0.0f, -kGridHalfWidth + zIndex * kGridEvery};

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
	const uint32_t kSubdivision = 10;                                        // 分割数
	const float kLonEvery = 2.0f * std::numbers::pi_v<float> / kSubdivision; // 経度分割1つ分の角度
	const float kLatEvery = std::numbers::pi_v<float> / kSubdivision;        // 緯度分割1つ分の角度

	// 緯度の方向に分割　-π/2 ～ π/2
	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
		float lat = -std::numbers::pi_v<float> / 2.0f + kLatEvery * latIndex; // 現在の緯度

		// 経度の方向に分割 0 ～ 2π
		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
			float lon = lonIndex * kLonEvery; // 現在の経度

			// 現在の点を求める
			Vector3 start = {
			    sphere.center.x + sphere.radius * std::cosf(lat) * std::cosf(lon), sphere.center.y + sphere.radius * std::sinf(lat), sphere.center.z + sphere.radius * std::cosf(lat) * std::sinf(lon)};

			// 次の点を求める（経度方向）
			Vector3 end1 = {
			    sphere.center.x + sphere.radius * std::cosf(lat) * std::cosf(lon + kLonEvery),
			    sphere.center.y + sphere.radius * std::sinf(lat),
			    sphere.center.z + sphere.radius * std::cosf(lat) * std::sinf(lon + kLonEvery),
			};

			// 次の点を求める（緯度方向）
			Vector3 end2 = {
			    sphere.center.x + sphere.radius * std::cosf(lat + kLatEvery) * std::cosf(lon),
			    sphere.center.y + sphere.radius * std::sinf(lat + kLatEvery),
			    sphere.center.z + sphere.radius * std::cosf(lat + kLatEvery) * std::sinf(lon),
			};

			// 座標変換を行う
			start = Transform(start, viewProjectionMatrix);
			start = Transform(start, viewportMarix);
			end1 = Transform(end1, viewProjectionMatrix);
			end1 = Transform(end1, viewportMarix);
			end2 = Transform(end2, viewProjectionMatrix);
			end2 = Transform(end2, viewportMarix);

			// 線を描画
			Novice::DrawLine(int(start.x), int(start.y), int(end1.x), int(end1.y), color);
			Novice::DrawLine(int(start.x), int(start.y), int(end2.x), int(end2.y), color);
		}
	}
}

void DrawBall(const Ball& ball, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMarix, uint32_t color) {
	const uint32_t kSubdivision = 10;                                        // 分割数
	const float kLonEvery = 2.0f * std::numbers::pi_v<float> / kSubdivision; // 経度分割1つ分の角度
	const float kLatEvery = std::numbers::pi_v<float> / kSubdivision;        // 緯度分割1つ分の角度

	// 緯度の方向に分割　-π/2 ～ π/2
	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
		float lat = -std::numbers::pi_v<float> / 2.0f + kLatEvery * latIndex; // 現在の緯度

		// 経度の方向に分割 0 ～ 2π
		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
			float lon = lonIndex * kLonEvery; // 現在の経度

			// 現在の点を求める
			Vector3 start = {
			    ball.position.x + ball.radius * std::cosf(lat) * std::cosf(lon), ball.position.y + ball.radius * std::sinf(lat), ball.position.z + ball.radius * std::cosf(lat) * std::sinf(lon)};

			// 次の点を求める（経度方向）
			Vector3 end1 = {
			    ball.position.x + ball.radius * std::cosf(lat) * std::cosf(lon + kLonEvery),
			    ball.position.y + ball.radius * std::sinf(lat),
			    ball.position.z + ball.radius * std::cosf(lat) * std::sinf(lon + kLonEvery),
			};

			// 次の点を求める（緯度方向）
			Vector3 end2 = {
			    ball.position.x + ball.radius * std::cosf(lat + kLatEvery) * std::cosf(lon),
			    ball.position.y + ball.radius * std::sinf(lat + kLatEvery),
			    ball.position.z + ball.radius * std::cosf(lat + kLatEvery) * std::sinf(lon),
			};

			// 座標変換を行う
			start = Transform(start, viewProjectionMatrix);
			start = Transform(start, viewportMarix);
			end1 = Transform(end1, viewProjectionMatrix);
			end1 = Transform(end1, viewportMarix);
			end2 = Transform(end2, viewProjectionMatrix);
			end2 = Transform(end2, viewportMarix);

			// 線を描画
			Novice::DrawLine(int(start.x), int(start.y), int(end1.x), int(end1.y), color);
			Novice::DrawLine(int(start.x), int(start.y), int(end2.x), int(end2.y), color);
		}
	}
}

void DrawPlane(const Plane& plane, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	Vector3 center = plane.distance * plane.normal; // 1
	Vector3 perpendiculars[4];
	perpendiculars[0] = Normalize(Perpendicular(plane.normal));                             // 2
	perpendiculars[1] = {-perpendiculars[0].x, -perpendiculars[0].y, -perpendiculars[0].z}; // 3
	perpendiculars[2] = Cross(plane.normal, perpendiculars[0]);                             // 4
	perpendiculars[3] = {-perpendiculars[2].x, -perpendiculars[2].y, -perpendiculars[2].z}; // 5

	// 6
	Vector3 points[4];
	for (int32_t index = 0; index < 4; ++index) {
		Vector3 extend = 2.0f * perpendiculars[index];
		Vector3 point = center + extend;
		points[index] = Transform(Transform(point, viewProjectionMatrix), viewportMatrix);
	}
	Novice::DrawLine(static_cast<int>(points[0].x), static_cast<int>(points[0].y), static_cast<int>(points[2].x), static_cast<int>(points[2].y), color);
	Novice::DrawLine(static_cast<int>(points[2].x), static_cast<int>(points[2].y), static_cast<int>(points[1].x), static_cast<int>(points[1].y), color);
	Novice::DrawLine(static_cast<int>(points[1].x), static_cast<int>(points[1].y), static_cast<int>(points[3].x), static_cast<int>(points[3].y), color);
	Novice::DrawLine(static_cast<int>(points[3].x), static_cast<int>(points[3].y), static_cast<int>(points[0].x), static_cast<int>(points[0].y), color);
}

void DrawTriangle(const Triangle& triangle, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	Vector3 vertices[3];
	for (int i = 0; i < 3; ++i) {
		vertices[i] = Transform(triangle.vertices[i], viewProjectionMatrix);
		vertices[i] = Transform(vertices[i], viewportMatrix);
	}

	Novice::DrawTriangle(
	    static_cast<int>(vertices[0].x), static_cast<int>(vertices[0].y), static_cast<int>(vertices[1].x), static_cast<int>(vertices[1].y), static_cast<int>(vertices[2].x),
	    static_cast<int>(vertices[2].y), color, kFillModeWireFrame);
}

void DrawLineXY(const Vector3& start, const Vector3& end, uint32_t color) {
	Novice::DrawLine(static_cast<int>(start.x), static_cast<int>(start.y), static_cast<int>(end.x), static_cast<int>(end.y), color);
}

void DrawAABB(const AABB& aabb, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	Vector3 points[8];

	points[0] = aabb.min;
	points[1] = Vector3(aabb.min.x, aabb.max.y, aabb.min.z);
	points[2] = Vector3(aabb.max.x, aabb.max.y, aabb.min.z);
	points[3] = Vector3(aabb.max.x, aabb.min.y, aabb.min.z);

	points[4] = Vector3(aabb.min.x, aabb.min.y, aabb.max.z);
	points[5] = Vector3(aabb.min.x, aabb.max.y, aabb.max.z);
	points[6] = aabb.max;
	points[7] = Vector3(aabb.max.x, aabb.min.y, aabb.max.z);

	for (int i = 0; i < 8; ++i) {
		points[i] = Transform(points[i], viewProjectionMatrix);
		points[i] = Transform(points[i], viewportMatrix);
	}

	DrawLineXY(points[0], points[1], color);
	DrawLineXY(points[1], points[2], color);
	DrawLineXY(points[2], points[3], color);
	DrawLineXY(points[3], points[0], color);

	DrawLineXY(points[4], points[5], color);
	DrawLineXY(points[5], points[6], color);
	DrawLineXY(points[6], points[7], color);
	DrawLineXY(points[7], points[4], color);

	DrawLineXY(points[0], points[4], color);
	DrawLineXY(points[1], points[5], color);
	DrawLineXY(points[2], points[6], color);
	DrawLineXY(points[3], points[7], color);
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

Vector3 Project(const Vector3& v1, const Vector3& v2) {
	float dot = Dot(v1, v2);
	float lenSquared = LengthSquared(v2);
	float scalar = dot / lenSquared;
	return {v2.x * scalar, v2.y * scalar, v2.z * scalar};
}

Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t) { return Vector3(v1.x + t * (v2.x - v1.x), v1.y + t * (v2.y - v1.y), v1.z + t * (v2.z - v1.z)); }

void DrawOBB(const OBB& obb, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	Vector3 halfSizeX = obb.orientations[0] * obb.size.x;
	Vector3 halfSizeY = obb.orientations[1] * obb.size.y;
	Vector3 halfSizeZ = obb.orientations[2] * obb.size.z;

	Vector3 points[8];

	points[0] = obb.center + halfSizeX + halfSizeY + halfSizeZ;
	points[1] = obb.center + halfSizeX + halfSizeY - halfSizeZ;
	points[2] = obb.center + halfSizeX - halfSizeY + halfSizeZ;
	points[3] = obb.center + halfSizeX - halfSizeY - halfSizeZ;

	points[4] = obb.center - halfSizeX + halfSizeY + halfSizeZ;
	points[5] = obb.center - halfSizeX + halfSizeY - halfSizeZ;
	points[6] = obb.center - halfSizeX - halfSizeY + halfSizeZ;
	points[7] = obb.center - halfSizeX - halfSizeY - halfSizeZ;

	for (int i = 0; i < 8; ++i) {
		points[i] = Transform(points[i], viewProjectionMatrix);
		points[i] = Transform(points[i], viewportMatrix);
	}

	DrawLineXY(points[0], points[1], color);
	DrawLineXY(points[1], points[3], color);
	DrawLineXY(points[3], points[2], color);
	DrawLineXY(points[2], points[0], color);

	DrawLineXY(points[4], points[5], color);
	DrawLineXY(points[5], points[7], color);
	DrawLineXY(points[7], points[6], color);
	DrawLineXY(points[6], points[4], color);

	DrawLineXY(points[0], points[4], color);
	DrawLineXY(points[1], points[5], color);
	DrawLineXY(points[2], points[6], color);
	DrawLineXY(points[3], points[7], color);
}

void SetAABB(AABB& aabb) {
	aabb.min.x = (std::min)(aabb.min.x, aabb.max.x);
	aabb.max.x = (std::max)(aabb.min.x, aabb.max.x);
	aabb.min.y = (std::min)(aabb.min.y, aabb.max.y);
	aabb.max.y = (std::max)(aabb.min.y, aabb.max.y);
	aabb.min.z = (std::min)(aabb.min.z, aabb.max.z);
	aabb.max.z = (std::max)(aabb.min.z, aabb.max.z);
}

// 線分と点の最近接点を計算する関数
Vector3 ClosestPoint(const Vector3& point, const Segment& segment) {
	Vector3 pointOnLine = segment.origin; // 直線上の任意の点はセグメントの始点と同じと仮定
	float dot = Dot(segment.diff, point - pointOnLine);
	float lenSquared = LengthSquared(segment.diff);
	float t = dot / lenSquared;
	return {pointOnLine.x + segment.diff.x * t, pointOnLine.y + segment.diff.y * t, pointOnLine.z + segment.diff.z * t};
}

Vector3 Perpendicular(const Vector3& vector) {
	if (vector.x != 0.0f || vector.y != 0.0f) {
		return {-vector.y, vector.x, 0.0f};
	}
	return {0.0f, -vector.z, vector.y};
}

void CameraMove(Vector3& cameraRotate, Vector3& cameraTranslate, Vector2Int& clickPosition, char* keys, char* preKeys) {
	// カーソルを動かすときの感度
	float mouseSensitivity = 0.003f;
	// カメラの移動速度
	float moveSpeed = 0.005f;

	// 各フラグ
	static bool isLeftClicked = false;
	static bool isRightClicked = false;
	static bool isDebugCamera = false;

	// 回転を考慮する
	Matrix4x4 rotationMatrix = MakeRotateXYZMatrix(cameraRotate);
	Vector3 X = {1.0f, 0.0f, 0.0f};
	Vector3 Y = {0.0f, 1.0f, 0.0f};
	Vector3 Z = {0.0f, 0.0f, -1.0f};

	Vector3 rotatedX = Transform(X, rotationMatrix);
	Vector3 rotatedY = Transform(Y, rotationMatrix);
	Vector3 rotatedZ = Transform(Z, rotationMatrix);

	if (keys[DIK_SPACE] && preKeys[DIK_SPACE] == 0) {
		isDebugCamera = !isDebugCamera;
	}

	if (isDebugCamera) {

		/// ========カメラ操作========
		// カメラの回転を更新する
		if (Novice::IsPressMouse(1) == 1) {
			if (!isRightClicked) {
				// マウスがクリックされたときに現在のマウス位置を保存する
				Novice::GetMousePosition(&clickPosition.x, &clickPosition.y);
				isRightClicked = true;
			} else {
				// マウスがクリックされている間はカメラの回転を更新する
				Vector2Int currentMousePos;
				Novice::GetMousePosition(&currentMousePos.x, &currentMousePos.y);

				float deltaX = static_cast<float>(currentMousePos.x - clickPosition.x);
				float deltaY = static_cast<float>(currentMousePos.y - clickPosition.y);

				cameraRotate.x += deltaY * mouseSensitivity;
				cameraRotate.y += deltaX * mouseSensitivity;

				// 現在のマウス位置を保存する
				clickPosition = currentMousePos;
			}
		} else {
			// マウスがクリックされていない場合はフラグをリセットする
			isRightClicked = false;
		}

		// カメラの位置を更新する
		if (Novice::IsPressMouse(0) == 1) {
			if (!isLeftClicked) {
				// マウスがクリックされたときに現在のマウス位置を保存する
				Novice::GetMousePosition(&clickPosition.x, &clickPosition.y);
				isLeftClicked = true;
			} else {
				// マウスがクリックされている間はカメラの位置を更新する
				Vector2Int currentMousePos;
				Novice::GetMousePosition(&currentMousePos.x, &currentMousePos.y);

				float deltaX = static_cast<float>(currentMousePos.x - clickPosition.x);
				float deltaY = static_cast<float>(currentMousePos.y - clickPosition.y);

				cameraTranslate -= rotatedX * deltaX * mouseSensitivity;
				cameraTranslate += rotatedY * deltaY * mouseSensitivity;

				// 現在のマウス位置を保存する
				clickPosition = currentMousePos;
			}
		} else {
			// マウスがクリックされていない場合はフラグをリセットする
			isLeftClicked = false;
		}

		// マウスホイールの移動量を取得する
		int wheelDelta = -Novice::GetWheel();

		// マウスホイールの移動量に応じてカメラの移動を更新する
		cameraTranslate += rotatedZ * float(wheelDelta) * moveSpeed;
		/// =====================
	}
	ImGui::Begin("Camera");
	if (ImGui::TreeNode("Camera Explanation")) {
		ImGui::Text(
		    "SPACE : DebugCamera on:off\nDebugCamera = %d (0 = false , 1 = true)\nMouseRightClick : moveCameraRotate\nMouseLeftClick : moveCameraTranslate.xy\nMouseWheel : moveCameraTranslate.z",
		    isDebugCamera);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Camera Value")) {
		ImGui::DragFloat3("Translate", &cameraTranslate.x, 0.01f);
		ImGui::DragFloat3("Rotate", &cameraRotate.x, 0.01f);
		ImGui::TreePop();
	}
	ImGui::End();
}

bool IsCollision(const Sphere& s1, const Sphere& s2) {
	float distance = Length(s2.center - s1.center);

	if (distance <= s1.radius + s2.radius) {
		// 当たったとき
		return true;
	} else {
		// 当たっていないとき
		return false;
	}
};

bool IsCollision(const Sphere& sphere, const Plane& plane) {
	// 平面の法線方向に対する距離を計算する
	float distance = Dot(plane.normal, sphere.center) - plane.distance;

	// 球の半径と比較して判定する
	if (std::abs(distance) <= sphere.radius) {
		return true; // 衝突している
	} else {
		return false; // 衝突していない
	}
}

bool IsCollision(Segment& line, Plane& plane) {
	float dot = Dot(plane.normal, line.diff);

	if (dot == 0.0f) {
		return false;
	}

	// tを求める
	float t = (plane.distance - Dot(line.origin, plane.normal)) / dot;
	// tが線分の範囲内にあるかをチェックする
	if (t >= 0.0f && t <= 1.0f) {
		// 交差点が線分上にある
		return true;
	} else {
		// 交差点が線分の範囲外にある
		return false;
	}
}

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

bool IsCollision(const AABB& aabb1, const AABB& aabb2) {
	if ((aabb1.min.x <= aabb2.max.x && aabb1.max.x >= aabb2.min.x) && (aabb1.min.y <= aabb2.max.y && aabb1.max.y >= aabb2.min.y) && (aabb1.min.z <= aabb2.max.z && aabb1.max.z >= aabb2.min.z)) {
		return true;
	} else {
		return false;
	}
};

bool IsCollision(const AABB& aabb, const Sphere& sphere) {
	// 最近接点を求める
	Vector3 closestPoint{std::clamp(sphere.center.x, aabb.min.x, aabb.max.x), std::clamp(sphere.center.y, aabb.min.y, aabb.max.y), std::clamp(sphere.center.z, aabb.min.z, aabb.max.z)};

	// 最近接点と球の中心との距離を求める
	float distance = Length(closestPoint - sphere.center);

	// 距離が半径より小さければ衝突
	if (distance <= sphere.radius) {
		return true;
	} else {
		return false;
	}
}

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

bool IsCollision(const OBB& obb, const Sphere& sphere, const Matrix4x4& rotateMatrix) {

	// OBBのWorldMatrixを作成
	Matrix4x4 obbWorldMatrix = MakeOBBWorldMatrix(obb, rotateMatrix);

	// OBBのWorldMatrixの逆行列を取得
	Matrix4x4 obbWorldMatrixInverse = Inverse(obbWorldMatrix);

	// Sphereの中心点をOBBのローカル空間に変換
	Vector3 centerInOBBLocalSpace = Transform(sphere.center, obbWorldMatrixInverse);

	// OBBからAABBを作成
	AABB aabbOBBLocal = ConvertOBBToAABB(obb);

	// ローカル空間でのSphere
	Sphere sphereOBBLocal{centerInOBBLocalSpace, sphere.radius};

	Vector3 closestPoint = {
	    std::max(aabbOBBLocal.min.x, std::min(sphereOBBLocal.center.x, aabbOBBLocal.max.x)), std::max(aabbOBBLocal.min.y, std::min(sphereOBBLocal.center.y, aabbOBBLocal.max.y)),
	    std::max(aabbOBBLocal.min.z, std::min(sphereOBBLocal.center.z, aabbOBBLocal.max.z))};
	Vector3 distance = closestPoint - sphereOBBLocal.center;
	return Dot(distance, distance) <= (sphereOBBLocal.radius * sphereOBBLocal.radius);
}

bool IsCollision(const OBB& obb, const Segment& segment, const Matrix4x4& rotateMatrix) {
	// OBBのWorldMatrixを作成
	Matrix4x4 obbWorldMatrix = MakeOBBWorldMatrix(obb, rotateMatrix);

	// OBBのWorldMatrixの逆行列を取得
	Matrix4x4 obbWorldMatrixInverse = Inverse(obbWorldMatrix);

	// セグメントの始点と終点をOBBのローカル空間に変換
	Vector3 localOrigin = Transform(segment.origin, obbWorldMatrixInverse);
	Vector3 localEnd = Transform(segment.origin + segment.diff, obbWorldMatrixInverse);

	// OBBからAABBを作成
	AABB localAABB = ConvertOBBToAABB(obb);

	// ローカル空間でのセグメント
	Segment localSegment;
	localSegment.origin = localOrigin;
	localSegment.diff = localEnd - localOrigin;

	// ローカル空間でAABBとセグメントの衝突判定を行う
	return IsCollision(localAABB, localSegment);
}

// 衝突判定関数の実装
Matrix4x4 MakeOBBWorldMatrix(const OBB& obb, const Matrix4x4& rotateMatrix) {
	Matrix4x4 translationMatrix = MakeTranslateMatrix(obb.center);
	return Multiply(rotateMatrix, translationMatrix); // rotateMatrixを使用
}

AABB ConvertOBBToAABB(const OBB& obb) {
	AABB aabb;
	aabb.min = {-obb.size.x, -obb.size.y, -obb.size.z};
	aabb.max = {obb.size.x, obb.size.y, obb.size.z};
	return aabb;
}

// ばねの動き
void SpringMove(Spring& spring, Ball& ball, const Vector3& Gravity) {
	Vector3 diff = ball.position - spring.anchor;
	float length = Length(diff);
	if (length != 0.0f) {
		Vector3 direction = Normalize(diff);
		Vector3 restPosition = spring.anchor + direction * spring.naturalLength;
		Vector3 displacement = length * (ball.position - restPosition);
		Vector3 restoringForce = -spring.stiffness * displacement;
		Vector3 dampringForce = -spring.dampingCoefficient * ball.velocity;
		Vector3 force = restoringForce + dampringForce;
		ball.acceleration = force / ball.mass;
	}
	// 重力加速度を加算
	ball.acceleration += Gravity;

	// 加速度も速度もどちらも秒を基準にした値である
	// それが、1/60秒(deltaTime)適用されたと考える
	ball.velocity += ball.acceleration * DELTA_TIME;
	ball.position += ball.velocity * DELTA_TIME;
}

// 円運動
void CircularMotion(Vector3& p, Vector3& c, const float& r, float& angularVelocity, float& angle) {

	angle += angularVelocity * DELTA_TIME;

	p.x = c.x + std::cos(angle) * r;
	p.y = c.y + std::sin(angle) * r;
	p.z = c.z;
}

// 振り子運動
void PendulumMovement(Pendulum& pendulum, Vector3& p) {
	pendulum.angularAcceleration = -(9.8f / pendulum.length) * std::sin(pendulum.angle);
	pendulum.angularVelocity += pendulum.angularAcceleration * DELTA_TIME;
	pendulum.angle += pendulum.angularVelocity * DELTA_TIME;
	// pは振り子の先端の位置。取り付けたいものを取り付ければいい
	p.x = pendulum.anchor.x + std::sin(pendulum.angle) * pendulum.length;
	p.y = pendulum.anchor.y - std::cos(pendulum.angle) * pendulum.length;
	p.z = pendulum.anchor.z;
};

// 円錐振り子運動
void ConicalPendulumMove(ConicalPendulum& conicalPendulum, Ball& ball) {
	conicalPendulum.angularVelocity = std::sqrt(9.8f / (conicalPendulum.length * std::cos(conicalPendulum.halfApexAngle)));
	conicalPendulum.angle += conicalPendulum.angularVelocity * DELTA_TIME;
	float radius = std::sin(conicalPendulum.halfApexAngle) * conicalPendulum.length;
	float height = std::cos(conicalPendulum.halfApexAngle) * conicalPendulum.length;
	ball.position.x = conicalPendulum.anchor.x + std::cos(conicalPendulum.angle) * radius;
	ball.position.y = conicalPendulum.anchor.y - height;
	ball.position.z = conicalPendulum.anchor.z - std::sin(conicalPendulum.angle) * radius;
}