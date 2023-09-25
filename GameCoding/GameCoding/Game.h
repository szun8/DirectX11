#pragma once
class Game
{
public:
	Game();
	~Game();

public:
	void Init(HWND hwnd);
	void Update();
	void Render();

private:
	void RenderBegin();
	void RenderEnd();

private:
	void CreateDeviceAndSwapChain();
	void CreateRenderTargetView();
	void SetViewport();

private:
	void CreateGeometry();		// 도형 생성
	void CreateInputLayout();	// 도형에 대한 설명서

	void CreateVS();
	void CreatePS();

	void LoadShaderFromFile(const wstring& path, const string& name, const string& version, ComPtr<ID3DBlob>& blob);

private:
	HWND _hwnd;
	uint32 _width = 0;
	uint32 _height = 0;

private:
	// Device & SwapChain
	ComPtr<ID3D11Device> _device = nullptr;
	ComPtr<ID3D11DeviceContext> _deviceContext = nullptr;
	ComPtr<IDXGISwapChain> _swapChain = nullptr;

	// RenderTargetView
	ComPtr<ID3D11RenderTargetView> _renderTargetView;	// 후면 버퍼를 묘사하는 존재...

	// Misc
	D3D11_VIEWPORT _viewport = { 0 };
	float _clearColor[4] = {0.f, 0.f, 0.f, 0.f};

private:
	// Geometry
	vector<Vertex> _vertices;

	// [CPU <->RAM] [GPU<->VRAM]
	// 동적할당한 정점데이터는 RAM에 존재 -> GPU한테도 똑같이 정보를 넘겨 만들어줘야함
	ComPtr<ID3D11Buffer> _vertexBuffer = nullptr;
	ComPtr<ID3D11InputLayout> _inputLayout = nullptr;

	// Shdaer Load
	// VS
	ComPtr<ID3D11VertexShader> _vertexShader = nullptr;
	ComPtr<ID3DBlob> _vsBlob = nullptr;
	// PS
	ComPtr<ID3D11PixelShader> _pixelShader = nullptr;
	ComPtr<ID3DBlob> _psBlob = nullptr;

};

