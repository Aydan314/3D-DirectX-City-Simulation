#include "MeshManager.h"

MeshManager::MeshManager(ID3D11Device* device)
{
    _device = device;
    LoadGameMeshes();
}

void MeshManager::LoadGameMeshes()
{
    for (auto meshName : GAME_MESHS) 
    {
        char* cstr = &meshName[0];
        _loadedMeshes.push_back(OBJLoader::Load(cstr, _device, true));
    }
}

MeshData* MeshManager::GetMesh(MeshID meshID)
{
    return &_loadedMeshes[meshID];
}
