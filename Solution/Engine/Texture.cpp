#include "stdafx.h"

#include <D3DX11tex.h>
#include <DL_Debug.h>
#include "Texture.h"
#include "TextureHelper.h"

namespace Prism
{
	Texture::Texture()
		: myFileName("Not initialized")
		, myShaderView(nullptr)
		, myRenderTargetView(nullptr)
		, myTextureFormat(0)
		, myDepthStencilView(nullptr)
		, myDepthStencilShaderView(nullptr)
		, myTexture(nullptr)
		, myDepthTexture(nullptr)
		, myIsDepthTexture(false)
	{
	}

	Texture::~Texture()
	{
		Release();
	}

	void Texture::Init(float aWidth, float aHeight, unsigned int aBindFlag
		, unsigned int aFormat)
	{
		myIsDepthTexture = false;
		myFileName = "Initied as SRV/RTV/DSV";
		myShaderView = nullptr;
		myRenderTargetView = nullptr;
		myTexture = nullptr;

		myDepthStencilView = nullptr;
		myDepthStencilShaderView = nullptr;
		myDepthTexture = nullptr;

		myTextureFormat = aFormat;

		if ((aBindFlag & D3D11_BIND_SHADER_RESOURCE) > 0 || (aBindFlag & D3D11_BIND_RENDER_TARGET) > 0)
		{
			D3D11_TEXTURE2D_DESC tempBufferInfo;
			tempBufferInfo.Width = static_cast<unsigned int>(aWidth);
			tempBufferInfo.Height = static_cast<unsigned int>(aHeight);
			tempBufferInfo.MipLevels = 1;
			tempBufferInfo.ArraySize = 1;
			tempBufferInfo.Format = static_cast<DXGI_FORMAT>(myTextureFormat);
			tempBufferInfo.SampleDesc.Count = 1;
			tempBufferInfo.SampleDesc.Quality = 0;
			tempBufferInfo.Usage = D3D11_USAGE_DEFAULT;
			tempBufferInfo.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
			tempBufferInfo.CPUAccessFlags = 0;
			tempBufferInfo.MiscFlags = 0;

			HRESULT hr = Engine::GetInstance()->GetDevice()->CreateTexture2D(&tempBufferInfo, NULL, &myTexture);
			Engine::GetInstance()->SetDebugName(myTexture, "Texture::myTexture");

			if ((aBindFlag & D3D11_BIND_SHADER_RESOURCE) > 0)
			{
				hr = Engine::GetInstance()->GetDevice()->CreateShaderResourceView(myTexture, NULL, &myShaderView);
				if (FAILED(hr))
					assert(0);

				Engine::GetInstance()->SetDebugName(myShaderView, "Texture::myShaderView");
			}

			if ((aBindFlag & D3D11_BIND_RENDER_TARGET) > 0)
			{
				hr = Engine::GetInstance()->GetDevice()->CreateRenderTargetView(myTexture, NULL, &myRenderTargetView);
				if (FAILED(hr))
					assert(0);

				Engine::GetInstance()->SetDebugName(myRenderTargetView, "Texture::myRenderTargetView");
			}
		}

		if ((aBindFlag & D3D11_BIND_DEPTH_STENCIL) > 0)
		{
			CreateDepthStencilView(aWidth, aHeight);
		}
	}

