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
	ZeroMemory(&desc, sizeof(desc));	// �޸𸮸� �� 0���� �ʱ�ȭ
	{	// ��µǾ��� ȭ��ũ�⿡ ���� ������ ����� �����ϰ� �ʱ�ȭ
		desc.BufferDesc.Width = _width;
		desc.BufferDesc.Height = _height;
		desc.BufferDesc.RefreshRate.Denominator = 1;	// ȭ�� �ֻ���, �и�
		desc.BufferDesc.RefreshRate.Numerator = 60;		// ����
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;	// ��Ƽ ���ø��� ���õ� ��, �ϴ� �����ؼ� 1��
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	// ���� ������� �׷��ִ� ���ҷ� ���۸� ����ϰڴ�
		desc.BufferCount = 1;	// �ĸ� ���۴� �ϳ��� ���
		desc.OutputWindow = _hwnd;
		desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	}

	// �ĸ� ����
	HRESULT hr = ::D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,	// GPU�� ����ϰڴ�
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
	CHECK(hr);	// ������ �ƴϸ� ũ������ �� (pch.h�� ��ũ�� �����س���)
}

void Game::CreateRenderTargetView()
{
	HRESULT hr;
	ComPtr<ID3D11Texture2D> backBuffer = nullptr;
	hr = _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)backBuffer.GetAddressOf());
	// �ؽ��ĸ� ���� BackBuffer �ּҿ� �־��ֱ�
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
	_deviceContext->OMSetRenderTargets(1, _renderTargetView.GetAddressOf(), nullptr); // �������� GPU�� �׸��� �׸��µ����� ���������� �� ������� ��ȭ���� �׷���
	_deviceContext->ClearRenderTargetView(_renderTargetView.Get(), _clearColor);	// �츮�� ��ȭ���� ����ְ�
	_deviceContext->RSSetViewports(1, &_viewport);	// ������ ���������� ����...
}

void Game::RenderEnd()
{	// �� �׸� ���� �����ϰڴ�
	// [�������] <-Present- [�ĸ����]
	HRESULT hr = _swapChain->Present(1, 0);
	CHECK(hr);
}
