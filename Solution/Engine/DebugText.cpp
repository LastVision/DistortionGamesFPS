#include "stdafx.h"

#include <D3D11.h>
#include <d3dx11effect.h>
#include "Camera.h"
#include "Effect.h"
#include "EffectContainer.h"
#include "Engine.h"
#include "DebugFont.h"
#include "DebugText.h"
#include <string.h>
#include "VertexBufferWrapper.h"
#include "IndexBufferWrapper.h"
#include "Surface.h"

Prism::DebugText::DebugText()
	: BaseModel()
	, myVertices(16)
	, myIndices(16)
	, myFont(nullptr)
	, myTextWidth(0.f)
	, myCharSpacing(0.f)
	, myLastText("Not initialized.")
{
}

void Prism::DebugText::Init(DebugFont* aFont)
{
	myEffect = EffectContainer::GetInstance()->GetEffect("Data/Resource/Shader/S_effect_fontDebug.fx");
	myFont = aFont;
	myCharSize = myFont->GetCharSize();
	myCharSpacing = 17.f;
	myScale = { 1.f, 1.f };

	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	InitInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), "DebugText::InputLayout");
	InitVertexBuffer(sizeof(VertexPosUV), D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
	InitIndexBuffer();
	InitSurface("DiffuseTexture", myFont->GetTexture()->GetFileName());
	InitBlendState("DebugText::BlendState");

	ZeroMemory(myInitData, sizeof(*myInitData));
}

void Prism::DebugText::Render(const std::string& aString, const CU::Vector2<float>& aPosition
	, const CU::Vector2<float>& aScale, const CU::Vector4<float>& aColor)
{
	aColor;
	if (myLastText != aString)
	{
		ConstructBuffers(aString);
	}

	Engine::GetInstance()->SetDepthBufferState(eDepthStencil::Z_DISABLED);

	myPosition = aPosition;
	myScale = aScale;

	float blendFactor[4];
	blendFactor[0] = 0.f;
	blendFactor[1] = 0.f;
	blendFactor[2] = 0.f;
	blendFactor[3] = 0.f;

	myEffect->SetBlendState(myBlendState, blendFactor);
	myEffect->SetProjectionMatrix(Engine::GetInstance()->GetOrthogonalMatrix());
	myEffect->SetPosAndScale(aPosition, aScale);
	//myEffect->SetColor(aColor);

	BaseModel::Render();

	Engine::GetInstance()->SetDepthBufferState(eDepthStencil::Z_ENABLED);
}


CU::Vector2<float> Prism::DebugText::GetTextSize(const std::string& aString) const
{
	int numOfLetters = aString.length();
	CU::Vector2<float> size;


	size.x += (myCharSize.x - myCharSpacing) * numOfLetters;
	size.y = myCharSize.y;

	return size;
}

const CU::Vector2<float>& Prism::DebugText::GetCharSize() const
{
	return myCharSize;
}

void Prism::DebugText::ConstructBuffers(const std::string& aString)
{
	myLastText = aString;
	myTextWidth = 0;

	int numOfLetters = aString.length();
	float drawX = 0;
	float drawY = 0;
	float z = 1.f;

	myVertices.RemoveAll();
	myIndices.RemoveAll();
	VertexPosUV vert;
	for (int i = 0; i < numOfLetters; ++i)
	{
		DebugFont::CharacterData charData = myFont->GetCharData(aString[i]);

		float left = drawX;
		float right = left + myCharSize.x;
		float top = drawY;
		float bottom = top - myCharSize.y;


		vert.myPos = CU::Vector3<float>(left, top, z);
		vert.myUV = charData.myTopLeftUV;
		myVertices.Add(vert);

		vert.myPos = CU::Vector3<float>(right, bottom, z);
		vert.myUV = charData.myBottomRightUV;
		myVertices.Add(vert);

		vert.myPos = CU::Vector3<float>(left, bottom, z);
		vert.myUV = { charData.myTopLeftUV.x, charData.myBottomRightUV.y };
		myVertices.Add(vert);

		vert.myPos = CU::Vector3<float>(right, top, z);
		vert.myUV = { charData.myBottomRightUV.x, charData.myTopLeftUV.y };
		myVertices.Add(vert);


		int startIndex = i * 4;
		myIndices.Add(startIndex + 0);
		myIndices.Add(startIndex + 1);
		myIndices.Add(startIndex + 2);

		myIndices.Add(startIndex + 0);
		myIndices.Add(startIndex + 3);
		myIndices.Add(startIndex + 1);


		drawX += myCharSize.x - myCharSpacing;
		z -= 0.001f;
	}

	SetupVertexBuffer(myVertices.Size(), sizeof(VertexPosUV), reinterpret_cast<char*>(&myVertices[0])
		 , "DebugText::VertexBuffer");
	SetupIndexBuffer(myIndices.Size(), reinterpret_cast<char*>(&myIndices[0])
		, "DebugText::IndexBuffer");

	mySurfaces[0]->SetVertexCount(myVertices.Size());
	mySurfaces[0]->SetIndexCount(myIndices.Size());

	myTextWidth = drawX - myTextWidth;
}

float Prism::DebugText::GetTextWidth() const
{
	return myTextWidth;
}