	void Texture::InitAsDepthBuffer(float aWidth, float aHeight)
	{
		myIsDepthTexture = true;
		/*int width = 1024 * 2;
		int height = 1024 * 2;*/

		int width = static_cast<int>(aWidth);
		int height = static_cast<int>(aHeight);

		myFileName = "Initied as DSV";
		myShaderView = nullptr;
		myRenderTargetView = nullptr;
		myTexture = nullptr;

		myDepthStencilView = nullptr;
		myDepthStencilShaderView = nullptr;
		myDepthTexture = nullptr;

		D3D11_TEXTURE2D_DESC textureBufferInfo;
		textureBufferInfo.Width = width;
		textureBufferInfo.Height = height;
		textureBufferInfo.MipLevels = 1;
		textureBufferInfo.ArraySize = 1;
		textureBufferInfo.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		textureBufferInfo.SampleDesc.Count = 1;
		textureBufferInfo.SampleDesc.Quality = 0;
		textureBufferInfo.Usage = D3D11_USAGE_DEFAULT;
		int flags = 0;
		flags |= D3D11_BIND_SHADER_RESOURCE;
		flags |= D3D11_BIND_RENDER_TARGET;

		textureBufferInfo.BindFlags = flags;
		textureBufferInfo.CPUAccessFlags = 0;
		textureBufferInfo.MiscFlags = 0;
		HRESULT hr = Engine::GetInstance()->GetDevice()->CreateTexture2D(&textureBufferInfo, NULL, &myTexture);
		DL_ASSERT_EXP(hr == S_OK, "Failed to create texture");

		hr = Engine::GetInstance()->GetDevice()->CreateRenderTargetView(myTexture, NULL, &myRenderTargetView);
		if (FAILED(hr))
			assert(0);

		CreateDepthStencilView(static_cast<float>(width), static_cast<float>(height));

		/*D3D11_TEXTURE2D_DESC tempBufferInfo;
		tempBufferInfo.Width = width;
		tempBufferInfo.Height = height;
		tempBufferInfo.MipLevels = 1;
		tempBufferInfo.ArraySize = 1;

		tempBufferInfo.Format = DXGI_FORMAT_R32_TYPELESS;
		tempBufferInfo.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;

		tempBufferInfo.SampleDesc.Count = 1;
		tempBufferInfo.SampleDesc.Quality = 0;
		tempBufferInfo.Usage = D3D11_USAGE_DEFAULT;
		tempBufferInfo.CPUAccessFlags = 0;
		tempBufferInfo.MiscFlags = 0;

		hr = Engine::GetInstance()->GetDevice()->CreateTexture2D(&tempBufferInfo, NULL, &myDepthTexture);
		if (FAILED(hr))
		assert(0);
		Engine::GetInstance()->SetDebugName(myDepthTexture, "Texture::myDepthTexture");

		D3D11_DEPTH_STENCIL_VIEW_DESC depthDesc;
		ZeroMemory(&depthDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
		depthDesc.Format = DXGI_FORMAT_D32_FLOAT;
		depthDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthDesc.Texture2D.MipSlice = 0;

		hr = Engine::GetInstance()->GetDevice()->CreateDepthStencilView(myDepthTexture, &depthDesc, &myDepthStencilView);
		if (FAILED(hr))
		assert(0);


		D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
		ZeroMemory(&viewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		viewDesc.Format = DXGI_FORMAT_R32_FLOAT;
		viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		viewDesc.Texture2D.MipLevels = 1;
		viewDesc.Texture2D.MostDetailedMip = 0;

		hr = Engine::GetInstance()->GetDevice()->CreateShaderResourceView(myDepthTexture, &viewDesc, &myDepthStencilShaderView);
		if (FAILED(hr))
		assert(0);

		Engine::GetInstance()->SetDebugName(myDepthStencilShaderView, "Texture::myDepthStencilShaderView");*/
	}

	void Texture::CopyDepthBuffer(ID3D11Texture2D* aSource)
	{
		Engine::GetInstance()->GetContex()->CopyResource(myDepthTexture, aSource);
	}

	bool Texture::LoadTexture(const std::string& aFilePath)
	{
		HRESULT hr = TextureHelper::CreateShaderResourceViewFromFile(aFilePath, &myShaderView);

		myFileName = aFilePath;
		if (FAILED(hr) != S_OK)
		{
			RESOURCE_LOG("Failed to load: %s, trying to use MissingTexture-texture.", aFilePath.c_str());

#ifndef _DEBUG
			DL_MESSAGE_BOX(CU::Concatenate("Failed to load: %s,\ntrying to use MissingTexture-texture.", aFilePath.c_str()).c_str()
				, "Failed to load texture", MB_ICONWARNING);
#endif

			hr = D3DX11CreateShaderResourceViewFromFile(Engine::GetInstance()->GetDevice()
				, "Data/Resource/Texture/T_missing_texture.dds", NULL, NULL, &myShaderView, NULL);
			myFileName = "Data/Resource/Texture/T_missing_texture.dds";
			if (FAILED(hr) != S_OK)
			{
				DL_ASSERT("[Texture]: Failed to load MissingTexture-texture: Data/Resource/Texture/T_missing_texture.dds");
			}
		}

		Engine::GetInstance()->SetDebugName(myShaderView, "Texture::myShaderView");

		return true;
	}

	const std::string& Texture::GetFileName() const
	{
		return myFileName;
	}

	ID3D11ShaderResourceView* Texture::GetShaderView() const
	{
		DL_ASSERT_EXP(myShaderView != nullptr, "[Texture]: Tried to GetShaderView, but texture wasnt created with D3D11_BIND_SHADER_RESOURCE");
		return myShaderView;
	}

	ID3D11RenderTargetView* Texture::GetRenderTargetView() const
	{
		DL_ASSERT_EXP(myRenderTargetView != nullptr, "[Texture]: Tried to GetRenderTargetView, but texture wasnt created with D3D11_BIND_RENDER_TARGET");
		return myRenderTargetView;
	}

