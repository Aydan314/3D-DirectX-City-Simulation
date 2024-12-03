#pragma once
#include "GameDefinitions.h"
#include "TextureManager.h"
#include "MeshManager.h"

struct ResourceManagers
{
	ID3D11DeviceContext* immediateContext;
	TextureManager* textureManager;
	MeshManager* meshManager;
};

class GameObject
{
protected:
	ID3D11DeviceContext* _immediateContext = nullptr;
	TextureManager* _textureManager;
	MeshManager* _meshManager;

	XMFLOAT4X4 _objectTransformMatrix;
	MeshData _mesh;
	ID3D11ShaderResourceView* _texture = nullptr;
	ID3D11ShaderResourceView* _specularTexture = nullptr;
	TextureState _textureState = TEXTURE_STATE_NONE;

	Transform _transform = Transform();

	GameObject* _parent = nullptr;
	std::vector<GameObject*> _children;
	std::string _name = "OBJECT";

	bool _collisionEnabled = true;
	Hitbox _hitbox;

public:
	GameObject(Transform initTransform, ResourceManagers resourceManagers);
	GameObject(GameObjectValues initValues, ResourceManagers resourceManagers);

	// Set methods //
	void SetMesh(MeshData mesh);
	virtual void SetPosition(XMFLOAT3 position);
	virtual void SetRotation(XMFLOAT3 rotation);
	void SetScale(XMFLOAT3 scale);
	void SetTexture(ID3D11ShaderResourceView* texture);
	void SetSpecularTexture(ID3D11ShaderResourceView* texture);
	virtual void SetTransform(Transform transform);
	void SetParent(GameObject* object);
	void AddChild(GameObject* object);
	void SetName(std::string name);
	void SetHitbox(Hitbox hitbox);
	void SetCollisionEnabled(bool state);

	// Get methods //
	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotation();
	XMFLOAT3 GetScale();
	Transform GetTransform();
	GameObject* GetParent();
	GameObject* GetChild(int child);
	std::string GetName();
	Hitbox GetHitbox();
	bool GetCollisionEnabled();
	bool IsColliding(Hitbox other);


	virtual void Update(float deltaTime);
	virtual void Draw(FrameData frameData);

	void UpdateObjectMatrix();
};

