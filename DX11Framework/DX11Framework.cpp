#include "DX11Framework.h"
#include <string>

//#define RETURNFAIL(x) if(FAILED(x)) return x;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch (message)
    {
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

HRESULT DX11Framework::Initialise(HINSTANCE hInstance, int nShowCmd)
{
    HRESULT hr = S_OK;

    hr = CreateWindowHandle(hInstance, nShowCmd);
    if (FAILED(hr)) return E_FAIL;

    hr = CreateD3DDevice();
    if (FAILED(hr)) return E_FAIL;

    hr = CreateSwapChainAndFrameBuffer();
    if (FAILED(hr)) return E_FAIL;

    hr = InitShadersAndInputLayout();
    if (FAILED(hr)) return E_FAIL;

    hr = InitVertexIndexBuffers();
    if (FAILED(hr)) return E_FAIL;

    hr = InitPipelineVariables();
    if (FAILED(hr)) return E_FAIL;

    hr = InitRunTimeData();
    if (FAILED(hr)) return E_FAIL;

    InitScene();

    return hr;
}

HRESULT DX11Framework::CreateWindowHandle(HINSTANCE hInstance, int nCmdShow)
{
    const wchar_t* windowName  = L"DX11Framework";

    WNDCLASSW wndClass;
    wndClass.style = 0;
    wndClass.lpfnWndProc = WndProc;
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.hInstance = 0;
    wndClass.hIcon = 0;
    wndClass.hCursor = 0;
    wndClass.hbrBackground = 0;
    wndClass.lpszMenuName = 0;
    wndClass.lpszClassName = windowName;

    RegisterClassW(&wndClass);

    _windowHandle = CreateWindowExW(0, windowName, windowName, WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 
        _WindowWidth, _WindowHeight, nullptr, nullptr, hInstance, nullptr);

    return S_OK;
}

HRESULT DX11Framework::CreateD3DDevice()
{
    HRESULT hr = S_OK;

    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0,
    };

    ID3D11Device* baseDevice;
    ID3D11DeviceContext* baseDeviceContext;

    DWORD createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
    hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_BGRA_SUPPORT | createDeviceFlags, featureLevels, ARRAYSIZE(featureLevels), D3D11_SDK_VERSION, &baseDevice, nullptr, &baseDeviceContext);
    if (FAILED(hr)) return hr;

    ///////////////////////////////////////////////////////////////////////////////////////////////

    hr = baseDevice->QueryInterface(__uuidof(ID3D11Device), reinterpret_cast<void**>(&_device));
    hr = baseDeviceContext->QueryInterface(__uuidof(ID3D11DeviceContext), reinterpret_cast<void**>(&_immediateContext));

    baseDevice->Release();
    baseDeviceContext->Release();

    ///////////////////////////////////////////////////////////////////////////////////////////////

    hr = _device->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&_dxgiDevice));
    if (FAILED(hr)) return hr;

    IDXGIAdapter* dxgiAdapter;
    hr = _dxgiDevice->GetAdapter(&dxgiAdapter);
    hr = dxgiAdapter->GetParent(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(&_dxgiFactory));
    dxgiAdapter->Release();

    return S_OK;
}