	ID3D11Texture2D* Texture::GetTexture() const
	{
		DL_ASSERT_EXP(myTexture != nullptr, "[Texture]: Tried to GetTexture, but texture wasnt created with D3D11_BIND_SHADER_RESOURCE");
		return myTexture;
	}

	ID3D11DepthStencilView* Texture::GetDepthStencilView() const
	{
		DL_ASSERT_EXP(myDepthStencilView != nullptr, "[Texture]: Tried to GetDepthStencilView, but texture wasnt created with D3D11_BIND_DEPTH_STENCIL");
		return myDepthStencilView;
	}

	ID3D11ShaderResourceView* Texture::GetDepthStencilShaderView() const
	{
		DL_ASSERT_EXP(myDepthStencilShaderView != nullptr, "[Texture]: Tried to GetDepthStencilShaderView, but texture wasnt created with D3D11_BIND_DEPTH_STENCIL");
		return myDepthStencilShaderView;
	}

	ID3D11Texture2D* Texture::GetDepthTexture() const
	{
		DL_ASSERT_EXP(myDepthStencilShaderView != nullptr, "[Texture]: Tried to GetDepthTexture, but texture wasnt created with D3D11_BIND_DEPTH_STENCIL");
		return myDepthTexture;
	}

	ID3D11RenderTargetView* Texture::GetCubemapRenderTarget(int aIndex)
	{
		return myCubemapRenderTargets[aIndex];
	}

	D3D11_VIEWPORT* Texture::GetViewPort()
	{
		return myViewPort;
	}

