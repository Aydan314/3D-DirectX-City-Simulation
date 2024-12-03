#include "Camera.h"

Camera::Camera(float aspect, float FOV, Transform transform) : GameObject(transform, ResourceManagers())
{
	// Set initial values and position the camera //
	_aspect = aspect;
	_FOV = FOV;

	UpdateViewMatrix();
	UpdateProjectionMatrix();
}

void Camera::UpdateViewMatrix()
{
	XMVECTOR posVector = XMLoadFloat3(&_transform.position);

	// Calculates matrix to apply to all vectors using the cameras current rotation //
	XMMATRIX camRotationMatrix = XMMatrixRotationRollPitchYaw(_transform.rotation.x, _transform.rotation.y, _transform.rotation.z);

	// Calculates where the camera is looking //
	XMVECTOR camTarget = XMVector3TransformCoord(FORWARD_VECTOR, camRotationMatrix);

	camTarget += posVector;

	// Calculates which orientation the camera is in //
	XMVECTOR upDir = XMVector3TransformCoord(UP_VECTOR, camRotationMatrix);
	
	// Final matrix is calculated //
	_viewMatrix = XMMatrixLookAtLH(posVector,camTarget,upDir);
	
	float xRot = _transform.rotation.x;
	if (_disableMovementY) xRot = 0;

	XMMATRIX vecRotationMatrix = XMMatrixRotationRollPitchYaw(xRot, _transform.rotation.y,_transform.rotation.z);

	// Camera movement vectors are rotated with camera //
	_forwardVec = XMVector3TransformCoord(FORWARD_VECTOR, vecRotationMatrix);
	_backwardVec = XMVector3TransformCoord(BACKWARD_VECTOR, vecRotationMatrix);
	_leftVec = XMVector3TransformCoord(LEFT_VECTOR, vecRotationMatrix);
	_rightVec = XMVector3TransformCoord(RIGHT_VECTOR, vecRotationMatrix);
	_upVec = XMVector3TransformCoord(UP_VECTOR, vecRotationMatrix);
	_downVec = XMVector3TransformCoord(DOWN_VECTOR, vecRotationMatrix);

}

void Camera::UpdateProjectionMatrix()
{
	XMMATRIX perspective = XMMatrixPerspectiveFovLH(XMConvertToRadians(_FOV), _aspect, 0.01f, 1000.0f);
	XMStoreFloat4x4(&_projectionMatrix, perspective);
}

void Camera::Update(float deltaTime)
{
	if (_rotationTime > 0.f)
	{
		// Handles rotation lerp //
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
		// Handles position lerp //
		float progress = 1 - (_positionTime / _positionLerpDuration);

		_positionVec = _fromPosition + (_fromPosition - _positionTarget) * progress;
		_positionTime -= deltaTime;
	}
	else
	{
		_positionTime = 0.f;

	}

	for (GameObject* child : _children)
	{
		child->Update(deltaTime);
	}
}

void Camera::Draw(FrameData frameData)
{

	for (GameObject* child : _children)
	{
		child->Draw(frameData);
	}
}

void Camera::AdjustRotation(XMFLOAT3 rotAdjustment, bool lerp)
{
	if (lerp) 
	{
		// Sets up variables for lerp //
		_rotationTarget.x = _transform.rotation.x + rotAdjustment.x;
		_rotationTarget.y = _transform.rotation.y + rotAdjustment.y;
		_rotationTarget.z = _transform.rotation.z + rotAdjustment.z;

		_fromRotation = _transform.rotation;

		if (_rotationTime == 0) 
		{
			_rotationTime = _rotationLerpDuration;
		}
	}
	else 
	{
		_transform.rotation.x += rotAdjustment.x;
		_transform.rotation.y += rotAdjustment.y;
		_transform.rotation.z += rotAdjustment.z;

		// Prevent camera from looking too far up or down //
		if (_transform.rotation.x > XM_PI / 2.f && _constrainPitch) _transform.rotation.x = XM_PI / 2.f;
		if (_transform.rotation.x < -XM_PI / 2.f && _constrainPitch) _transform.rotation.x = -XM_PI / 2.f;

		UpdateViewMatrix();
	}
}

