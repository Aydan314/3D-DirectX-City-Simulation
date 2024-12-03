#pragma once
#include "GameObject.h"

struct SkyboxResources 
{
	ID3D11DepthStencilState* depthStencilState;
	ID3D11InputLayout* inputLayout;
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11RasterizerState* rasterizerState;
};

class Skybox : GameObject
{
private:
	SkyboxResources _resources;

public:
	Skybox(Transform transform, ResourceManagers resourceManagers, SkyboxResources resources);
	Skybox(GameObjectValues initValues, ResourceManagers resourceManagers, SkyboxResources resources);

	void Update(float deltaTime) override;
	void Draw(FrameData frameData) override;
};

