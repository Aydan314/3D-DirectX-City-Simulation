#pragma once
#include "GameDefinitions.h"
#include "OBJLoader.h"

class MeshManager
{
private:
	std::vector<MeshData> _loadedMeshes;
	ID3D11Device* _device;

public:
	MeshManager(ID3D11Device* device);

	void LoadGameMeshes();

	MeshData* GetMesh(MeshID meshID);
};

