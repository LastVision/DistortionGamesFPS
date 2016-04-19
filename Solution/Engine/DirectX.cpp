#include "stdafx.h"
#include "DirectX.h"
#include <D3D11.h>
#include "Texture.h"
#include <GameConstants.h>

#pragma comment(lib, "dxgi.lib")

namespace Prism
{
	DirectX::DirectX(HWND& aHwnd, SetupInfo& aSetupInfo)
		: myHWND(&aHwnd)
		, mySetupInfo(aSetupInfo)
		, myDebugInterface(nullptr)
		, myInfoQueue(nullptr)
	{
		myWindowSize.x = mySetupInfo.myScreenWidth;
		myWindowSize.y = mySetupInfo.myScreenHeight;
		D3DSetup();
	}

	DirectX::DirectX(HWND& aHwnd, SetupInfo& aSetupInfo, ID3D11Device* aDevice, ID3D11DeviceContext* aContext)
		: myHWND(&aHwnd)
		, mySetupInfo(aSetupInfo)
		, myDebugInterface(nullptr)
		, myInfoQueue(nullptr)
	{
		myWindowSize.x = mySetupInfo.myScreenWidth;
		myWindowSize.y = mySetupInfo.myScreenHeight;
		myDevice = aDevice;
		myContext = aContext;
		D3DSetup();
	}

	DirectX::~DirectX()
	{
		CleanD3D();
	}

	void DirectX::Present(const unsigned int aSyncInterval, const unsigned int aFlags)
	{
		mySwapChain->Present(aSyncInterval, aFlags);
	}

