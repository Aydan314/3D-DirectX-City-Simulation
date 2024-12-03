#include "CameraManager.h"

CameraManager::CameraManager()
{
}

void CameraManager::AddCamera(Camera* camera)
{
	_cameras.push_back(camera);
}

Camera* CameraManager::GetCameraAtIndex(int index)
{
	if (index < _cameras.size()) 
	{
		return _cameras[index];
	}
	return nullptr;
}

Camera* CameraManager::GetActiveCamera()
{
	return _cameras[_currentCam];
}

void CameraManager::SetActiveCamera(int index)
{
	if (index < _cameras.size()) 
	{
		_currentCam = index;
	}
}

int CameraManager::GetActiveCameraIndex()
{
	return _currentCam;
}

void CameraManager::NextCamera()
{
	_currentCam++;
	if (_currentCam >= _cameras.size()) _currentCam = 0;
}

void CameraManager::PrevCamera()
{
	_currentCam--;
	if (_currentCam < 0) _currentCam = _cameras.size() - 1;
}

std::vector<Camera*> CameraManager::GetCameras()
{
	return _cameras;
}
