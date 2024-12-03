#pragma once
#include "GameObject.h"

const XMVECTOR FORWARD_VECTOR = { 0,0,1.0f };
const XMVECTOR BACKWARD_VECTOR = { 0,0,-1.0f };

const XMVECTOR LEFT_VECTOR = { -1.0,0,0 };
const XMVECTOR RIGHT_VECTOR = { 1.0,0,0 };

const XMVECTOR UP_VECTOR = { 0,1.0f,0 };
const XMVECTOR DOWN_VECTOR = { 0,-1.0f,0 };


class Camera : public GameObject
{
protected:
	XMVECTOR _positionTarget;
	XMVECTOR _fromPosition;
	float _positionLerpDuration = 1000.f;
	float _positionTime = 0.f;

	XMFLOAT3 _rotationTarget;
	XMFLOAT3 _fromRotation;
	float _rotationLerpDuration = 100.f;
	float _rotationTime = 0.f;

	XMMATRIX _viewMatrix;
	XMFLOAT4X4 _projectionMatrix;
	

	XMVECTOR _forwardVec;
	XMVECTOR _backwardVec;
	XMVECTOR _leftVec;
	XMVECTOR _rightVec;
	XMVECTOR _upVec;
	XMVECTOR _downVec;
	XMVECTOR _positionVec = {0,0,0};

	Direction _prevInput[2];

	float _zoom;
	float _aspect;
	float _FOV;
	float _baseSpeed = 40.f;
	float _sprintSpeed = 160.f;
	float _speed = _baseSpeed;
	bool _constrainPitch = true;
	bool _disableMovementY = false;
	bool _inCollision = false;

public:

	Camera(float aspect, float FOV = 45, Transform transform = Transform());

	void UpdateViewMatrix();
	void UpdateProjectionMatrix();
	void Update(float deltaTime) override;
	void Draw(FrameData frameData) override;

	void AdjustRotation(XMFLOAT3 rotAdjustment, bool lerp = false);
	void AdjustPosition(XMFLOAT3 posAdjustment);
	virtual void AdjustPosition(XMVECTOR posAdjustment, bool lerp = false);

	void SetFOV(float FOV);
	void SetSpeed(float speed);
	void SetPosition(XMFLOAT3 position) override;
	void SetRotation(XMFLOAT3 rotation) override;
	void SetTransform(Transform transform) override;
	void SetBaseSpeed(float speed);
	void SetSprintSpeed(float speed);
	void SetInCollision(bool state);

	void SetConstrainPitch(bool state);

	virtual void MoveCamera(Direction direction, float deltaTime, bool lerp = false);

	float LerpToValue(float fromValue, float toValue, float progress);

	XMVECTOR GetForwardVector();
	XMVECTOR GetBackwardVector();
	XMVECTOR GetUpVector();
	XMVECTOR GetDownVector();
	XMVECTOR GetLeftVector();
	XMVECTOR GetRightVector();

	XMMATRIX* GetViewMatrix();
	XMFLOAT4X4* GetProjectionMatrix();
	float GetFOV();
	float GetSpeed();
	float GetBaseSpeed();
	float GetSprintSpeed();
	
};


