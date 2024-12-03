#pragma once

#include "GameDefinitions.h"
#include "GameObject.h"
#include "Skybox.h"
#include "GameScene.h"
#include "Camera.h"
#include "OBJLoader.h"
#include "TextureManager.h"
#include "MeshManager.h"
#include "InputManager.h"
#include "JSONmanager.h"
#include "WorldGenerator.h"
#include "DirectionalLight.h"
#include "WorldEffectsObject.h"
#include "MovingObject.h"
#include "FollowCamera.h"
#include "CameraManager.h"
#include "PointLight.h"
#include "FirstPersonCamera.h"
//#include <wrl.h>
//using Microsoft::WRL::ComPtr;

class DX11Framework
{
	int _WindowWidth = 1280;
	int _WindowHeight = 768;

	float _cameraFOV = 45.f;
	ID3D11DeviceContext* _immediateContext = nullptr;
	ID3D11Device* _device;
	IDXGIDevice* _dxgiDevice = nullptr;
	IDXGIFactory2* _dxgiFactory = nullptr;
	ID3D11RenderTargetView* _frameBufferView = nullptr;
	IDXGISwapChain1* _swapChain;
	D3D11_VIEWPORT _viewport;
	ID3D11SamplerState* _bilinearSamplerState;

	bool _isFillState = true;
	ID3D11RasterizerState* _fillState;
	ID3D11RasterizerState* _wireframeState;
	ID3D11RasterizerState* _skyboxState;
	ID3D11VertexShader* _vertexShader;
	ID3D11VertexShader* _vertexShaderSkybox;
	ID3D11InputLayout* _inputLayout;
	ID3D11InputLayout* _inputLayoutSkybox;
	ID3D11ShaderResourceView* _skyboxTexture;
	
	ID3D11PixelShader* _pixelShader;
	ID3D11PixelShader* _pixelShaderSkybox;
	ID3D11Buffer* _constantBuffer;

	ID3D11Buffer* _cubeVertexBuffer;
	ID3D11Buffer* _cubeIndexBuffer;

	ID3D11Buffer* _pyramidVertexBuffer;
	ID3D11Buffer* _pyramidIndexBuffer;

	GameScene* _scene;
	GameObject* _player;
	DirectionalLight* _directionalLight;
	CameraManager* _cameraManager;
	TextureManager* _textureManager;
	MeshManager* _meshManager;
	InputManager* _inputManager;
	JSONmanager* _JSONmanager;
	WorldGenerator* _worldGenerator;

	HWND _windowHandle;

	XMFLOAT4X4 _World;
	XMFLOAT4X4 _World2;
	XMFLOAT4X4 _World3;
	XMFLOAT4X4 _View;
	XMFLOAT4X4 _Projection;

	ConstantBuffer _cbData;

	ID3D11Texture2D* _depthStencilBuffer;
	ID3D11DepthStencilView* _depthStencilView;
	ID3D11DepthStencilState* _depthStencilSkybox;


public:
	HRESULT Initialise(HINSTANCE hInstance, int nCmdShow);
	HRESULT CreateWindowHandle(HINSTANCE hInstance, int nCmdShow);
	HRESULT CreateD3DDevice();
	HRESULT CreateSwapChainAndFrameBuffer();
	HRESULT InitShadersAndInputLayout();
	HRESULT InitVertexIndexBuffers();
	HRESULT InitPipelineVariables();
	HRESULT InitRunTimeData();
	~DX11Framework();

	void InitScene();
	void CreateWorld(XMINT2 mapSize, int seed);

	void Update();
	void Draw();
};