HRESULT DX11Framework::CreateSwapChainAndFrameBuffer()
{
    HRESULT hr = S_OK;

    DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
    swapChainDesc.Width = 0; // Defer to WindowWidth
    swapChainDesc.Height = 0; // Defer to WindowHeight
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //FLIP* modes don't support sRGB backbuffer
    swapChainDesc.Stereo = FALSE;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = 2;
    swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
    swapChainDesc.Flags = 0;

    hr = _dxgiFactory->CreateSwapChainForHwnd(_device, _windowHandle, &swapChainDesc, nullptr, nullptr, &_swapChain);
    if (FAILED(hr)) return hr;

    ///////////////////////////////////////////////////////////////////////////////////////////////

    ID3D11Texture2D* frameBuffer = nullptr;

    hr = _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&frameBuffer));
    if (FAILED(hr)) return hr;

    D3D11_RENDER_TARGET_VIEW_DESC framebufferDesc = {};
    framebufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; //sRGB render target enables hardware gamma correction
    framebufferDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

    hr = _device->CreateRenderTargetView(frameBuffer, &framebufferDesc, &_frameBufferView);

    D3D11_TEXTURE2D_DESC depthBufferDesc = {};
    frameBuffer->GetDesc(&depthBufferDesc);

    depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    _device->CreateTexture2D(&depthBufferDesc, nullptr, &_depthStencilBuffer);
    _device->CreateDepthStencilView(_depthStencilBuffer, nullptr, &_depthStencilView);

    frameBuffer->Release();

    D3D11_DEPTH_STENCIL_DESC dsDescSkybox = {};
    dsDescSkybox.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
    dsDescSkybox.DepthEnable = true;
    dsDescSkybox.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;

    _device->CreateDepthStencilState(&dsDescSkybox, &_depthStencilSkybox);


    return hr;
}

HRESULT DX11Framework::InitShadersAndInputLayout()
{
    HRESULT hr = S_OK;
    ID3DBlob* errorBlob;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif
    
    ID3DBlob* vsBlob;

    ////////////////////////////// Create Simple shader //////////////////////////////
    
    // Vertex shader //
    hr =  D3DCompileFromFile(L"SimpleShaders.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VS_main", "vs_5_0", dwShaderFlags, 0, &vsBlob, &errorBlob);
    if (FAILED(hr))
    {
        MessageBoxA(_windowHandle, (char*)errorBlob->GetBufferPointer(), nullptr, ERROR);
        errorBlob->Release();
        return hr;
    }

    hr = _device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &_vertexShader);
    if (FAILED(hr)) return hr;

    D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
    {

        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA,   0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0 },
        { "TEXCOORD",0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0 }
    };

    hr = _device->CreateInputLayout(inputElementDesc, ARRAYSIZE(inputElementDesc), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &_inputLayout);
    if (FAILED(hr)) return hr;

    // Pixel shader //

    ID3DBlob* psBlob;

    hr = D3DCompileFromFile(L"SimpleShaders.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PS_main", "ps_5_0", dwShaderFlags, 0, &psBlob, &errorBlob);
    if (FAILED(hr))
    {
        MessageBoxA(_windowHandle, (char*)errorBlob->GetBufferPointer(), nullptr, ERROR);
        errorBlob->Release();
        return hr;
    }

    hr = _device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &_pixelShader);
    if (FAILED(hr)) return hr;

    vsBlob->Release();
    psBlob->Release();

    ///////////////// Create skybox shader //////////////////////////////

    // Vertex Shader //
    ID3DBlob* vsBlobSkybox;

    hr = D3DCompileFromFile(L"SkyboxShader.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VS_main", "vs_5_0", dwShaderFlags, 0, &vsBlobSkybox, &errorBlob);
    if (FAILED(hr)) 
    {
        MessageBoxA(_windowHandle, (char*)errorBlob->GetBufferPointer(), nullptr, ERROR);
        errorBlob->Release();
        return hr;
    }

    hr = _device->CreateVertexShader(vsBlobSkybox->GetBufferPointer(), vsBlobSkybox->GetBufferSize(), nullptr, &_vertexShaderSkybox);
    if (FAILED(hr)) return hr;
    

    D3D11_INPUT_ELEMENT_DESC inputElementDescSkybox[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0 }
    };

    hr = _device->CreateInputLayout(inputElementDescSkybox, ARRAYSIZE(inputElementDescSkybox), vsBlobSkybox->GetBufferPointer(), vsBlobSkybox->GetBufferSize(), &_inputLayoutSkybox);
    if (FAILED(hr)) return hr;

    // Pixel shader //
    ID3DBlob* psBlobSkybox;

    hr = D3DCompileFromFile(L"SkyboxShader.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PS_main", "ps_5_0", dwShaderFlags, 0, &psBlobSkybox, &errorBlob);
    if (FAILED(hr))
    {
        MessageBoxA(_windowHandle, (char*)errorBlob->GetBufferPointer(), nullptr, ERROR);
        errorBlob->Release();
        return hr;
    }

    hr = _device->CreatePixelShader(psBlobSkybox->GetBufferPointer(), psBlobSkybox->GetBufferSize(), nullptr, &_pixelShaderSkybox);
    if (FAILED(hr)) return hr;

    vsBlobSkybox->Release();
    psBlobSkybox->Release();

    return hr;
}

