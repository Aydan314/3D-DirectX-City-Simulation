#include "MovingObject.h"

MovingObject::MovingObject(Transform transform, ResourceManagers resourceManagers) : GameObject(transform, resourceManagers)
{
}

MovingObject::MovingObject(GameObjectValues gameObjectValues, ResourceManagers resourceManagers) : GameObject(gameObjectValues, resourceManagers)
{
}

void MovingObject::Update(float deltaTime)
{
    if (_waypoints.size() > 0) 
    {
        _calculationDelay += deltaTime;

        // Mirrors thee object if its front is the wrong way round //
        if (_modelFlip) 
        {
            SetPosition
            ({
                GetPosition().x - (_speed * sin(GetRotation().y)),
                GetPosition().y,
                GetPosition().z - (_speed * cos(GetRotation().y))
                });
        }
        else 
        {
            SetPosition
            ({
                GetPosition().x + (_speed * sin(GetRotation().y)),
                GetPosition().y,
                GetPosition().z + (_speed * cos(GetRotation().y))
                });
        }

        if (!_hasTarget) 
        {
            CalculateTarget();
            _hasTarget = true;
        }
        // If the Object has reached the waypoint a new waypoint is set //
        if (CalculateDistance(GetPosition(), _waypoints[_currentWaypoint]) <= _acceptanceDistance) 
        {
            _currentWaypoint++;

            if (_currentWaypoint >= _waypoints.size()) _currentWaypoint = 0;

            _hasTarget = false;
        }

        // Objects heading will update every so often to avoid overshooting //
        if (_calculationDelay >= _calculationCooldown) 
        {
            CalculateTarget();
        }
    }

    for (GameObject* child : _children)
    {
        child->Update(deltaTime);
    }
}

void MovingObject::CalculateTarget()
{
    // Calculates the angle of travel to the target //
    float distX = _waypoints[_currentWaypoint].x - GetPosition().x;
    float distY = _waypoints[_currentWaypoint].y - GetPosition().y;
    float distZ = _waypoints[_currentWaypoint].z - GetPosition().z;

    float angleY = atan2(distX, distZ);

    if (_modelFlip) angleY += XM_PI;

    float angleX = GetRotation().x;
    float angleZ = GetRotation().z;

    SetRotation({angleX, angleY, angleZ});
}

float MovingObject::CalculateDistance(XMFLOAT3 posA, XMFLOAT3 posB)
{

    return sqrtf(pow(posA.x - posB.x, 2) + pow(posA.y - posB.y, 2) + pow(posA.z - posB.z, 2));
}

void MovingObject::SetWaypoints(std::vector<XMFLOAT3> waypoints)
{
    _waypoints = waypoints;
}

std::vector<XMFLOAT3> MovingObject::GetWaypoints()
{
    return _waypoints;
}

void MovingObject::SetSpeed(float speed)
{
    _speed = speed;
}

void MovingObject::SetAcceptanceDistance(float distance)
{
    _acceptanceDistance = distance;
}

void MovingObject::SetModelFlip(bool flip)
{
    _modelFlip = flip;
}

float MovingObject::GetSpeed()
{
    return _speed;
}
