#include "stdafx.h"
#include "Sprite.h"
#include <D3D11.h>
#include <d3dx11effect.h>
#include "Camera.h"
#include "Effect.h"
#include "EffectContainer.h"
#include "Engine.h"
#include "VertexBufferWrapper.h"
#include "IndexBufferWrapper.h"
#include "Surface.h"
#include "TextureContainer.h"

Prism::Sprite::Sprite(const std::string& aFileName, const CU::Vector2<float>& aSpriteSize
		, const CU::Vector2<float>& aHotSpot)
	: mySize(aSpriteSize)
	, myHotspot(aHotSpot)
	, myTexture(nullptr)
	, myShaderView(nullptr)
	, myTopLeftUV(0.f, 0.f)
	, myRightBottomUV(1.f, 1.f)
{
	myFileName = aFileName;

	myEffect = EffectContainer::GetInstance()->GetEffect("Data/Resource/Shader/S_effect_sprite.fx");
	myEffect->AddListener(this);

	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	InitInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), "Sprite::InputLayout");
	InitVertexBuffer(sizeof(VertexPosUV), D3D11_USAGE_IMMUTABLE, 0);
	InitIndexBuffer();

	DL_ASSERT_EXP(myFileName.size() > 0, "File name can't be empty.");
	InitSurface("DiffuseTexture", myFileName);
	InitBlendState("Sprite::BlendState");

	ZeroMemory(myInitData, sizeof(*myInitData));

	CreateVertices();
}

Prism::Sprite::Sprite(ID3D11Texture2D* aTexture, const CU::Vector2<float>& aSpriteSize
	, const CU::Vector2<float>& aHotSpot)
	: mySize(aSpriteSize)
	, myHotspot(aHotSpot)
	, myTexture(nullptr)
	, myShaderView(nullptr)
	, myTopLeftUV(0.f, 0.f)
	, myRightBottomUV(1.f, 1.f)
{
	myFileName = "Inited from ID3D11Texture";

	myEffect = EffectContainer::GetInstance()->GetEffect("Data/Resource/Shader/S_effect_sprite.fx");
	myEffect->AddListener(this);

	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	D3D11_TEXTURE2D_DESC desc;
	aTexture->GetDesc(&desc);
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	HRESULT hr = Engine::GetInstance()->GetDevice()->CreateTexture2D(&desc, NULL, &myTexture);
	if (FAILED(hr))
	{
		DL_ASSERT("Failed to CreateTexture2D");
	}
	Engine::GetInstance()->SetDebugName(myTexture, "Sprite::myTexture");

	hr = Engine::GetInstance()->GetDevice()->CreateShaderResourceView(myTexture, NULL, &myShaderView);
	if (FAILED(hr))
	{
		DL_ASSERT("Failed to CopyFromD3DTexture");
	}
	Engine::GetInstance()->SetDebugName(myShaderView, "Sprite::myShaderView");

	
	CopyFromD3DTexture(aTexture);

	InitInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), "Sprite::InputLayout");
	InitVertexBuffer(sizeof(VertexPosUV), D3D11_USAGE_IMMUTABLE, 0);
	InitSurface("DiffuseTexture", myShaderView);
	InitIndexBuffer();
	InitBlendState("Sprite::BlendState");

	ZeroMemory(myInitData, sizeof(*myInitData));

	CreateVertices();
}

void Prism::Sprite::Render(const CU::Vector2<float>& aPosition, const CU::Vector2<float>& aScale
	, const CU::Vector4<float>& aColor)
{
	Engine::GetInstance()->DisableZBuffer();

	myPosition = aPosition;
	//myOrientation.SetPos(aPosition);
	myScale = aScale;

	float blendFactor[4];
	blendFactor[0] = 0.f;
	blendFactor[1] = 0.f;
	blendFactor[2] = 0.f;
	blendFactor[3] = 0.f;

	//myEffect->SetBlendState(myBlendState, blendFactor);
	myEffect->SetProjectionMatrix(Engine::GetInstance()->GetOrthogonalMatrix());
	myEffect->SetPosAndScale(aPosition, aScale);
	myEffect->SetColor(aColor);
	myEffect->SetSpriteOrientation(myOrientation);

	BaseModel::Render();

	Engine::GetInstance()->EnableZBuffer();
}

void Prism::Sprite::CreateVertices()
{
	CU::GrowingArray<VertexPosUV> vertices(4);
	CU::GrowingArray<int> indices(6);

	float left = 0 - myHotspot.x;
	float right = left + mySize.x;
	float bottom = 0 - myHotspot.y;
	float top = bottom + mySize.y;

	VertexPosUV vert;
	vert.myPos = { left, top, 0.0f };
	vert.myUV = { myTopLeftUV.x, myTopLeftUV.y };
	vertices.Add(vert);

	vert.myPos = { right, bottom, 0.0f };
	vert.myUV = { myRightBottomUV.x, myRightBottomUV.y };
	vertices.Add(vert);

	vert.myPos = { left, bottom, 0.0f };
	vert.myUV = { myTopLeftUV.x, myRightBottomUV.y };
	vertices.Add(vert);

	vert.myPos = { right, top, 0.0f };
	vert.myUV = { myRightBottomUV.x, myTopLeftUV.y };
	vertices.Add(vert);


	indices.Add(0);
	indices.Add(1);
	indices.Add(2);

	indices.Add(0);
	indices.Add(3);
	indices.Add(1);

	SetupVertexBuffer(vertices.Size(), sizeof(VertexPosUV), reinterpret_cast<char*>(&vertices[0]), "Sprite::VertexBuffer");
	SetupIndexBuffer(indices.Size(), reinterpret_cast<char*>(&indices[0]), "Sprite::IndexBuffer");

	mySurfaces[0]->SetVertexCount(vertices.Size());
	mySurfaces[0]->SetIndexCount(indices.Size());
}

void Prism::Sprite::Rotate(float aRadians)
{
	myOrientation = CU::Matrix44<float>::CreateRotateAroundZ(aRadians) * myOrientation;
}

void Prism::Sprite::ResizeTexture(ID3D11Texture2D* aSrcTexture)
{
	myTexture->Release();
	myShaderView->Release();

	D3D11_TEXTURE2D_DESC desc;
	aSrcTexture->GetDesc(&desc);
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	HRESULT hr = Engine::GetInstance()->GetDevice()->CreateTexture2D(&desc, NULL, &myTexture);
	if (FAILED(hr))
	{
		DL_ASSERT("Failed to CreateTexture2D");
	}
	Engine::GetInstance()->SetDebugName(myTexture, "Sprite::myTexture");

	hr = Engine::GetInstance()->GetDevice()->CreateShaderResourceView(myTexture, NULL, &myShaderView);
	if (FAILED(hr))
	{
		DL_ASSERT("Failed to CopyFromD3DTexture");
	}
	Engine::GetInstance()->SetDebugName(myShaderView, "Sprite::myShaderView");

	CopyFromD3DTexture(aSrcTexture);

	mySurfaces.DeleteAll();
	InitSurface("DiffuseTexture", myShaderView);
}

void Prism::Sprite::CopyFromD3DTexture(ID3D11Texture2D* aTexture)
{
	DL_ASSERT_EXP(myTexture != nullptr, "Tried to CopyFromD3DTexture on a sprite that was created from file");
	Engine::GetInstance()->GetContex()->CopyResource(myTexture, aTexture);
}