#pragma once
#include <iostream>
#include <windows.h>
#include <d3d11_4.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <vector>
#include "DDSTextureLoader.h"
#include "JSON/json.hpp"
#include "fstream"

#define DESIRED_FRAME_RATE 120
#define MAX_LIGHTS 16

using namespace DirectX;
using json = nlohmann::json;

enum TextureState 
{
	TEXTURE_STATE_NONE,
	TEXTURE_STATE_DIFFUSE,
	TEXTURE_STATE_DIFFUSE_SPECULAR
};

enum Direction
{
	DIRECTION_FORWARD,
	DIRECTION_BACKWARD,
	DIRECTION_LEFT,
	DIRECTION_RIGHT,
	DIRECTION_UP,
	DIRECTION_DOWN
};

enum TextureID 
{
	TEXTURE_CRATE,
	TEXTURE_ASPHALT,
	TEXTURE_PLANE,
	TEXTURE_CAR,
	TEXTURE_ROAD,
	TEXTURE_SKYBOX,
	TEXTURE_GRASS,
	TEXTURE_BRICK_BUILDING,
	TEXTURE_APARTMENT_BUILDING,
	TEXTURE_SPECULAR_CRATE,
	TEXTURE_LAMPPOST
};

const std::vector<const wchar_t*> GAME_TEXTURES =
{
	L"Textures/Crate_COLOR.dds",
	L"Textures/Asphalt_COLOR.dds",
	L"Textures/Hercules_COLOR.dds",
	L"Textures/Car_COLOR.dds",
	L"Textures/Road_COLOR.dds",
	L"Textures/skybox.dds",
	L"Textures/Grass_COLOR.dds",
	L"Textures/Brick_Building_COLOR.dds",
	L"Textures/Apartment_Building_COLOR.dds",
	L"Textures/Crate_SPEC.dds",
	L"Textures/Lamppost_COLOR.dds"
};

enum MeshID 
{
	MESH_CAR,
	MESH_PLANE,
	MESH_CUBE,
	MESH_CYLINDER,
	MESH_DONUT,
	MESH_SPHERE,
	MESH_ROAD_STRAIGHT,
	MESH_ROAD_CROSSROAD,
	MESH_ROAD_TJUNCTION,
	MESH_ROAD_CORNER,
	MESH_GRASS_TILE,
	MESH_BRICK_BUILDING,
	MESH_APARTMENT_BUILDING,
	MESH_LAMPPOST
};

enum GameObjectID
{
	OBJECT_HERCULES,
	OBJECT_CAR,
	OBJECT_CRATE,
	OBJECT_CRATE2,
	OBJECT_ROAD_STRAIGHT,
	OBJECT_SKYBOX,
	OBJECT_ROAD_CROSSROAD,
	OBJECT_ROAD_TJUNCTION,
	OBJECT_ROAD_CORNER,
	OBJECT_GRASS_TILE,
	OBJECT_BRICK_BUILDING,
	OBJECT_APARTMENT_BUILDING,
	OBJECT_LAMPPOST
};

const std::vector<std::string> GAME_MESHS =
{
	"3D Models/Car/Car.obj",
	"3D Models/Airplane/Hercules.obj",
	"3D Models/Made In Blender/cube.obj",
	"3D Models/Made In Blender/cylinder.obj",
	"3D Models/Made In Blender/donut.obj",
	"3D Models/Made In Blender/sphere.obj",
	"3D Models/Roads/Road_Straight.obj",
	"3D Models/Roads/Road_Crossroad.obj",
	"3D Models/Roads/Road_Tjunction.obj",
	"3D Models/Roads/Road_Corner.obj",
	"3D Models/Roads/Grass_Tile.obj",
	"3D Models/Buildings/Brick_Building.obj",
	"3D Models/Buildings/Apartment_Building.obj",
	"3D Models/Buildings/Lamppost.obj"
};

struct SimpleVertex
{
	XMFLOAT3 Position;
	XMFLOAT3 Normal;
	XMFLOAT2 TexCoord;

	bool operator<(const SimpleVertex other) const
	{
		return memcmp((void*)this, (void*)&other, sizeof(SimpleVertex)) > 0;
	};
};

struct LightValues 
{
	XMFLOAT4 diffuseLight;
	XMFLOAT4 diffuseMaterial;
	XMFLOAT4 ambientLight;
	XMFLOAT4 ambientMaterial;
	XMFLOAT4 specularLight;
	XMFLOAT4 specularMaterial;
	float specPower;
	XMFLOAT3 lightDir;
	float radius;
	XMFLOAT3 lightPos;
};


struct ConstantBuffer
{
	XMMATRIX Projection;
	XMMATRIX View;
	XMMATRIX World;
	LightValues PrimaryLightData;
	LightValues LightData[16];
	UINT TextureState;
	XMFLOAT3 CameraPosition;
	
};

struct Vector3f
{
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;

	Vector3f() 
	{
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	}

	Vector3f(float X, float Y, float Z)
	{
		x = X;
		y = Y;
		z = Z;
	}
};

struct Transform
{
	XMFLOAT3 position = { 0,0,0 };
	XMFLOAT3 rotation = { 0,0,0 };
	XMFLOAT3 scale = { 1,1,1 };
};

struct Hitbox 
{
	XMFLOAT3 minPosition;
	XMFLOAT3 maxPosition;
};

struct GameObjectValues
{
	std::string name;
	int meshID;
	int textureID;
	int specularTextureID;
	bool collisionEnabled;

	Hitbox hitbox;
	Transform transform;
};

struct MeshData
{
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	UINT VBStride;
	UINT VBOffset;
	UINT indexCount;
};

struct FrameData 
{
	ConstantBuffer* cbData;
	D3D11_MAPPED_SUBRESOURCE* mappedSubresource;
	XMFLOAT4X4 view;
	XMFLOAT4X4 projection;
	ID3D11Buffer* constantBuffer;
};

const std::vector<GameObjectID> GAME_BUILDINGS=
{
	OBJECT_GRASS_TILE,
	OBJECT_BRICK_BUILDING,
	OBJECT_APARTMENT_BUILDING
};