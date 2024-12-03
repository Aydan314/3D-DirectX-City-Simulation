#include "InputManager.h"

InputManager::InputManager(HWND* windowHandle)
{
	_windowHandle = windowHandle;
}

void InputManager::Update(float deltaTime)
{
	// Update current and previous mouse positions //
	POINT point;
	GetCursorPos(&point);

	RECT screenRect;
	GetClientRect(*_windowHandle,&screenRect);

	ScreenToClient(*_windowHandle, &point);

	int screenWidth = screenRect.right - screenRect.left;
	int screenHeight = screenRect.bottom - screenRect.top;

	_prevMousePos = _mousePos;

	_mousePos = { (point.x - screenWidth / 2.f), (point.y - screenHeight / 2.f) };

	_mousePos.y *= -2.f;
	_mousePos.x *= -2.0f;

}

void InputManager::SetMouseSensitivity(float sensitivity)
{
	_mouseSensitivity = sensitivity;
}

XMFLOAT2 InputManager::GetMousePos()
{
	return _mousePos;
}

XMFLOAT2 InputManager::GetMouseMovement()
{
	return { (_prevMousePos.x - _mousePos.x) * _mouseSensitivity, (_prevMousePos.y - _mousePos.y) * _mouseSensitivity };
}

float InputManager::GetMouseSensitivity()
{
	return _mouseSensitivity;
}
