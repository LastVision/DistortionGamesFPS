#include "stdafx.h"
#include "Font.h"
#include "Surface.h"
#include "Text.h"
#include "Texture.h"

#include "Engine.h"

Prism::Text::Text(const Font& aFont)
	: myFont(aFont)
	, myColor(1.f, 1.f, 1.f, 1.f)
{
	myEffect = EffectContainer::GetInstance()->GetEffect("Data/Resource/Shader/S_effect_font.fx");
	myScale = { 1.f, 1.f };

	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	InitInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), "Text::InputLayout");
	InitVertexBuffer(sizeof(VertexPosUV), D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
	InitIndexBuffer();
	InitSurface("DiffuseTexture", myFont.GetTexture()->GetFileName());
	InitBlendState("Text::BlendState");

	ZeroMemory(myInitData, sizeof(*myInitData));

	myVertices.Init(1024);
	myIndices.Init(1024);

	SetText(" ");
}


void Prism::Text::SetText(const std::string& aText)
{
	if (myText == aText)
	{
		return;
	}

	float aspect = Engine::GetInstance()->GetWindowSize().x / Engine::GetInstance()->GetWindowSize().y;

	if (aspect < 1.5f)
	{
		myScale = CU::Vector2<float>(0.5f, 0.5f);
	}

	if (aText == "")
	{
		myText = " ";
	}
	else
	{
		myText = aText;
	}

	ConstructBuffers();
}

void Prism::Text::Render()
{
	if (Engine::GetInstance()->myWireframeShouldShow == true)
	{
		Engine::GetInstance()->DisableWireframe();
	}
	Engine::GetInstance()->DisableZBuffer();
	
	float blendFactor[4];
	blendFactor[0] = 0.f;
	blendFactor[1] = 0.f;
	blendFactor[2] = 0.f;
	blendFactor[3] = 0.f;

	myEffect->SetBlendState(myBlendState, blendFactor);
	myEffect->SetProjectionMatrix(Engine::GetInstance()->GetOrthogonalMatrix());
	myEffect->SetPosAndScale(myPosition, myScale);
	myEffect->SetColor(myColor);

	BaseModel::Render();

	Engine::GetInstance()->EnableZBuffer();

	if (Engine::GetInstance()->myWireframeShouldShow == true)
	{
		Engine::GetInstance()->EnableWireframe();
	}
}

float Prism::Text::GetWidth() const
{
	float width = 0.f;
	for (unsigned int i = 0; i < myText.length(); ++i)
	{
		width += myFont.GetCharData(myText[i]).myAdvanceX * myScale.x;
	}

	return width;
}

void Prism::Text::ConstructBuffers()
{
	int numOfLetters = myText.length();
	float drawX = 0;
	float drawY = 0;
	float z = 1.f;

	myVertices.RemoveAll();
	myIndices.RemoveAll();
	VertexPosUV vert;
	for (int i = 0, row = 0; i < numOfLetters; ++i)
	{
		if (myText[i] == '\n')
		{
			drawX = 0;
			drawY -= 33.f;
			++row;
			continue;
		}
		Font::CharacterData charData = myFont.GetCharData(myText[i]);

		float left = drawX + charData.myOffset.x;
		float right = left + charData.mySize.x;
		float top = drawY + charData.myOffset.y;
		float bottom = top - charData.mySize.y;


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


		int startIndex = (i - row) * 4;
		myIndices.Add(startIndex + 0);
		myIndices.Add(startIndex + 1);
		myIndices.Add(startIndex + 2);

		myIndices.Add(startIndex + 0);
		myIndices.Add(startIndex + 3);
		myIndices.Add(startIndex + 1);


		drawX += charData.myAdvanceX;
		z -= 0.001f;
	}


	ModelLoader* loader = ModelLoader::GetInstance();
	bool wePaused = false;
	if (loader->IsPaused() == false)
	{
		loader->Pause();
		wePaused = true;
	}

	SetupVertexBuffer(myVertices.Size(), sizeof(VertexPosUV), reinterpret_cast<char*>(&myVertices[0]), "Text::VertexBuffer");
	SetupIndexBuffer(myIndices.Size(), reinterpret_cast<char*>(&myIndices[0]), "Text::IndexBuffer");

	if (wePaused == true)
	{
		loader->UnPause();
	}

	mySurfaces[0]->SetVertexCount(myVertices.Size());
	mySurfaces[0]->SetIndexCount(myIndices.Size());
}