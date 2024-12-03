#include "FirstPersonCamera.h"

FirstPersonCamera::FirstPersonCamera(float aspect, float FOV, Transform transform) : Camera(aspect, FOV, transform)
{
	_disableMovementY = true;
	UpdateViewMatrix();
}

void FirstPersonCamera::Update(float deltaTime)
{
	if (_rotationTime > 0.f)
	{
		// Lerp camera rotation //
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

	if (_positionTime > 0.f)
	{
		// Lerp camera position //
		float progress = 1 - (_positionTime / _positionLerpDuration);

		_positionVec = _fromPosition + (_fromPosition - _positionTarget) * progress;
		_positionTime -= deltaTime;
	}
	else
	{
		_positionTime = 0.f;

	}
	
	// Attach player to camera //
	if (_player) 
	{
		_player->SetPosition
		({
			GetPosition().x,
			GetPosition().y + _playerHeightOffset,
			GetPosition().z
		});
	}
	
}

void FirstPersonCamera::Draw(FrameData frameData)
{
	if (_player) _player->Draw(frameData);
}

void FirstPersonCamera::AttachPlayer(GameObject* player)
{
	_player = player;
}

void FirstPersonCamera::MoveCamera(Direction direction, float deltaTime, bool lerp)
{
	float velocity = _speed * deltaTime;

	// Check previous 2 inputs to make sure player cant diagonally go through collision //
	if (_inCollision && direction == _prevInput[0]) velocity = 0.f;
	else if (_inCollision && direction == _prevInput[1]) velocity = 0.f;

	// Handle camera movement using calculated movement vectors //

	switch (direction) {
	case DIRECTION_UP:
		break;
	case DIRECTION_DOWN:
		break;
	case DIRECTION_LEFT:
		AdjustPosition(_leftVec * velocity, lerp);
		break;
	case DIRECTION_RIGHT:
		AdjustPosition(_rightVec * velocity, lerp);
		break;
	case DIRECTION_FORWARD:
		AdjustPosition(_forwardVec * velocity, lerp);
		break;
	case DIRECTION_BACKWARD:
		AdjustPosition(_backwardVec * velocity, lerp);
		break;
	default:
		break;
	}

	// Store input //
	_prevInput[1] = _prevInput[0];
	_prevInput[0] = direction;
	
}

void FirstPersonCamera::SetPlayerHeightOffset(float offset)
{
	_playerHeightOffset = offset;
}
