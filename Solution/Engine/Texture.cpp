#include "stdafx.h"

//#include <CommonHelper.h>
#include <D3DX11tex.h>
#include <DL_Debug.h>
#include "Texture.h"
#include "TextureHelper.h"

Prism::Texture::Texture()
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

Prism::Texture::~Texture()
{
	Release();
}

void Prism::Texture::Init(float aWidth, float aHeight, unsigned int aBindFlag
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

void Prism::Texture::InitAsDepthBuffer()
{
	myIsDepthTexture = true;
	int width = Prism::Engine::GetInstance()->GetWindowSizeInt().x;
	int height = Prism::Engine::GetInstance()->GetWindowSizeInt().y;

	width = 1024 * 2;
	height = 1024 * 2;
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

	D3D11_TEXTURE2D_DESC tempBufferInfo;
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

	//tempBufferInfo.Format = DXGI_FORMAT_D32_FLOAT;
	hr = Engine::GetInstance()->GetDevice()->CreateDepthStencilView(myDepthTexture, &depthDesc, &myDepthStencilView);
	if (FAILED(hr))
		assert(0);


	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	ZeroMemory(&viewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	viewDesc.Format = DXGI_FORMAT_R32_FLOAT;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	viewDesc.Texture2D.MipLevels = 1;
	viewDesc.Texture2D.MostDetailedMip = 0;

	//tempBufferInfo.Format = static_cast<DXGI_FORMAT>(aFormat);
	hr = Engine::GetInstance()->GetDevice()->CreateShaderResourceView(myDepthTexture, &viewDesc, &myDepthStencilShaderView);
	if (FAILED(hr))
		assert(0);

	Engine::GetInstance()->SetDebugName(myDepthStencilShaderView, "Texture::myDepthStencilShaderView");
}

void Prism::Texture::CopyDepthBuffer(ID3D11Texture2D* aSource)
{
	Engine::GetInstance()->GetContex()->CopyResource(myDepthTexture, aSource);
}

bool Prism::Texture::LoadTexture(const std::string& aFilePath)
{
	//HRESULT hr = D3DX11CreateShaderResourceViewFromFile(Engine::GetInstance()->GetDevice(), aFilePath.c_str()
	//	, NULL, NULL, &myShaderView, NULL);
	//
	//if (FAILED(hr) == S_OK)
	//{
	//	ID3D11Resource* resource = nullptr;
	//	myShaderView->GetResource(&resource);
	//	ID3D11Texture2D* texture2D = reinterpret_cast<ID3D11Texture2D*>(resource);
	//	D3D11_TEXTURE2D_DESC* texture2DDEsc = new D3D11_TEXTURE2D_DESC;
	//	texture2D->GetDesc(texture2DDEsc);
	//	UINT width = texture2DDEsc->Width;
	//	UINT height = texture2DDEsc->Height;
	//	texture2D->Release();
	//	delete texture2DDEsc;
	//
	//	std::string errorMessage = "Texturesize not power of 2: [" + aFilePath + "].";
	//	DL_ASSERT_EXP(CU::IsValidTextureSize(height) && CU::IsValidTextureSize(width), errorMessage.c_str());
	//}

	HRESULT hr = TextureHelper::CreateShaderResourceViewFromFile(aFilePath, &myShaderView);


	myFileName = aFilePath;
	if (FAILED(hr) != S_OK)
	{
		RESOURCE_LOG("Failed to load: %s, trying to use MissingTexture-texture.", aFilePath.c_str());
		DL_MESSAGE_BOX(CU::Concatenate("Failed to load: %s,\ntrying to use MissingTexture-texture.", aFilePath.c_str()).c_str()
			, "Failed to load texture", MB_ICONWARNING);
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

const std::string& Prism::Texture::GetFileName() const
{
	return myFileName;
}

ID3D11ShaderResourceView* Prism::Texture::GetShaderView() const
{
	DL_ASSERT_EXP(myShaderView != nullptr, "[Texture]: Tried to GetShaderView, but texture wasnt created with D3D11_BIND_SHADER_RESOURCE");
	return myShaderView;
}

ID3D11RenderTargetView* Prism::Texture::GetRenderTargetView() const
{
	DL_ASSERT_EXP(myRenderTargetView != nullptr, "[Texture]: Tried to GetRenderTargetView, but texture wasnt created with D3D11_BIND_RENDER_TARGET");
	return myRenderTargetView;
}

ID3D11Texture2D* Prism::Texture::GetTexture() const
{
	DL_ASSERT_EXP(myTexture != nullptr, "[Texture]: Tried to GetTexture, but texture wasnt created with D3D11_BIND_SHADER_RESOURCE");
	return myTexture;
}

ID3D11DepthStencilView* Prism::Texture::GetDepthStencilView() const
{
	DL_ASSERT_EXP(myDepthStencilView != nullptr, "[Texture]: Tried to GetDepthStencilView, but texture wasnt created with D3D11_BIND_DEPTH_STENCIL");
	return myDepthStencilView;
}

ID3D11ShaderResourceView* Prism::Texture::GetDepthStencilShaderView() const
{
	DL_ASSERT_EXP(myDepthStencilShaderView != nullptr, "[Texture]: Tried to GetDepthStencilShaderView, but texture wasnt created with D3D11_BIND_DEPTH_STENCIL");
	return myDepthStencilShaderView;
}

ID3D11Texture2D* Prism::Texture::GetDepthTexture() const
{
	DL_ASSERT_EXP(myDepthStencilShaderView != nullptr, "[Texture]: Tried to GetDepthTexture, but texture wasnt created with D3D11_BIND_DEPTH_STENCIL");
	return myDepthTexture;
}

void Prism::Texture::ClearDepth()
{
	Engine::GetInstance()->GetContex()->ClearDepthStencilView(myDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void Prism::Texture::Release()
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

void Prism::Texture::Resize(float aWidth, float aHeight)
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
		InitAsDepthBuffer();
	}
	else
	{
		Init(aWidth, aHeight, bindFlag, myTextureFormat);
	}
}

void Prism::Texture::CreateDepthStencilView(float aWidth, float aHeight)
{
	D3D11_TEXTURE2D_DESC tempBufferInfo;
	tempBufferInfo.Width = static_cast<unsigned int>(aWidth);
	tempBufferInfo.Height = static_cast<unsigned int>(aHeight);
	tempBufferInfo.MipLevels = 1;
	tempBufferInfo.ArraySize = 1;
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

	//tempBufferInfo.Format = DXGI_FORMAT_D32_FLOAT;
	hr = Engine::GetInstance()->GetDevice()->CreateDepthStencilView(myDepthTexture, &depthDesc, &myDepthStencilView);
	if (FAILED(hr))
		assert(0);


	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	ZeroMemory(&viewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	viewDesc.Format = DXGI_FORMAT_R32_FLOAT;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	viewDesc.Texture2D.MipLevels = 1;
	viewDesc.Texture2D.MostDetailedMip = 0;

	//tempBufferInfo.Format = static_cast<DXGI_FORMAT>(aFormat);
	hr = Engine::GetInstance()->GetDevice()->CreateShaderResourceView(myDepthTexture, &viewDesc, &myDepthStencilShaderView);
	if (FAILED(hr))
		assert(0);

	Engine::GetInstance()->SetDebugName(myDepthStencilShaderView, "Texture::myDepthStencilShaderView");
}