HRESULT DX11Framework::InitVertexIndexBuffers()
{
    HRESULT hr = S_OK;


    return S_OK;
}

HRESULT DX11Framework::InitPipelineVariables()
{
    HRESULT hr = S_OK;

    // Define game managers //
    _textureManager = new TextureManager(_device);
    _meshManager = new MeshManager(_device);
    _inputManager = new InputManager(&_windowHandle);
    _JSONmanager = new JSONmanager();
    _worldGenerator = new WorldGenerator(_JSONmanager);

    //Input Assembler
    _immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    _immediateContext->IASetInputLayout(_inputLayout);

    // fill rasterizer //
    D3D11_RASTERIZER_DESC fillDesc = {};
    fillDesc.FillMode = D3D11_FILL_SOLID;
    fillDesc.CullMode = D3D11_CULL_BACK;

    hr = _device->CreateRasterizerState(&fillDesc, &_fillState);
    if (FAILED(hr)) return hr;

    _immediateContext->RSSetState(_fillState);

    // wireFrame rasterizer //
    D3D11_RASTERIZER_DESC wireframeDesc = {};
    wireframeDesc.FillMode = D3D11_FILL_WIREFRAME;
    wireframeDesc.CullMode = D3D11_CULL_NONE;

    hr = _device->CreateRasterizerState(&wireframeDesc, &_wireframeState);
    if (FAILED(hr)) return hr;

    // Skybox rasterizer //
    D3D11_RASTERIZER_DESC skyboxDesc = {};
    skyboxDesc.FillMode = D3D11_FILL_SOLID;
    skyboxDesc.CullMode = D3D11_CULL_NONE;

    hr = _device->CreateRasterizerState(&skyboxDesc, &_skyboxState);
    if (FAILED(hr)) return hr;




    //Viewport Values
    _viewport = { 0.0f, 0.0f, (float)_WindowWidth, (float)_WindowHeight, 0.0f, 1.0f };
    _immediateContext->RSSetViewports(1, &_viewport);

    //Constant Buffer
    D3D11_BUFFER_DESC constantBufferDesc = {};
    constantBufferDesc.ByteWidth = sizeof(ConstantBuffer);
    constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    hr = _device->CreateBuffer(&constantBufferDesc, nullptr, &_constantBuffer);
    if (FAILED(hr)) { return hr; }

    // Texture Sampler //
    D3D11_SAMPLER_DESC bilinearSamplerdesc = {};

    bilinearSamplerdesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    bilinearSamplerdesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    bilinearSamplerdesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    bilinearSamplerdesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    bilinearSamplerdesc.MaxLOD = 1;
    bilinearSamplerdesc.MinLOD = 0;

    hr = _device->CreateSamplerState(&bilinearSamplerdesc, &_bilinearSamplerState);
    if (FAILED(hr)) return hr;

    _immediateContext->PSSetSamplers(0, 1, &_bilinearSamplerState);

    _immediateContext->VSSetConstantBuffers(0, 1, &_constantBuffer);
    _immediateContext->PSSetConstantBuffers(0, 1, &_constantBuffer);

    return S_OK;
}

HRESULT DX11Framework::InitRunTimeData()
{
    HRESULT hr = S_OK;

    // Create primary directional light //
    
    LightValues lightValues =
    {
        XMFLOAT4{ 0.8f,0.8f,0.6f,1.0f },
        XMFLOAT4{ 1.0f,1.0f,1.0f,1.0f },

        XMFLOAT4{ 0.1f,0.2f,0.2f,1.0f },
        XMFLOAT4{ 1.0f,1.0f,1.0f,1.0f },

        XMFLOAT4{ 0.4f,0.4f,0.4f,1.0f },
        XMFLOAT4{ 1.0f,1.0f,1.0f,1.0f },
        100.0f,

        XMFLOAT3{ 0.0f,0.0f,1.0f }
    };

    _directionalLight = new DirectionalLight(lightValues);

    return S_OK;
}

