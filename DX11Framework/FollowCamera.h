#pragma once
#include "Camera.h"
#include "GameObject.h"

class FollowCamera : public Camera
{
protected:
	GameObject* _target;
	XMFLOAT3 _followOffset = { 0,3,0 };

public:
	FollowCamera(float aspect, float FOV = 45, Transform transform = Transform());

	void Update(float deltaTime) override;
	void AdjustPosition(XMVECTOR posAdjustment, bool lerp) override;
	void SetPosition(XMFLOAT3 position) override;

	void SetTarget(GameObject* target);
	void SetFollowOffset(XMFLOAT3 followOffset);

	GameObject* GetTarget();
	XMFLOAT3 GetFollowOffset();
};

