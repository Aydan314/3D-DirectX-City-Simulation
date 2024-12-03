#include "WorldEffectsObject.h"

WorldEffectsObject::WorldEffectsObject(Transform transform, ResourceManagers resourceManagers) : GameObject(transform, resourceManagers)
{

}

void WorldEffectsObject::Draw(FrameData frameData)
{

}

void WorldEffectsObject::Update(float deltaTime)
{
	if (_sun) 
	{
		_sun->SetLightDirection({ 0,cos(_sunAngle),sin(_sunAngle)});
		_sunAngle += 0.1f * deltaTime;

		if (_sunAngle >= XM_2PI) _sunAngle = 0.f;

		UpdateSunlightColour();
	}
}

void WorldEffectsObject::UpdateSunlightColour()
{
	float lightLevel = 0.5f * (1 + sin(_sunAngle));

	if (lightLevel < MIN_DARKNESS) lightLevel = MIN_DARKNESS;
	else if (lightLevel > (1 - MIN_DARKNESS)) lightLevel = (1 - MIN_DARKNESS);

	float sunRedHue = 0.f;

	// Sunrise when gradient of curve more than specified value //
	if (cos(_sunAngle) >= MIN_RED_HUE_START)
	{
		sunRedHue = ((cos(_sunAngle)) - MIN_RED_HUE_START) * RED_HUE_INTENSITY;
	}

	// Sunset when gradient of curve if less than specified value //
	if (cos(_sunAngle) <= -MIN_RED_HUE_START) 
	{
		sunRedHue = ( - 1.f * ((cos(_sunAngle)) + MIN_RED_HUE_START)) * RED_HUE_INTENSITY;
	}

	// Light values are updated //
	_sun->SetAmbientLight
	(
		{ 
		(_sun->GetInitAmbientLight().x + sunRedHue) * lightLevel,
		_sun->GetInitAmbientLight().y * lightLevel, 
		_sun->GetInitAmbientLight().z * lightLevel,
		_sun->GetInitAmbientLight().w
		}
	);
	_sun->SetDiffuseLight
	(
		{
		(_sun->GetInitDiffuseLight().x + sunRedHue) * lightLevel,
		_sun->GetInitDiffuseLight().y * lightLevel,
		_sun->GetInitDiffuseLight().z * lightLevel,
		_sun->GetInitDiffuseLight().w
		}
	);
	_sun->SetSpecularLight
	(
		{
		(_sun->GetInitSpecularLight().x + sunRedHue) * lightLevel,
		_sun->GetInitSpecularLight().y * lightLevel,
		_sun->GetInitSpecularLight().z * lightLevel,
		_sun->GetInitSpecularLight().w
		}
	);

	
}

void WorldEffectsObject::SetLightAsSun(DirectionalLight* light)
{
	_sun = light;
}