DX11Framework::~DX11Framework()
{
    if(_immediateContext)_immediateContext->Release();
    if(_device)_device->Release();
    if(_dxgiDevice)_dxgiDevice->Release();
    if(_dxgiFactory)_dxgiFactory->Release();
    if(_frameBufferView)_frameBufferView->Release();
    if(_swapChain)_swapChain->Release();

    if (_wireframeState)_wireframeState->Release();
    if(_fillState)_fillState->Release();
    if(_vertexShader)_vertexShader->Release();
    if(_inputLayout)_inputLayout->Release();
    if(_pixelShader)_pixelShader->Release();
    if(_constantBuffer)_constantBuffer->Release();
    if(_cubeVertexBuffer)_cubeVertexBuffer->Release();
    if(_cubeIndexBuffer)_cubeIndexBuffer->Release();
    if(_depthStencilBuffer)_depthStencilBuffer->Release();
    if(_depthStencilView)_depthStencilView->Release();

}

void DX11Framework::InitScene()
{
    float aspect = _viewport.Width / _viewport.Height;

    _scene = new GameScene();

    ResourceManagers resourceManagers =
    {
        _immediateContext,
        _textureManager,
        _meshManager
    };

    // Create object to handle day night cycle //
    WorldEffectsObject* worldEffects = new WorldEffectsObject(Transform(), resourceManagers);
    worldEffects->SetLightAsSun(_directionalLight);
    _scene->AddGameObject((GameObject*)worldEffects);

    // Create plane to fly over city //
    MovingObject* plane = new MovingObject(*_JSONmanager->GetGameObject(OBJECT_HERCULES), resourceManagers);
    ((GameObject*)plane)->SetPosition({ -200,100,-200 });
    plane->SetWaypoints({ {1000,100,1000},{-200,100,-200} });
    plane->SetModelFlip(true);
    _scene->AddGameObject((GameObject*)plane);

    // Create Player //
    _player = new GameObject(*_JSONmanager->GetGameObject(OBJECT_CRATE), resourceManagers);
    _player->SetName("Player");
    _player->SetPosition({ 0, 2, 0 });

    CreateWorld({ 40,40 }, time(NULL));
   
    //////////// Create Cameras ///////////////////
    _cameraManager = new CameraManager();

    // Create Free Camera //
    Camera* freeCam = new Camera(aspect);
    freeCam->SetPosition({ 0,2,0 });
    _cameraManager->AddCamera(freeCam);
    _scene->AddGameObject(freeCam);

    // Create a Camera Following a Car //
    FollowCamera* followCamCar = new FollowCamera(aspect);
    followCamCar->SetTarget(_scene->GetFirstInstanceOfName("Car"));
    _cameraManager->AddCamera(followCamCar);

    // Create a Camera Following the Plane //
    FollowCamera* followCamPlane = new FollowCamera(aspect);
    followCamPlane->SetTarget(_scene->GetFirstInstanceOfName("Plane"));
    followCamPlane->SetBaseSpeed(10.f);
    followCamPlane->SetSprintSpeed(20.f);
    _cameraManager->AddCamera(followCamPlane);

    // Create a First Person Camera //
    FirstPersonCamera* fpCamera = new FirstPersonCamera(aspect);
    fpCamera->AttachPlayer(_player);
    fpCamera->SetPosition({ 0,2,0 });
    fpCamera->SetBaseSpeed(20.f);
    fpCamera->SetSprintSpeed(40.f);
    _cameraManager->AddCamera(fpCamera);
    _scene->AddGameObject(fpCamera);

    // Create Skybox //

    SkyboxResources skyboxResources =
    {
        _depthStencilSkybox,
        _inputLayoutSkybox,
        _vertexShaderSkybox,
        _pixelShaderSkybox,
        _skyboxState
    };

    Skybox* skybox = new Skybox(*_JSONmanager->GetGameObject(OBJECT_SKYBOX), resourceManagers, skyboxResources);
    ((GameObject*)skybox)->SetPosition({ 0,10,0 });
    _scene->AddGameObject((GameObject*)skybox);
   
}

