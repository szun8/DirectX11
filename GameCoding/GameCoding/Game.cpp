#include "pch.h"
#include "Game.h"

Game::Game()
{
}

Game::~Game()
{
}

void Game::Init(HWND hwnd)
{
	_hwnd = hwnd;
	_width = GWinSizeX;
	_height = GWinSizeY;
	CreateDeviceAndSwapChain();
	CreateRenderTargetView();
	SetViewport();

	CreateGeometry();
	CreateVS();
	CreateInputLayout();	// vs단계에서 건내주는 정보 묘사
	CreatePS();
}

void Game::Update()
{
}

void Game::CreateDeviceAndSwapChain()
{
	DXGI_SWAP_CHAIN_DESC desc;
	ZeroMemory(&desc, sizeof(desc));	// 메모리를 다 0으로 초기화
	{	// 출력되어질 화면크기에 맞춰 버퍼의 사이즈도 동일하게 초기화
		desc.BufferDesc.Width = _width;
		desc.BufferDesc.Height = _height;
		desc.BufferDesc.RefreshRate.Denominator = 1;	// 화면 주사율, 분모
		desc.BufferDesc.RefreshRate.Numerator = 60;		// 분자
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;	// 멀티 샘플링과 관련된 것, 일단 사용안해서 1로
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	// 최종 결과물을 그려주는 역할로 버퍼를 사용하겠다
		desc.BufferCount = 1;	// 후면 버퍼는 하나만 사용
		desc.OutputWindow = _hwnd;
		desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	}

	// 후면 버퍼
	HRESULT hr = ::D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,	// GPU를 사용하겠다
		nullptr,
		0,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&desc,
		_swapChain.GetAddressOf(),
		_device.GetAddressOf(),
		nullptr,
		_deviceContext.GetAddressOf()
	);
	CHECK(hr);	// 성공이 아니면 크래쉬가 남 (pch.h에 매크로 정의해놓음)
}

void Game::CreateRenderTargetView()
{
	HRESULT hr;
	ComPtr<ID3D11Texture2D> backBuffer = nullptr;
	hr = _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)backBuffer.GetAddressOf());
	// 텍스쳐를 만들어서 BackBuffer 주소에 넣어주기
	CHECK(hr);

	_device->CreateRenderTargetView(backBuffer.Get(), nullptr, _renderTargetView.GetAddressOf());
	CHECK(hr);
}

void Game::SetViewport()
{
	_viewport.TopLeftX = 0.f;
	_viewport.TopLeftY = 0.f;
	_viewport.Width = static_cast<float>(_width);
	_viewport.Height = static_cast<float>(_height);
	_viewport.MinDepth = 0.f;
	_viewport.MaxDepth = 1.f;
}

void Game::CreateGeometry()
{
	{	// vertexData, 반시계 방향으로 정점 연결 in CPU
		_vertices.resize(3);

		_vertices[0].position = Vec3(-0.5f, -0.5f, 0.f);
		_vertices[0].color = Color(1.f, 0.f, 0.f, 1.f);	// red

		_vertices[1].position = Vec3(0.f, 0.5f, 0.f);
		_vertices[1].color = Color(0.f, 1.f, 0.f, 1.f);	// green

		_vertices[2].position = Vec3(0.5f,-0.5f, 0.f);
		_vertices[2].color = Color(0.f, 0.f, 1.f, 1.f);	// blue
	}

	{	// vertexBuffer, GPU쪽에 버퍼가 만들어지면서 초기설정을 완료함
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Usage = D3D11_USAGE_IMMUTABLE;	// READ ONLY IN GPU
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;	// 정점버퍼를 만드는데 사용할 거
		desc.ByteWidth = (uint32)(sizeof(Vertex) * _vertices.size());	// 버퍼의 크기가 얼마인지
		
		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(data));
		data.pSysMem = _vertices.data();	// 데이터 정보의 시작 주소를 넣어줌

		_device->CreateBuffer(&desc, &data, _vertexBuffer.GetAddressOf());
	}
}

