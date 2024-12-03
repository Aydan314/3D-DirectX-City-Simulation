#pragma once
#include "DirectionalLight.h"

class PointLight : public DirectionalLight
{
private:

public:
	PointLight(LightValues lightValues);

	float GetRadius();
	XMFLOAT3 GetPosition();

	void SetRadius(float radius);
	void SetPosition(XMFLOAT3 position);


};

