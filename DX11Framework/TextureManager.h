#pragma once
#include "GameDefinitions.h"

class TextureManager
{
private:
	std::vector<ID3D11ShaderResourceView*> _loadedTextures;
	ID3D11Device* _device;

public:
	TextureManager(ID3D11Device* device);

	HRESULT LoadGameTextures();

	ID3D11ShaderResourceView* GetTexture(TextureID texID);

};

