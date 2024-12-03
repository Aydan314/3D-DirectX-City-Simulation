#include "TextureManager.h"

TextureManager::TextureManager(ID3D11Device* device)
{
	_device = device;
	LoadGameTextures();
}

HRESULT TextureManager::LoadGameTextures()
{
	HRESULT hr = S_OK;

	// Iterate through all textures and add them into the game //
	for (int i = 0; i < GAME_TEXTURES.size(); i++) 
	{

		ID3D11ShaderResourceView* newTex = nullptr;
		

		hr = CreateDDSTextureFromFile(_device, GAME_TEXTURES[i], nullptr, &newTex);
		if (FAILED(hr)) return hr;

		_loadedTextures.push_back(newTex);

	}

	return S_OK;
}

ID3D11ShaderResourceView* TextureManager::GetTexture(TextureID texID)
{
	return _loadedTextures[texID];
}
