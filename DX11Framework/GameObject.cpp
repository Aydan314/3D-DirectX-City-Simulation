#include "GameObject.h"

GameObject::GameObject(Transform initTransform, ResourceManagers resourceManagers)
{
    SetTransform(initTransform);
    // defines the surface to render to //
    _immediateContext = resourceManagers.immediateContext;

    _textureManager = resourceManagers.textureManager;
    _meshManager = resourceManagers.meshManager;

    
}

GameObject::GameObject(GameObjectValues initValues, ResourceManagers resourceManagers)
{
    _name = initValues.name;
    _collisionEnabled = initValues.collisionEnabled;
    _hitbox = initValues.hitbox;
    SetTransform(initValues.transform);

    // defines the surface to render to //
    _immediateContext = resourceManagers.immediateContext;

    // Get access to all textures and mesh loaded into the game //
    _textureManager = resourceManagers.textureManager;
    _meshManager = resourceManagers.meshManager;

    SetMesh(*_meshManager->GetMesh((MeshID)initValues.meshID));
    SetTexture(_textureManager->GetTexture((TextureID)initValues.textureID));
    if (initValues.specularTextureID != -1) SetSpecularTexture(_textureManager->GetTexture((TextureID)initValues.specularTextureID));
}

void GameObject::SetMesh(MeshData mesh)
{
	_mesh = mesh;
}

void GameObject::SetPosition(XMFLOAT3 position)
{
    _transform.position = position;
    UpdateObjectMatrix();
}

void GameObject::SetRotation(XMFLOAT3 rotation)
{
    _transform.rotation = rotation;
    UpdateObjectMatrix();
}

void GameObject::SetScale(XMFLOAT3 scale)
{
    _transform.scale = scale; 
    UpdateObjectMatrix();
}

void GameObject::SetTexture(ID3D11ShaderResourceView* texture)
{
    _texture = texture;
    _textureState = TEXTURE_STATE_DIFFUSE;
    
}

void GameObject::SetSpecularTexture(ID3D11ShaderResourceView* texture)
{
    _specularTexture = texture;
    _textureState = TEXTURE_STATE_DIFFUSE_SPECULAR;
}

void GameObject::SetTransform(Transform transform)
{
    _transform = transform;
    UpdateObjectMatrix();
}

void GameObject::SetParent(GameObject* object)
{
    _parent = object;
    _parent->AddChild(this);
}

void GameObject::AddChild(GameObject* object)
{
    _children.push_back(object);
    if (!object->GetParent()) object->SetParent(this);
}

void GameObject::SetName(std::string name)
{
    _name = name;
}

void GameObject::SetHitbox(Hitbox hitbox)
{
    _hitbox = hitbox;
}

void GameObject::SetCollisionEnabled(bool state)
{
    _collisionEnabled = state;
}

XMFLOAT3 GameObject::GetPosition()
{
    return _transform.position;
}

XMFLOAT3 GameObject::GetRotation()
{
    return _transform.rotation;
}

XMFLOAT3 GameObject::GetScale()
{
    return _transform.scale;
}

Transform GameObject::GetTransform()
{
    return _transform;
}

GameObject* GameObject::GetParent()
{
    return _parent;
}

GameObject* GameObject::GetChild(int child)
{
    if (child < _children.size()) 
    {
        return _children[child];
    }
    return nullptr;
}

std::string GameObject::GetName()
{
    return _name;
}

Hitbox GameObject::GetHitbox()
{
    // Calculates hitbox relative to the objects position //
    Hitbox hitbox =
    {
        {
            _hitbox.minPosition.x + GetPosition().x,
            _hitbox.minPosition.y + GetPosition().y,
            _hitbox.minPosition.z + GetPosition().z
        },
        {
            _hitbox.maxPosition.x + GetPosition().x,
            _hitbox.maxPosition.y + GetPosition().y,
            _hitbox.maxPosition.z + GetPosition().z
        }
    };

    return hitbox;
}

bool GameObject::GetCollisionEnabled()
{
    return _collisionEnabled;
}

bool GameObject::IsColliding(Hitbox other)
{
    if (other.minPosition.x >= GetHitbox().minPosition.x && other.maxPosition.x <= GetHitbox().maxPosition.x)
    {
        if (other.minPosition.y >= GetHitbox().minPosition.y && other.maxPosition.y <= GetHitbox().maxPosition.y)
        {
            if (other.minPosition.z >= GetHitbox().minPosition.z && other.maxPosition.z <= GetHitbox().maxPosition.z)
            {
                return true;
            }
            else return false;
        }
        else return false;
    }
    else return false;
}

void GameObject::Update(float deltaTime)
{
    for (GameObject* child : _children) 
    {
        child->Update(deltaTime);
    }
}

void GameObject::Draw(FrameData frameData)
{
    
    frameData.cbData->TextureState = _textureState;

    // Binds the texture if it has one //
    switch (_textureState) 
    {
    case TEXTURE_STATE_DIFFUSE:
        _immediateContext->PSSetShaderResources(0, 1, &_texture);
        break;
    case TEXTURE_STATE_DIFFUSE_SPECULAR:
        _immediateContext->PSSetShaderResources(0, 1, &_texture);
        _immediateContext->PSSetShaderResources(1, 1, &_specularTexture);
        break;
    }

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

    for (GameObject* child : _children)
    {
        child->Draw(frameData);
    }
}

void GameObject::UpdateObjectMatrix()
{
    XMMATRIX TransformMatrix = XMMatrixIdentity();

    // Edits the objects matrix with the new scale //
 
    TransformMatrix *= XMMatrixScalingFromVector({ _transform.scale.x,_transform.scale.y,_transform.scale.z });

    // Edits the objects matrix with the new rotation //
    TransformMatrix *= XMMatrixRotationX(_transform.rotation.x);
    TransformMatrix *= XMMatrixRotationY(_transform.rotation.y);
    TransformMatrix *= XMMatrixRotationZ(_transform.rotation.z);
   
    // Edits the objects matrix with the new translation //
    TransformMatrix *= XMMatrixTranslation(_transform.position.x, _transform.position.y, _transform.position.z);

    if (_parent) 
    {
        // Edits the matrix in the reverse order using the parents transform //
        
        // Object translated by parents position //
        TransformMatrix *= XMMatrixTranslation(_parent->GetPosition().x, _parent->GetPosition().y, _parent->GetPosition().z);

        // Object rotated by parents rotation //
        TransformMatrix *= XMMatrixRotationX(_parent->GetRotation().x);
        TransformMatrix *= XMMatrixRotationY(_parent->GetRotation().y);
        TransformMatrix *= XMMatrixRotationZ(_parent->GetRotation().z);

        // Object scaled by parents scale //
        TransformMatrix *= XMMatrixScalingFromVector({ _parent->GetScale().x,_parent->GetScale().y,_parent->GetScale().z });
    }

    // New transform is set //
    XMStoreFloat4x4(&_objectTransformMatrix, TransformMatrix);

    // Children are updated as changes have been made to parent //
    for (GameObject* child : _children) 
    {
        child->UpdateObjectMatrix();
    }
}
