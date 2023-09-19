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

void Game::Render()
{
	RenderBegin();


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
