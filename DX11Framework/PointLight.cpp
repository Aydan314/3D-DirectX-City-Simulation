#include "PointLight.h"

PointLight::PointLight(LightValues lightValues) : DirectionalLight(lightValues)
{
}

float PointLight::GetRadius()
{
	return _lightValues.radius;
}

XMFLOAT3 PointLight::GetPosition()
{
	return _lightValues.lightPos;
}

void PointLight::SetRadius(float radius)
{
	_lightValues.radius = radius;
}

void PointLight::SetPosition(XMFLOAT3 position)
{
	_lightValues.lightPos = position;
}
