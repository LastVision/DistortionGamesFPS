#pragma once
#include "SetupInfo.h"

struct ID3D11Debug;
struct ID3D11DepthStencilState;
struct ID3D11DepthStencilView;
struct ID3D11DepthStencilState;
struct ID3D11Device;
struct ID3D11DeviceChild;
struct ID3D11DeviceContext;
struct ID3D11InfoQueue;
struct ID3D11RasterizerState;
struct ID3D11RenderTargetView;
struct ID3D11Texture2D;
struct ID3D11ShaderResourceView;
struct IDXGISwapChain;
struct D3D11_VIEWPORT;
struct SetupInfo;


namespace Prism
{
	class DirectX
	{
	public:
		DirectX(HWND& aHwnd, SetupInfo& aSetupInfo);
		~DirectX();

		void Present(const unsigned int aSyncInterval, const unsigned int aFlags);
		void Clear(const float aClearColor[4]);

		void SetDebugName(ID3D11DeviceChild* aChild, const std::string& aName);

		void OnResize(int aWidth, int aHeigth);
		void SetFullscreen(bool aFullscreenFlag);
		bool IsFullscreen() const;

		ID3D11Device* GetDevice();
		ID3D11DeviceContext* GetContex();
		ID3D11DepthStencilView* GetDepthStencil();
		ID3D11RenderTargetView* GetBackbuffer();
		ID3D11ShaderResourceView* GetBackbufferView();
		ID3D11Texture2D* GetBackbufferTexture();
		ID3D11Texture2D* GetDepthbufferTexture();

		void SetDepthStencil(ID3D11DepthStencilView* aStencil);
		void RestoreDepthStencil();

		void RestoreViewPort();
		void SetBackBufferAsTarget();


		void SetDepthBufferState(eDepthStencil aState);
		eDepthStencil GetDepthBufferState() const;

		void SetRasterizeState(eRasterizer aState);
		eRasterizer GetRasterizerState() const;

	private:
		void operator=(const DirectX&) = delete;

		void CleanD3D();
		bool D3DSetup();
		bool D3DSwapChainSetup();
		bool D3DBackbufferSetup(int aWidth, int aHeight);
		bool D3DViewPortSetup(int aWidth, int aHeight);
		bool D3DSetupRasterizerStates();
		bool D3DSetupDepthStencilStates();
		void D3DGetRefreshRate(unsigned int& aNumerator, unsigned int& aDenominator);

		ID3D11Device* myDevice;
		ID3D11DeviceContext* myContext;
		ID3D11Debug* myDebugInterface;
		ID3D11InfoQueue *myInfoQueue;
		IDXGISwapChain* mySwapChain;

		ID3D11RenderTargetView* myBackbufferRenderTarget;
		ID3D11DepthStencilView* myBackbufferDepthStencil;
		ID3D11DepthStencilView* myOriginalBackbufferDepthStencil;
		ID3D11ShaderResourceView* myBackbufferShaderResource;
		ID3D11Texture2D* myDepthbufferTexture;
		ID3D11Texture2D* myBackbufferTexture;

		ID3D11RasterizerState* myRasterizerStates[static_cast<int>(eRasterizer::_COUNT)];
		ID3D11DepthStencilState* myDepthStencilStates[static_cast<int>(eDepthStencil::_COUNT)];

		D3D11_VIEWPORT* myViewPort;

		eDepthStencil myDepthStencilState;
		eRasterizer myRasterizerState;

		HWND& myHWND;
		SetupInfo mySetupInfo;
		CU::Vector2<int> myWindowSize;
	};
}

inline ID3D11Device* Prism::DirectX::GetDevice()
{
	DL_ASSERT_EXP(myDevice != nullptr, "DirectX: myDevice is nullptr, HOW?!");
	return myDevice;
}

inline ID3D11DeviceContext* Prism::DirectX::GetContex()
{
	DL_ASSERT_EXP(myContext != nullptr, "DirectX: myContex is nullptr, HOW?!");
	return myContext;
}