	void Texture::CreateDepthStencilView(float aWidth, float aHeight, int aArraySize)
	{
		D3D11_TEXTURE2D_DESC tempBufferInfo;
		tempBufferInfo.Width = static_cast<unsigned int>(aWidth);
		tempBufferInfo.Height = static_cast<unsigned int>(aHeight);
		tempBufferInfo.MipLevels = 1;
		tempBufferInfo.ArraySize = aArraySize;
		tempBufferInfo.Format = DXGI_FORMAT_R32_TYPELESS;
		tempBufferInfo.SampleDesc.Count = 1;
		tempBufferInfo.SampleDesc.Quality = 0;
		tempBufferInfo.Usage = D3D11_USAGE_DEFAULT;
		tempBufferInfo.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		tempBufferInfo.CPUAccessFlags = 0;
		tempBufferInfo.MiscFlags = 0;

		HRESULT hr = Engine::GetInstance()->GetDevice()->CreateTexture2D(&tempBufferInfo, NULL, &myDepthTexture);
		if (FAILED(hr))
			assert(0);

		Engine::GetInstance()->SetDebugName(myDepthTexture, "Texture::myDepthTexture");


		D3D11_DEPTH_STENCIL_VIEW_DESC depthDesc;
		ZeroMemory(&depthDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
		depthDesc.Format = DXGI_FORMAT_D32_FLOAT;
		depthDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthDesc.Texture2D.MipSlice = 0;

		hr = Engine::GetInstance()->GetDevice()->CreateDepthStencilView(myDepthTexture, &depthDesc, &myDepthStencilView);
		if (FAILED(hr))
			assert(0);


		D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
		ZeroMemory(&viewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		viewDesc.Format = DXGI_FORMAT_R32_FLOAT;
		viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		viewDesc.Texture2D.MipLevels = 1;
		viewDesc.Texture2D.MostDetailedMip = 0;

		hr = Engine::GetInstance()->GetDevice()->CreateShaderResourceView(myDepthTexture, &viewDesc, &myDepthStencilShaderView);
		if (FAILED(hr))
			assert(0);

		Engine::GetInstance()->SetDebugName(myDepthStencilShaderView, "Texture::myDepthStencilShaderView");
	}

	void Texture::CreateCubemap(float aWidth, float aHeight)
	{
		D3D11_TEXTURE2D_DESC tempBufferInfo;
		tempBufferInfo.Width = static_cast<unsigned int>(aWidth);
		tempBufferInfo.Height = static_cast<unsigned int>(aHeight);
		tempBufferInfo.MipLevels = 1;
		tempBufferInfo.ArraySize = 6;
		tempBufferInfo.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		tempBufferInfo.SampleDesc.Count = 1;
		tempBufferInfo.SampleDesc.Quality = 0;
		tempBufferInfo.Usage = D3D11_USAGE_DEFAULT;
		tempBufferInfo.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		tempBufferInfo.CPUAccessFlags = 0;
		tempBufferInfo.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

		HRESULT hr = Engine::GetInstance()->GetDevice()->CreateTexture2D(&tempBufferInfo, NULL, &myTexture);
		if (FAILED(hr))
			assert(0);

		Engine::GetInstance()->SetDebugName(myTexture, "Texture::myDepthTexture");

		hr = Engine::GetInstance()->GetDevice()->CreateShaderResourceView(myTexture, NULL, &myShaderView);
		if (FAILED(hr))
			assert(0);


		D3D11_RENDER_TARGET_VIEW_DESC renderTargetDesc;
		ZeroMemory(&renderTargetDesc, sizeof(renderTargetDesc));
		renderTargetDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		renderTargetDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
		renderTargetDesc.Texture2DArray.MipSlice = 0;
		renderTargetDesc.Texture2DArray.ArraySize = 1;

		for (int i = 0; i < 6; ++i)
		{
			renderTargetDesc.Texture2DArray.FirstArraySlice = i;
			hr = Engine::GetInstance()->GetDevice()->CreateRenderTargetView(myTexture, &renderTargetDesc, &myCubemapRenderTargets[i]);
			if (FAILED(hr))
				assert(0);
		}

		myViewPort = new D3D11_VIEWPORT();
		ZeroMemory(myViewPort, sizeof(D3D11_VIEWPORT));

		myViewPort->TopLeftX = 0;
		myViewPort->TopLeftY = 0;
		myViewPort->Width = aWidth;
		myViewPort->Height = aHeight;
		myViewPort->MinDepth = 0.f;
		myViewPort->MaxDepth = 1.f;

	}


	void Texture::Create3DTexture(float* aData, int aWidth, int aHeight, int aDepth)
	{
		D3D11_TEXTURE3D_DESC desc;
		desc.Depth = aDepth;
		desc.Width = aWidth;
		desc.Height = aHeight;
		desc.MipLevels = 1;
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA subData;
		subData.pSysMem = &aData;
		subData.SysMemPitch = sizeof(float) * 4 * aWidth;
		subData.SysMemSlicePitch = sizeof(float) * 4 * aHeight * aWidth;

		HRESULT hr = Engine::GetInstance()->GetDevice()->CreateTexture3D(&desc, &subData, &myD3D11Texture3D);
		if (FAILED(hr))
			assert(0);

		if (hr != S_OK)
		{
			DL_ASSERT("FAILED TO CREATE 3D TEXTURE");
		}

		hr = Engine::GetInstance()->GetDevice()->CreateShaderResourceView(myD3D11Texture3D, NULL, &myShaderView);
		if (FAILED(hr))
			assert(0);
	}

	void Texture::ClearDepth()
	{
		Engine::GetInstance()->GetContex()->ClearDepthStencilView(myDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}

	void Texture::Release()
	{
		if (myShaderView != nullptr)
		{
			myShaderView->Release();
		}

		if (myRenderTargetView != nullptr)
		{
			myRenderTargetView->Release();
		}

		if (myTexture != nullptr)
		{
			myTexture->Release();
		}

		if (myDepthStencilView != nullptr)
		{
			myDepthStencilView->Release();
		}

		if (myDepthStencilShaderView != nullptr)
		{
			myDepthStencilShaderView->Release();
		}

		if (myDepthTexture != nullptr)
		{
			myDepthTexture->Release();
		}
	}

	void Texture::Resize(float aWidth, float aHeight)
	{
		int bindFlag = 0;
		if (myShaderView != nullptr)
		{
			bindFlag |= D3D11_BIND_SHADER_RESOURCE;
			myShaderView->Release();
			myShaderView = nullptr;
		}

		if (myRenderTargetView != nullptr)
		{
			bindFlag |= D3D11_BIND_RENDER_TARGET;
			myRenderTargetView->Release();
			myRenderTargetView = nullptr;
		}

		if (myDepthStencilView != nullptr)
		{
			bindFlag |= D3D11_BIND_DEPTH_STENCIL;
			myDepthStencilView->Release();
			myDepthStencilView = nullptr;
			myDepthStencilShaderView->Release();
			myDepthStencilShaderView = nullptr;
		}

		if (myIsDepthTexture == true)
		{
			InitAsDepthBuffer(aWidth, aHeight);
		}
		else
		{
			Init(aWidth, aHeight, bindFlag, myTextureFormat);
		}
	}

	void Texture::SaveToFile(const std::string& aFilePath)
	{
		D3DX11SaveTextureToFile(Engine::GetInstance()->GetContex(), myTexture, D3DX11_IFF_DDS, aFilePath.c_str());
	}

	void Texture::Save3DToFile(const std::string& aFilePath)
	{
		D3DX11SaveTextureToFile(Engine::GetInstance()->GetContex(), myD3D11Texture3D, D3DX11_IFF_DDS, aFilePath.c_str());
	}
}