void DX11Framework::CreateWorld(XMINT2 mapSize, int seed)
{
    // Create roads and buildings //

    ResourceManagers resourceManagers =
    {
        _immediateContext,
        _textureManager,
        _meshManager
    };

    std::vector<std::vector<GameObjectValues>> map;

    map = _worldGenerator->GenerateWorld(mapSize, seed);

    GameObject* mapStart = new GameObject(map[0][0], resourceManagers);
    _scene->AddGameObject(mapStart);

    for (int x = 0; x < mapSize.x; x++)
    {
        for (int y = 0; y < mapSize.y; y++) 
        {
            GameObject* newObject = new GameObject(map[x][y], resourceManagers);
            _scene->AddGameObject(newObject);
        }
    }

    // Create cars on roads //

    for (auto car : _worldGenerator->GenerateCars()) 
    {
        MovingObject* newObject = new MovingObject(car, resourceManagers);
        newObject->SetWaypoints(_worldGenerator->GenerateCarRoute(((GameObject*)newObject)->GetTransform()));
        newObject->SetAcceptanceDistance(5);
        newObject->SetSpeed(0.5f + (rand() % 20) / 40.f);
        _scene->AddGameObject((GameObject*)newObject);
    }

    // Create lamp posts on streets //

    for (LightValues light : _worldGenerator->GenerateLampPosts())
    {
        PointLight* newLight = new PointLight(light);
        _scene->AddNewLight(newLight);

        GameObject* newObject = new GameObject(*_JSONmanager->GetGameObject(OBJECT_LAMPPOST), resourceManagers);
        newObject->SetPosition({ newLight->GetPosition().x,0.6f,newLight->GetPosition().z });
        _scene->AddGameObject(newObject);
    }
}

void DX11Framework::Update()
{
    // Record frame time //
    //Static initializes this value only once    
    static ULONGLONG frameStart = GetTickCount64();

    ULONGLONG frameNow = GetTickCount64();
    float deltaTime = (frameNow - frameStart) / 1000.0f;
    frameStart = frameNow;
    
    // Update GameObjects in the scene //
    _inputManager->Update(deltaTime);
    _scene->Update(deltaTime);
    _cameraManager->GetActiveCamera()->Update(deltaTime);

    bool collision = false;

    // Calculate Collisions //
    for (GameObject* object : _scene->GetAllObjects()) 
    {
        if (object->GetCollisionEnabled()) 
        {
            if (object->GetName() != "Player")
            {
                if (object->IsColliding(_player->GetHitbox()))
                {
                    collision = true;
                    break;
                }
            }
        }
    }

    _cameraManager->GetActiveCamera()->SetInCollision(collision);

    /////////////// Handle user input //////////////////////

    // Toggle wireframe view //
    if (GetAsyncKeyState(VK_F1) & 0x0001) 
    {
        _isFillState = !_isFillState;
    }

    // Zoom in and out //
    if (GetAsyncKeyState(VK_DOWN) & 0x0001) 
    {
        _cameraFOV++;
        _cameraManager->GetActiveCamera()->SetFOV(_cameraFOV);
    }

    else if (GetAsyncKeyState(VK_UP) & 0x0001)
    {
        _cameraFOV--;
        _cameraManager->GetActiveCamera()->SetFOV(_cameraFOV);
    }

    // Cycle through available cameras //
    if (GetAsyncKeyState(VK_LEFT) & 0x0001)
    {
        _cameraManager->PrevCamera();
    }

    else if (GetAsyncKeyState(VK_RIGHT) & 0x0001)
    {
        _cameraManager->NextCamera();
    }

    // Move camera in direction of input //
    if (GetAsyncKeyState('A') & 0x8000)
    {
        _cameraManager->GetActiveCamera()->MoveCamera(DIRECTION_LEFT,deltaTime);
    }

    else if (GetAsyncKeyState('D') & 0x8000)
    {
        _cameraManager->GetActiveCamera()->MoveCamera(DIRECTION_RIGHT, deltaTime);
    }

    if (GetAsyncKeyState('W') & 0x8000)
    {
        _cameraManager->GetActiveCamera()->MoveCamera(DIRECTION_FORWARD, deltaTime);
    }

    else if (GetAsyncKeyState('S') & 0x8000)
    {
        _cameraManager->GetActiveCamera()->MoveCamera(DIRECTION_BACKWARD, deltaTime);
    }

    if (GetAsyncKeyState(VK_SPACE) & 0x8000)
    {
        _cameraManager->GetActiveCamera()->MoveCamera(DIRECTION_UP, deltaTime);
    }

    else if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
    {
        _cameraManager->GetActiveCamera()->MoveCamera(DIRECTION_DOWN, deltaTime);
    }

    // Camera sprint //
    if (GetAsyncKeyState(VK_LCONTROL) & 0x8000)
    {
        _cameraManager->GetActiveCamera()->SetSpeed(_cameraManager->GetActiveCamera()->GetSprintSpeed());
    }
    else _cameraManager->GetActiveCamera()->SetSpeed(_cameraManager->GetActiveCamera()->GetBaseSpeed());

    // Handle camera look input //
    if (_inputManager->GetMouseMovement().y != 0 || _inputManager->GetMouseMovement().x != 0)
    {
        _cameraManager->GetActiveCamera()->AdjustRotation({ _inputManager->GetMouseMovement().y ,_inputManager->GetMouseMovement().x,0 }, true);
    }

    // Cap the framerate to the desired value //
    float desiredFrameTime = 1000.f / DESIRED_FRAME_RATE;

    if (deltaTime < desiredFrameTime)  Sleep(desiredFrameTime - deltaTime);
        
}