void Camera::AdjustPosition(XMFLOAT3 posAdjustment)
{
	_transform.position.x += posAdjustment.x;
	_transform.position.y += posAdjustment.y;
	_transform.position.z += posAdjustment.z;
	_positionVec = XMLoadFloat3(&_transform.position);
	UpdateViewMatrix();
}

void Camera::AdjustPosition(XMVECTOR posAdjustment, bool lerp)
{
	if (lerp) 
	{
		// Sets up variables for lerp //
		_positionTarget = _positionVec + posAdjustment;
		_fromPosition = _positionVec;

		if (_positionTime == 0) 
		{
			_positionTime = _positionLerpDuration;
		}
	}
	else 
	{
		_positionVec += posAdjustment;
		XMStoreFloat3(&_transform.position, _positionVec);
		UpdateViewMatrix();
	}
}

void Camera::SetPosition(XMFLOAT3 position)
{
	
	_transform.position = position;
	_positionVec = XMLoadFloat3(&_transform.position);
	UpdateObjectMatrix();
	UpdateViewMatrix();
}

void Camera::SetRotation(XMFLOAT3 rotation)
{
	_transform.rotation = rotation;

	// Prevent camera from looking too far up or down //
	if (_transform.rotation.x > XM_PI / 2.f && _constrainPitch) _transform.rotation.x = XM_PI / 2.f;
	if (_transform.rotation.x < -XM_PI / 2.f && _constrainPitch) _transform.rotation.x = -XM_PI / 2.f;

	UpdateObjectMatrix();
	UpdateViewMatrix();
}

void Camera::SetTransform(Transform transform)
{
	_transform = transform;
	UpdateViewMatrix();
}

void Camera::SetBaseSpeed(float speed)
{
	_baseSpeed = speed;
}

void Camera::SetSprintSpeed(float speed)
{
	_sprintSpeed = speed;
}

void Camera::SetInCollision(bool state)
{
	_inCollision = state;
}

void Camera::SetFOV(float FOV)
{
	_FOV = FOV;

	if (_FOV <= 0.01f) 
	{
		_FOV = 0.01f;
	}

	UpdateProjectionMatrix();
}

void Camera::SetSpeed(float speed)
{
	_speed = speed;
}

void Camera::SetConstrainPitch(bool state)
{
	_constrainPitch = state;
}

void Camera::MoveCamera(Direction direction, float deltaTime, bool lerp)
{
	float velocity = _speed * deltaTime;

	// Handle camera movement using calculated movement vectors //

	switch (direction) {
	case DIRECTION_UP:
		AdjustPosition(_upVec * velocity, lerp);
		break;
	case DIRECTION_DOWN:
		AdjustPosition(_downVec * velocity, lerp);
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

	_prevInput[0] = direction;
}

float Camera::LerpToValue(float fromValue, float toValue, float progress)
{
	return (fromValue + ((toValue - fromValue) * progress));
}

XMVECTOR Camera::GetForwardVector()
{
	return _forwardVec;
}

XMVECTOR Camera::GetBackwardVector()
{
	return _backwardVec;
}

XMVECTOR Camera::GetUpVector()
{
	return _upVec;
}

XMVECTOR Camera::GetDownVector()
{
	return _downVec;
}

XMVECTOR Camera::GetLeftVector()
{
	return _leftVec;
}

XMVECTOR Camera::GetRightVector()
{
	return _rightVec;
}

XMMATRIX* Camera::GetViewMatrix()
{
	return &_viewMatrix;
}

XMFLOAT4X4* Camera::GetProjectionMatrix()
{
	return &_projectionMatrix;
}

float Camera::GetFOV()
{
	return _FOV;
}

float Camera::GetSpeed()
{
	return _speed;
}

float Camera::GetBaseSpeed()
{
	return _baseSpeed;
}

float Camera::GetSprintSpeed()
{
	return _sprintSpeed;
}
