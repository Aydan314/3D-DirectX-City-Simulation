#pragma once
#include "GameObject.h"
class MovingObject : GameObject
{
private:
	std::vector<XMFLOAT3> _waypoints;
	float _speed = 1.f;
	float _acceptanceDistance = 1.f;
	int _currentWaypoint = 0;
	float _calculationCooldown = 0.f;
	float _calculationDelay = 1000.f;
	bool _hasTarget = false;
	bool _modelFlip = false;

public:
	MovingObject(Transform transform, ResourceManagers resourceManagers);
	MovingObject(GameObjectValues gameObjectValues, ResourceManagers resourceManagers);

	void Update(float deltaTime) override;
	void CalculateTarget();
	float CalculateDistance(XMFLOAT3 posA, XMFLOAT3 posB);

	void SetWaypoints(std::vector<XMFLOAT3> waypoints);
	void SetSpeed(float speed);
	void SetAcceptanceDistance(float distance);
	void SetModelFlip(bool flip);

	std::vector<XMFLOAT3> GetWaypoints();
	float GetSpeed();
	
};

