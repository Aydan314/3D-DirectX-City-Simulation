#pragma once
#include "GameObject.h"
#include "DirectionalLight.h"

const float MIN_DARKNESS = 0.1f;
const float MIN_RED_HUE_START = 0.8f;
const float RED_HUE_INTENSITY = 1.2f;


class WorldEffectsObject : GameObject
{
private:
	DirectionalLight* _sun = nullptr;
	float _sunAngle = 0.f;

public:
	WorldEffectsObject(Transform transform, ResourceManagers resourceManagers);

	void Draw(FrameData frameData) override;
	void Update(float deltaTime) override;
	void UpdateSunlightColour();

	void SetLightAsSun(DirectionalLight* light);
};

