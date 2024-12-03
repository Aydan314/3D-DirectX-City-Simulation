#pragma once
#include "Camera.h"

class CameraManager
{
private:
	std::vector<Camera*> _cameras = {};
	int _currentCam = 0;

public:
	CameraManager();

	void AddCamera(Camera* camera);
	Camera* GetCameraAtIndex(int index);
	Camera* GetActiveCamera();

	void SetActiveCamera(int index);
	int GetActiveCameraIndex();

	void NextCamera();
	void PrevCamera();

	std::vector<Camera*> GetCameras();
};

