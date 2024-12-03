#include "FollowCamera.h"

FollowCamera::FollowCamera(float aspect, float FOV, Transform transform) : Camera(aspect, FOV,transform)
{
	_target = nullptr;
}

void FollowCamera::Update(float deltaTime)
{
	if (_rotationTime > 0.f)
	{
		float progress = 1 - (_rotationTime / _rotationLerpDuration);

		_transform.rotation.x = LerpToValue(_fromRotation.x, _rotationTarget.x, progress);
		_transform.rotation.y = LerpToValue(_fromRotation.y, _rotationTarget.y, progress);
		_transform.rotation.z = LerpToValue(_fromRotation.z, _rotationTarget.z, progress);

		// Prevent camera from looking too far up or down //
		if (_transform.rotation.x > XM_PI / 2.f && _constrainPitch) _transform.rotation.x = XM_PI / 2.f;
		if (_transform.rotation.x < -XM_PI / 2.f && _constrainPitch) _transform.rotation.x = -XM_PI / 2.f;

		_rotationTime -= deltaTime;

		UpdateViewMatrix();
	}
	else
	{
		_rotationTime = 0.f;
	}

	// If Camera attached to an object it will follow it //
	if (_parent)
	{
		SetPosition
		(
			{
			_parent->GetPosition().x + _followOffset.x,
			_parent->GetPosition().y + _followOffset.y,
			_parent->GetPosition().z + _followOffset.z
			}
		);
	}

	for (GameObject* child : _children)
	{
		child->Update(deltaTime);
	}
}

void FollowCamera::SetTarget(GameObject* target)
{
	_target = target;
	_target->AddChild(this);
}

void FollowCamera::SetFollowOffset(XMFLOAT3 followOffset)
{
	_followOffset = followOffset;
}

void FollowCamera::AdjustPosition(XMVECTOR posAdjustment, bool lerp)
{
	_positionVec += posAdjustment;

	XMStoreFloat3(&_followOffset, _positionVec);
	
	UpdateViewMatrix();
}

void FollowCamera::SetPosition(XMFLOAT3 position)
{
	_transform.position = position;
	UpdateObjectMatrix();
	UpdateViewMatrix();
}

GameObject* FollowCamera::GetTarget()
{
	return _target;
}

XMFLOAT3 FollowCamera::GetFollowOffset()
{
	return _followOffset;
}
