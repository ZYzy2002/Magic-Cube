#pragma once
#include <Windows.h>
#include <wrl/client.h>

#include "d3d11.h"
#pragma comment(lib,"d3d11.lib")

class mGraphic
{
public:
	mGraphic(HWND hwnd);


	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwapChain;
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDepthStencilView;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pRenderTarget;

	void Clear();
	void Swap();

	void TestDraw();
};