void DX11Framework::Draw()
{    
    if (_isFillState) _immediateContext->RSSetState(_fillState);
    else _immediateContext->RSSetState(_wireframeState);

    _immediateContext->IASetInputLayout(_inputLayout);

    //Present unbinds render target, so rebind and clear at start of each frame
    float backgroundColor[4] = { 0.025f, 0.025f, 0.025f, 1.0f };  
    _immediateContext->OMSetRenderTargets(1, &_frameBufferView, _depthStencilView);
    _immediateContext->ClearRenderTargetView(_frameBufferView, backgroundColor);
    _immediateContext->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0.0f);
   
    //Store this frames data in constant buffer struct
    _cbData.World = XMMatrixTranspose(XMLoadFloat4x4(&_World));

    _cbData.View = XMMatrixTranspose(*_cameraManager->GetActiveCamera()->GetViewMatrix());
    _cbData.Projection = XMMatrixTranspose(XMLoadFloat4x4(_cameraManager->GetActiveCamera()->GetProjectionMatrix()));

    // Stores this frames light data //
    _cbData.PrimaryLightData = _directionalLight->GetLightValues();

    int index = 0;
    PointLight* light;
    while ((light = _scene->GetLightAtIndex(index)) != nullptr) 
    {
        _cbData.LightData[index] = light->GetLightValues();
        index++;
    }

    _cbData.CameraPosition = _cameraManager->GetActiveCamera()->GetPosition();

    //Write constant buffer data onto GPU
    D3D11_MAPPED_SUBRESOURCE mappedSubresource;
    _immediateContext->Map(_constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
    memcpy(mappedSubresource.pData, &_cbData, sizeof(ConstantBuffer));
    _immediateContext->Unmap(_constantBuffer, 0);
    
    // Define shaders //

    _immediateContext->VSSetShader(_vertexShader, nullptr, 0);
    _immediateContext->PSSetShader(_pixelShader, nullptr, 0);

    // Pass data to be drawn into a scene //

    FrameData frameData = FrameData{ &_cbData, &mappedSubresource, _View, _Projection, _constantBuffer };

    _scene->Draw(frameData);

    _swapChain->Present(0, 0);
}