	void DirectX::Clear(const float aClearColor[4])
	{
		//myContext->OMSetRenderTargets(1, &myBackbufferRenderTarget, myBackbufferDepthStencil);
		myContext->ClearRenderTargetView(myBackbufferRenderTarget, aClearColor);
		myContext->ClearDepthStencilView(myBackbufferDepthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}

	void DirectX::SetDebugName(ID3D11DeviceChild* aChild, const std::string& aName)
	{
		if (aChild != nullptr)
		{
			aChild->SetPrivateData(WKPDID_D3DDebugObjectName, aName.size(), aName.c_str());
		}
	}

	void DirectX::OnResize(const int aWidth, const int aHeight)
	{
		myWindowSize.x = aWidth;
		myWindowSize.y = aHeight;

		myContext->OMSetRenderTargets(0, NULL, NULL);
		myBackbufferRenderTarget->Release();
		myBackbufferShaderResource->Release();
		myBackbufferTexture->Release();
		myContext->Flush();


		HRESULT result = mySwapChain->ResizeBuffers(1, aWidth, aHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
		if (result != S_OK)
		{
			DIRECTX_LOG("Failed to Resize SwapChain Buffers");
			DL_MESSAGE_BOX("Failed to Resize SwapChain Buffers", "DirectX: SwapChain", MB_ICONWARNING);
		}


		D3DBackbufferSetup(aWidth, aHeight);
		D3DViewPortSetup(aWidth, aHeight);
	}

	void DirectX::SetFullscreen(bool aFullscreenFlag)
	{
		HRESULT result = mySwapChain->SetFullscreenState(aFullscreenFlag, nullptr);
		if (result != S_OK)
		{
			DIRECTX_LOG("Failed to SetFullscreenState on SwapChain Buffers");
			DL_MESSAGE_BOX("Failed to SetFullscreenState on SwapChain Buffers", "DirectX: SwapChain", MB_ICONWARNING);
		}
	}

	bool DirectX::IsFullscreen() const
	{
		BOOL isFullscreen;

		HRESULT result = mySwapChain->GetFullscreenState(&isFullscreen, nullptr);
		if (result != S_OK)
		{
			DIRECTX_LOG("Failed to GetFullscreenState on SwapChain Buffers");
			DL_MESSAGE_BOX("Failed to GetFullscreenState on SwapChain Buffers", "DirectX: SwapChain", MB_ICONWARNING);
		}

		return isFullscreen == TRUE;
	}

	void DirectX::CleanD3D()
	{
		mySwapChain->SetFullscreenState(FALSE, NULL);

		SAFE_RELEASE(mySwapChain);
		SAFE_RELEASE(myBackbufferRenderTarget);
		SAFE_RELEASE(myBackbufferShaderResource);
		SAFE_RELEASE(myBackbufferTexture);
		SAFE_RELEASE(myBackbufferDepthStencil);
		SAFE_RELEASE(myDevice);

		SAFE_RELEASE(myRasterizerStates[static_cast<int>(eRasterizer::CULL_BACK)]);
		SAFE_RELEASE(myRasterizerStates[static_cast<int>(eRasterizer::NO_CULLING)]);
		SAFE_RELEASE(myRasterizerStates[static_cast<int>(eRasterizer::WIRE_FRAME)]);

		SAFE_RELEASE(myDepthStencilStates[static_cast<int>(eDepthStencil::Z_ENABLED)]);
		SAFE_RELEASE(myDepthStencilStates[static_cast<int>(eDepthStencil::Z_DISABLED)]);
		SAFE_RELEASE(myDepthStencilStates[static_cast<int>(eDepthStencil::READ_NO_WRITE)]);

		SAFE_DELETE(myViewPort);

		myContext->ClearState();
		myContext->Flush();
		SAFE_RELEASE(myContext);

#ifdef _DEBUG
		myDebugInterface->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
		myDebugInterface->Release();
#endif
	}

	ID3D11DepthStencilView* DirectX::GetDepthStencil()
	{
		return myBackbufferDepthStencil;
	}

	ID3D11RenderTargetView* DirectX::GetBackbuffer()
	{
		return myBackbufferRenderTarget;
	}

	ID3D11ShaderResourceView* DirectX::GetBackbufferView()
	{
		return myBackbufferShaderResource;
	}

	ID3D11Texture2D* DirectX::GetBackbufferTexture()
	{
		return myBackbufferTexture;
	}

	ID3D11Texture2D* DirectX::GetDepthbufferTexture()
	{
		return myDepthbufferTexture;
	}

	void DirectX::SetDepthStencil(ID3D11DepthStencilView* aStencil)
	{
		myBackbufferDepthStencil = aStencil;
		myContext->OMSetRenderTargets(1, &myBackbufferRenderTarget, myBackbufferDepthStencil);
	}

	void DirectX::RestoreDepthStencil()
	{
		myBackbufferDepthStencil = myOriginalBackbufferDepthStencil;
		myContext->OMSetRenderTargets(1, &myBackbufferRenderTarget, myBackbufferDepthStencil);

	}

	void DirectX::RestoreViewPort()
	{
		myContext->RSSetViewports(1, myViewPort);
	}

	void DirectX::SetBackBufferAsTarget()
	{
		myContext->OMSetRenderTargets(1, &myBackbufferRenderTarget, myBackbufferDepthStencil);
	}

	void DirectX::SetDepthBufferState(eDepthStencil aState)
	{
		DL_ASSERT_EXP(aState != eDepthStencil::_COUNT, "Invalid DepthStencilState");
		myDepthStencilState = aState;
		myContext->OMSetDepthStencilState(myDepthStencilStates[static_cast<int>(myDepthStencilState)], 1);
	}

	eDepthStencil DirectX::GetDepthBufferState() const
	{
		return myDepthStencilState;
	}

	void DirectX::SetRasterizeState(eRasterizer aState)
	{
		DL_ASSERT_EXP(aState != eRasterizer::_COUNT, "Invalid DepthStencilState");
		myRasterizerState = aState;
		myContext->RSSetState(myRasterizerStates[static_cast<int>(myRasterizerState)]);
	}

	eRasterizer DirectX::GetRasterizerState() const
	{
		return myRasterizerState;
	}


	bool DirectX::D3DSetup()
	{
		//if (D3DSwapChainSetup() == false)
		//{
		//	DIRECTX_LOG("Failed to Setup DirectX SwapChain");
		//	return false;
		//}

		if (D3DBackbufferSetup(mySetupInfo.myScreenWidth, mySetupInfo.myScreenHeight) == false)
		{
			DIRECTX_LOG("Failed to Setup Backbuffer");
			return false;
		}

		if (D3DViewPortSetup(mySetupInfo.myScreenWidth, mySetupInfo.myScreenHeight) == false)
		{
			DIRECTX_LOG("Failed to Setup DirectX ViewPort");
			return false;
		}

		if (D3DSetupDepthStencilStates() == false)
		{
			DIRECTX_LOG("Failed to Setup DepthStencil States");
			return false;
		}

		if (D3DSetupRasterizerStates() == false)
		{
			DIRECTX_LOG("Failed to Setup Rasterizer States");
			return false;
		}

		SetDepthBufferState(eDepthStencil::Z_ENABLED);
		SetRasterizeState(eRasterizer::CULL_BACK);


		DIRECTX_LOG("DirectX Setup Successful");
		return true;
	}

	bool DirectX::D3DSwapChainSetup()
	{
		unsigned int numerator = 0;
		unsigned int denominator = 1;

		if (GC::EnableVSync == true)
		{
			D3DGetRefreshRate(numerator, denominator);
		}

		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

		swapChainDesc.BufferCount = 1;
		swapChainDesc.BufferDesc.Width = mySetupInfo.myScreenWidth;
		swapChainDesc.BufferDesc.Height = mySetupInfo.myScreenHeight;
		//swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;
		swapChainDesc.OutputWindow = *myHWND;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.Windowed = true;

		HRESULT result = D3D11CreateDeviceAndSwapChain(NULL,
			D3D_DRIVER_TYPE_HARDWARE,
			NULL,
#ifdef _DEBUG

			D3D11_CREATE_DEVICE_DEBUG,
#else
			0,
#endif
			NULL,
			NULL,
			D3D11_SDK_VERSION,
			&swapChainDesc,
			&mySwapChain,
			&myDevice,
			NULL,
			&myContext);

		if (FAILED(result))
		{
			return false;
		}

#ifdef _DEBUG
		myDebugInterface = nullptr;
		result = myDevice->QueryInterface(__uuidof(ID3D11Debug), (void**)&myDebugInterface);
		if (FAILED(result))
		{
			DL_ASSERT("[DirectX]: Failed to Query DebugInterface");
			return false;
		}

		myInfoQueue = nullptr;
		if (FAILED(myDebugInterface->QueryInterface(__uuidof(ID3D11InfoQueue), (void**)&myInfoQueue)))
		{
			DL_ASSERT("[DirectX]: Failed to Query InfoQueue");
			return false;
		}

		myInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
		myInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);

		D3D11_MESSAGE_ID hide[] =
		{
			D3D11_MESSAGE_ID_DEVICE_PSSETSHADERRESOURCES_HAZARD,
			D3D11_MESSAGE_ID_DEVICE_OMSETRENDERTARGETS_HAZARD
			// Add more message IDs here as needed
		};

		D3D11_INFO_QUEUE_FILTER filter;
		memset(&filter, 0, sizeof(filter));
		filter.DenyList.NumIDs = _countof(hide);
		filter.DenyList.pIDList = hide;
		myInfoQueue->AddStorageFilterEntries(&filter);
		myInfoQueue->Release();
#endif

		return TRUE;
	}

	bool DirectX::D3DBackbufferSetup(int aWidth, int aHeight)
	{
		////BackbuffeTexture
		//mySwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&myBackbufferTexture);

		////BackbufferRenderTarget
		//myDevice->CreateRenderTargetView(myBackbufferTexture, NULL, &myBackbufferRenderTarget);
		//myDevice->CreateShaderResourceView(myBackbufferTexture, NULL, &myBackbufferShaderResource);

		//myContext->OMSetRenderTargets(1, &myBackbufferRenderTarget, NULL);


		//BackbufferDepthstencil
		D3D11_TEXTURE2D_DESC depthBufferInfo;
		ZeroMemory(&depthBufferInfo, sizeof(depthBufferInfo));

		depthBufferInfo.Width = aWidth;
		depthBufferInfo.Height = aHeight;
		depthBufferInfo.MipLevels = 1;
		depthBufferInfo.ArraySize = 1;
		depthBufferInfo.Format = DXGI_FORMAT_D32_FLOAT;
		depthBufferInfo.SampleDesc.Count = 1;
		depthBufferInfo.Usage = D3D11_USAGE_DEFAULT;
		depthBufferInfo.BindFlags = D3D11_BIND_DEPTH_STENCIL;


		HRESULT hr = myDevice->CreateTexture2D(&depthBufferInfo, NULL, &myDepthbufferTexture);
		if (FAILED(hr))
		{
			return false;
		}

		D3D11_DEPTH_STENCIL_VIEW_DESC stencilDesc;
		ZeroMemory(&stencilDesc, sizeof(stencilDesc));

		stencilDesc.Format = depthBufferInfo.Format;
		stencilDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

		stencilDesc.Texture2D.MipSlice = 0;

		hr = myDevice->CreateDepthStencilView(myDepthbufferTexture, &stencilDesc, &myBackbufferDepthStencil);
		if (FAILED(hr))
		{
			return false;
		}

		hr = myDevice->CreateDepthStencilView(myDepthbufferTexture, &stencilDesc, &myOriginalBackbufferDepthStencil);
		if (FAILED(hr))
		{
			return false;
		}

		return true;
	}

	bool DirectX::D3DViewPortSetup(int aWidth, int aHeight)
	{
		delete myViewPort;
		myViewPort = new D3D11_VIEWPORT();
		ZeroMemory(myViewPort, sizeof(D3D11_VIEWPORT));

		myViewPort->TopLeftX = 0;
		myViewPort->TopLeftY = 0;
		myViewPort->Width = static_cast<FLOAT>(aWidth);
		myViewPort->Height = static_cast<FLOAT>(aHeight);
		myViewPort->MinDepth = 0.f;
		myViewPort->MaxDepth = 1.f;

		myContext->RSSetViewports(1, myViewPort);

		return true;
	}

	bool DirectX::D3DSetupRasterizerStates()
	{
		D3D11_RASTERIZER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_RASTERIZER_DESC));
		desc.FrontCounterClockwise = false;
		desc.DepthBias = false;
		desc.DepthBiasClamp = 0;
		desc.SlopeScaledDepthBias = 0;
		desc.DepthClipEnable = false;
		desc.ScissorEnable = false;
		desc.MultisampleEnable = false;
		desc.AntialiasedLineEnable = false;


