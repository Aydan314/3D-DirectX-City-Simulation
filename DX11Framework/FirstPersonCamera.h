#pragma once
#include "Camera.h"
class FirstPersonCamera : public Camera
{
private:
	GameObject* _player = nullptr;
	float _playerHeightOffset = -1.f;

public:
	FirstPersonCamera(float aspect, float FOV = 45, Transform transform = Transform());

	void Update(float deltaTime) override;
	void Draw(FrameData frameData) override;

	void AttachPlayer(GameObject* player);

	void MoveCamera(Direction direction, float deltaTime, bool lerp = false) override;
	void SetPlayerHeightOffset(float offset);
};

