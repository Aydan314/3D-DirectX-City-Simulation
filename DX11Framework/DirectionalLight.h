#pragma once
#include "GameDefinitions.h"

class DirectionalLight
{
protected:
    LightValues _lightValues;
    LightValues _initLightValues;

public:
    DirectionalLight(LightValues lightValues);

    // Set Methods //
    void SetDiffuseLight(XMFLOAT4 colour);
    void SetAmbientLight(XMFLOAT4 colour);
    void SetSpecularLight(XMFLOAT4 colour);

    void SetDiffuseMaterial(XMFLOAT4 material);
    void SetAmbientMaterial(XMFLOAT4 material);
    void SetSpecularMaterial(XMFLOAT4 material);

    void SetSpecularPower(float amount);
    void SetLightDirection(XMFLOAT3 direction);

    // Get Methods //
    XMFLOAT4 GetDiffuseLight();
    XMFLOAT4 GetAmbientLight();
    XMFLOAT4 GetSpecularLight();

    XMFLOAT4 GetDiffuseMaterial();
    XMFLOAT4 GetAmbientMaterial();
    XMFLOAT4 GetSpecularMaterial();

    float GetSpecularPower();
    XMFLOAT3 GetLightDirection();

    LightValues GetLightValues();

    XMFLOAT4 GetInitDiffuseLight();
    XMFLOAT4 GetInitAmbientLight();
    XMFLOAT4 GetInitSpecularLight();

    XMFLOAT4 GetInitDiffuseMaterial();
    XMFLOAT4 GetInitAmbientMaterial();
    XMFLOAT4 GetInitSpecularMaterial();

    float GetInitSpecularPower();
    XMFLOAT3 GetInitLightDirection();
};