		desc.FillMode = D3D11_FILL_WIREFRAME;
		desc.CullMode = D3D11_CULL_BACK;
		myDevice->CreateRasterizerState(&desc, &myRasterizerStates[static_cast<int>(eRasterizer::WIRE_FRAME)]);


		desc.FillMode = D3D11_FILL_SOLID;
		desc.CullMode = D3D11_CULL_BACK;
		myDevice->CreateRasterizerState(&desc, &myRasterizerStates[static_cast<int>(eRasterizer::CULL_BACK)]);

		desc.FillMode = D3D11_FILL_SOLID;
		desc.CullMode = D3D11_CULL_NONE;
		myDevice->CreateRasterizerState(&desc, &myRasterizerStates[static_cast<int>(eRasterizer::NO_CULLING)]);

		return true;
	}

	bool DirectX::D3DSetupDepthStencilStates()
	{
		D3D11_DEPTH_STENCIL_DESC  stencilDesc;
		stencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
		stencilDesc.StencilEnable = true;
		stencilDesc.StencilReadMask = 0xFF;
		stencilDesc.StencilWriteMask = 0xFF;
		stencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		stencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		stencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		stencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		stencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		stencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		stencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		stencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;


		stencilDesc.DepthEnable = true;
		stencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		HRESULT hr = myDevice->CreateDepthStencilState(&stencilDesc, &myDepthStencilStates[static_cast<int>(eDepthStencil::Z_ENABLED)]);
		if (FAILED(hr))
		{
			return false;
		}
		SetDebugName(myDepthStencilStates[static_cast<int>(eDepthStencil::Z_ENABLED)], "DepthStencil::Z_ENABLED");

		stencilDesc.DepthEnable = false;
		stencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		hr = myDevice->CreateDepthStencilState(&stencilDesc, &myDepthStencilStates[static_cast<int>(eDepthStencil::Z_DISABLED)]);
		if (FAILED(hr))
		{
			return false;
		}
		SetDebugName(myDepthStencilStates[static_cast<int>(eDepthStencil::Z_DISABLED)], "DepthStencil::Z_DISABLED");

		stencilDesc.DepthEnable = true;
		stencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		stencilDesc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;
		stencilDesc.StencilEnable = false;
		stencilDesc.StencilReadMask = UINT8(0xFF);
		stencilDesc.StencilWriteMask = 0x0;

		stencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_ZERO;
		stencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_ZERO;
		stencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		stencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;

		stencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_ZERO;
		stencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_ZERO;
		stencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_ZERO;
		stencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_NEVER;

		hr = myDevice->CreateDepthStencilState(&stencilDesc, &myDepthStencilStates[static_cast<int>(eDepthStencil::READ_NO_WRITE)]);
		if (FAILED(hr))
		{
			return false;
		}
		SetDebugName(myDepthStencilStates[static_cast<int>(eDepthStencil::READ_NO_WRITE)], "DepthStencil::READ_NO_WRITE");

		return true;
	}

	void DirectX::D3DGetRefreshRate(unsigned int& aNumerator, unsigned int& aDenominator)
	{
		IDXGIFactory* factory;
		IDXGIAdapter* adapter;
		IDXGIOutput* adapterOutput;
		unsigned int numModes;
		DXGI_MODE_DESC* displayModeList;

		HRESULT result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
		result = factory->EnumAdapters(0, &adapter);
		result = adapter->EnumOutputs(0, &adapterOutput);
		adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
		displayModeList = new DXGI_MODE_DESC[numModes];
		result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);

		for (unsigned int i = 0; i < numModes; ++i)
		{
			if (displayModeList[i].Width == myWindowSize.x && displayModeList[i].Height == myWindowSize.y)
			{
				aNumerator = displayModeList[i].RefreshRate.Numerator;
				aDenominator = displayModeList[i].RefreshRate.Denominator;
			}
		}

		SAFE_ARRAY_DELETE(displayModeList);
		SAFE_RELEASE(adapterOutput);
		SAFE_RELEASE(adapter);
		SAFE_RELEASE(factory);
	}
}