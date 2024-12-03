#include "DirectionalLight.h"

DirectionalLight::DirectionalLight(LightValues lightValues)
{
    _lightValues.radius = 0;

	_lightValues = lightValues;
    _initLightValues = lightValues;
}

void DirectionalLight::SetDiffuseLight(XMFLOAT4 colour)
{
    _lightValues.diffuseLight = colour;
}

void DirectionalLight::SetAmbientLight(XMFLOAT4 colour)
{
    _lightValues.ambientLight = colour;
}

void DirectionalLight::SetSpecularLight(XMFLOAT4 colour)
{
    _lightValues.specularLight = colour;
}

void DirectionalLight::SetDiffuseMaterial(XMFLOAT4 material)
{
    _lightValues.diffuseMaterial = material;
}

void DirectionalLight::SetAmbientMaterial(XMFLOAT4 material)
{
    _lightValues.ambientMaterial = material;
}

void DirectionalLight::SetSpecularMaterial(XMFLOAT4 material)
{
    _lightValues.specularMaterial = material;
}

void DirectionalLight::SetSpecularPower(float amount)
{
    _lightValues.specPower = amount;
}

void DirectionalLight::SetLightDirection(XMFLOAT3 direction)
{
    _lightValues.lightDir = direction;
}

XMFLOAT4 DirectionalLight::GetDiffuseLight()
{
    return _lightValues.diffuseLight;
}

XMFLOAT4 DirectionalLight::GetAmbientLight()
{
    return _lightValues.ambientLight;
}

XMFLOAT4 DirectionalLight::GetSpecularLight()
{
    return _lightValues.specularLight;
}

XMFLOAT4 DirectionalLight::GetDiffuseMaterial()
{
    return _lightValues.diffuseMaterial;
}

XMFLOAT4 DirectionalLight::GetAmbientMaterial()
{
    return _lightValues.ambientMaterial;
}

XMFLOAT4 DirectionalLight::GetSpecularMaterial()
{
    return _lightValues.specularMaterial;
}

float DirectionalLight::GetSpecularPower()
{
    return _lightValues.specPower;
}

XMFLOAT3 DirectionalLight::GetLightDirection()
{
    return _lightValues.lightDir;
}

LightValues DirectionalLight::GetLightValues()
{
    return _lightValues;
}

XMFLOAT4 DirectionalLight::GetInitDiffuseLight()
{
    return _initLightValues.diffuseLight;
}

XMFLOAT4 DirectionalLight::GetInitAmbientLight()
{
    return _initLightValues.ambientLight;
}

XMFLOAT4 DirectionalLight::GetInitSpecularLight()
{
    return _initLightValues.specularLight;
}

XMFLOAT4 DirectionalLight::GetInitDiffuseMaterial()
{
    return _initLightValues.diffuseMaterial;
}

XMFLOAT4 DirectionalLight::GetInitAmbientMaterial()
{
    return _initLightValues.ambientMaterial;
}

XMFLOAT4 DirectionalLight::GetInitSpecularMaterial()
{
    return _initLightValues.specularMaterial;
}

float DirectionalLight::GetInitSpecularPower()
{
    return _initLightValues.specPower;
}

XMFLOAT3 DirectionalLight::GetInitLightDirection()
{
    return _initLightValues.lightDir;
}
