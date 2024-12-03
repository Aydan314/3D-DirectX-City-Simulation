#pragma once
#include "GameDefinitions.h"

class InputManager
{
private:
	XMFLOAT2 _prevMousePos = {0,0};
	XMFLOAT2 _mousePos = {0,0};
	HWND* _windowHandle;
	float _mouseSensitivity = 0.025f;

public:
	InputManager(HWND* windowHandle );

	void Update(float deltaTime);
	void SetMouseSensitivity(float sensitivity);

	XMFLOAT2 GetMousePos();
	XMFLOAT2 GetMouseMovement();
	float GetMouseSensitivity();
};

