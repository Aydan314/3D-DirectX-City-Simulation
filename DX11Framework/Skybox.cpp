#include "Skybox.h"

Skybox::Skybox(Transform transform, ResourceManagers resourceManagers, SkyboxResources resources) : GameObject(transform, resourceManagers)
{
    _resources = resources;
}

Skybox::Skybox(GameObjectValues initValues, ResourceManagers resourceManagers, SkyboxResources resources) : GameObject(initValues, resourceManagers)
{
    _resources = resources;
}

void Skybox::Update(float deltaTime)
{
    
}

void Skybox::Draw(FrameData frameData)
{
    _immediateContext->RSSetState(_resources.rasterizerState);

    SetPosition(frameData.cbData->CameraPosition);

    _immediateContext->VSSetShader(_resources.vertexShader, nullptr, 0);
    _immediateContext->PSSetShader(_resources.pixelShader, nullptr, 0);

    _immediateContext->IASetInputLayout(_resources.inputLayout);
    _immediateContext->OMSetDepthStencilState(_resources.depthStencilState, 0);

    // Binds the texture if it has one //
    
    if (_textureState == TEXTURE_STATE_DIFFUSE) _immediateContext->PSSetShaderResources(0, 1, &_texture);

    // Defines size of memory allocated for each vertex so the GPU can iterate through them //
    UINT stride = { sizeof(SimpleVertex) };

    // Defines which position in the list of vertecies to start rendering from //
    UINT offset = 0;

    // Writes the objects mesh to buffers to be accessed by the GPU //
    _immediateContext->IASetVertexBuffers(0, 1, &_mesh.vertexBuffer, &stride, &offset);
    _immediateContext->IASetIndexBuffer(_mesh.indexBuffer, DXGI_FORMAT_R16_UINT, 0);

    // Allocates memory in the GPU to this data and denies GPU access to it temporarily //
    _immediateContext->Map(frameData.constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, frameData.mappedSubresource);

    // Copies the objects matrix into the constant buffer to be wrote to GPU //
    frameData.cbData->World = XMMatrixTranspose(XMLoadFloat4x4(&_objectTransformMatrix));
    memcpy(frameData.mappedSubresource->pData, frameData.cbData, sizeof(ConstantBuffer));

    // Allows the GPU access to the buffer again //
    _immediateContext->Unmap(frameData.constantBuffer, 0);

    // Data in the buffer is drawn to screen //
    _immediateContext->DrawIndexed(_mesh.indexCount, 0, 0);
}