void Game::CreateInputLayout()
{	// vertexBuffer가 넘겨준 데이터가 어떻게 되어있는지 묘사를 해줘야함
	// 전체 데이터에서 어떻게 끊어 사용할 것인지 알려달라.
	// [				] -(여기과정)-> [    ][      ][  ]...

	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA,}
	};	// 5번째 인자는 해당 인풋데이터의 offset을 알려줌(시작에서 시작해 12번 offset부터 컬러데이터다)
	const int32 count = sizeof(layout) / sizeof(D3D11_INPUT_ELEMENT_DESC);	// layout의 갯수
	_device->CreateInputLayout(layout, count, _vsBlob->GetBufferPointer(), _vsBlob->GetBufferSize(), _inputLayout.GetAddressOf());
	// 정점에 관련된 부분이기에 vs만 정보들만 채워주기
}

void Game::CreateVS()
{
	LoadShaderFromFile(L"Default.hlsl", "VS", "vs_5_0",_vsBlob);
	// 1)해당 파일의 2)메인 함수의 3)표준 쉐이더 버전 4)해당 Blob에 결과물이 채워짐
	HRESULT hr = _device->CreateVertexShader(
		_vsBlob->GetBufferPointer(),
		_vsBlob->GetBufferSize(),
		nullptr,
		_vertexShader.GetAddressOf());
	// blob에 있는 정보를 이용하여 버텍스 쉐이더를 만드는 작업을 수행
	CHECK(hr);
}

void Game::CreatePS()
{
	LoadShaderFromFile(L"Default.hlsl", "PS", "ps_5_0", _psBlob);
	// 1)해당 파일의 2)메인 함수의 3)표준 쉐이더 버전 4)해당 Blob에 결과물이 채워짐
	HRESULT hr = _device->CreatePixelShader(
		_psBlob->GetBufferPointer(),
		_psBlob->GetBufferSize(),
		nullptr,
		_pixelShader.GetAddressOf());
	// blob에 있는 정보를 이용하여 버텍스 쉐이더를 만드는 작업을 수행
	CHECK(hr);
}

void Game::LoadShaderFromFile(const wstring& path, const string& name, const string& version, ComPtr<ID3DBlob>& blob)
{
	const uint32 compileFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
	// 디버그용도이며 최적화는 일단 스킵하겠다
	
	// D3DCompiler.h에서 지원하는 함수 in pch.h
	HRESULT hr = ::D3DCompileFromFile(
		path.c_str(),
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		name.c_str(),
		version.c_str(),
		compileFlag,
		0,
		blob.GetAddressOf(),
		nullptr
	);

	CHECK(hr);
}

void Game::Render()
{
	RenderBegin();
	{
		uint32 stride = sizeof(Vertex);
		uint32 offset = 0;
		// IA
		_deviceContext->IASetVertexBuffers(0, 1, _vertexBuffer.GetAddressOf(), &stride, &offset);
		_deviceContext->IASetInputLayout(_inputLayout.Get());
		_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	// 삼각형으로 인지하고 그려달라

		// VS
		_deviceContext->VSSetShader(_vertexShader.Get(), nullptr, 0);

		// RS

		// PS
		_deviceContext->PSSetShader(_pixelShader.Get(), nullptr, 0);

		// OM

		_deviceContext->Draw(_vertices.size(), 0);
	}

	RenderEnd();
}

void Game::RenderBegin()
{
	_deviceContext->OMSetRenderTargets(1, _renderTargetView.GetAddressOf(), nullptr); // 마지막에 GPU가 그림을 그리는데까지 성공했으면 그 결과물을 도화지에 그려줘
	_deviceContext->ClearRenderTargetView(_renderTargetView.Get(), _clearColor);	// 우리의 도화지를 비워주고
	_deviceContext->RSSetViewports(1, &_viewport);	// 렌더링 파이프라인 시작...
}

void Game::RenderEnd()
{	// 다 그린 것을 제출하겠다
	// [전면버퍼] <-Present- [후면버퍼]
	HRESULT hr = _swapChain->Present(1, 0);
	CHECK(hr);
}
