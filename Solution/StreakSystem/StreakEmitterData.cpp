#include "stdafx.h"

#include <d3dx11effect.h>
#include <DL_Debug.h>
#include "StreakEmitterData.h"
#include "Effect.h"
#include "EffectContainer.h"
#include "Engine.h"
#include "Texture.h"
#include "TextureContainer.h"
#include <XMLReader.h>

Prism::StreakEmitterData::StreakEmitterData()
	: myStreakInputLayout(nullptr)
{
}

Prism::StreakEmitterData::~StreakEmitterData()
{
	/*if (myStreakInputLayout != nullptr)
	{
		myStreakInputLayout->Release();
		myStreakInputLayout = nullptr;
	}*/
}

bool Prism::StreakEmitterData::LoadDataFile(const std::string& aFileName)
{
	XMLReader reader;
	reader.OpenDocument(aFileName);
	tinyxml2::XMLElement* element = reader.ForceFindFirstChild("root");

	element = reader.ForceFindFirstChild(element, "emitter");

	element = reader.ForceFindFirstChild(element, "myEffectName");
	reader.ForceReadAttribute(element, "value", myEffectName);

	element = reader.ForceFindNextElement(element, "myEmissionExtents");
	reader.ForceReadAttribute(element, "x", myEmissionExtents.x);
	reader.ForceReadAttribute(element, "y", myEmissionExtents.y);
	reader.ForceReadAttribute(element, "z", myEmissionExtents.z);

	element = reader.ForceFindNextElement(element, "myEmissionLifeTime");
	reader.ForceReadAttribute(element, "value", myEmissionLifeTime);

	element = reader.ForceFindNextElement(element, "myEmissionRate");
	reader.ForceReadAttribute(element, "value", myEmissionRate);

	element = reader.ForceFindNextElement(element, "myEmissionRateDelta");
	reader.ForceReadAttribute(element, "value", myEmissionRateDelta);

	element = reader.ForceFindNextElement(element, "myEmissionSpeed");
	reader.ForceReadAttribute(element, "value", myEmissionSpeed);

	element = reader.ForceFindNextElement(element, "myEmissionVelocityDelta");
	reader.ForceReadAttribute(element, "x", myEmissionVelocityDelta.x);
	reader.ForceReadAttribute(element, "y", myEmissionVelocityDelta.y);
	reader.ForceReadAttribute(element, "z", myEmissionVelocityDelta.z);

	element = reader.ForceFindNextElement(element, "myMaxNrOfStreaks");
	reader.ForceReadAttribute(element, "value", myMaxNrOfStreaks);

	element = reader.ForceFindNextElement(element, "myMaxRotation");
	reader.ForceReadAttribute(element, "x", myMaxRotation.x);
	reader.ForceReadAttribute(element, "y", myMaxRotation.y);
	reader.ForceReadAttribute(element, "z", myMaxRotation.z);

	element = reader.ForceFindNextElement(element, "myMaxVelocity");
	reader.ForceReadAttribute(element, "x", myMaxVelocity.x);
	reader.ForceReadAttribute(element, "y", myMaxVelocity.y);
	reader.ForceReadAttribute(element, "z", myMaxVelocity.z);

	element = reader.ForceFindNextElement(element, "myMinRotation");
	reader.ForceReadAttribute(element, "x", myMinRotation.x);
	reader.ForceReadAttribute(element, "y", myMinRotation.y);
	reader.ForceReadAttribute(element, "z", myMinRotation.z);

	element = reader.ForceFindNextElement(element, "myMinRotationDelta");
	reader.ForceReadAttribute(element, "value", myMinRotationDelta);

	element = reader.ForceFindNextElement(element, "myMaxRotationDelta");
	reader.ForceReadAttribute(element, "value", myMaxRotationDelta);

	element = reader.ForceFindNextElement(element, "myMinVelocity");
	reader.ForceReadAttribute(element, "x", myMinVelocity.x);
	reader.ForceReadAttribute(element, "y", myMinVelocity.y);
	reader.ForceReadAttribute(element, "z", myMinVelocity.z);

	element = reader.ForceFindNextElement(element, "myGravity");
	reader.ForceReadAttribute(element, "x", myGravity.x);
	reader.ForceReadAttribute(element, "y", myGravity.y);
	reader.ForceReadAttribute(element, "z", myGravity.z);


	element = reader.ForceFindNextElement(element, "myTextureName");
	reader.ForceReadAttribute(element, "value", myTextureName);

	element = reader.ForceFindFirstChild("root");
	element = reader.ForceFindFirstChild(element, "Streak");

	element = reader.ForceFindFirstChild(element, "myStreaksFollowEmitter");
	reader.ForceReadAttribute(element, "value", myStreakData.myStreaksFollowEmitter);

	element = reader.ForceFindNextElement(element, "myAffectedByGravity");
	reader.ForceReadAttribute(element, "value", myStreakData.myAffectedByGravity);

	element = reader.ForceFindNextElement(element, "myAlphaDelta");
	reader.ForceReadAttribute(element, "value", myStreakData.myAlphaDelta);

	element = reader.ForceFindNextElement(element, "myLifeTime");
	reader.ForceReadAttribute(element, "value", myStreakData.myLifeTime);

	element = reader.ForceFindNextElement(element, "myMaxStartSize");
	reader.ForceReadAttribute(element, "value", myStreakData.myMaxStartSize);

	element = reader.ForceFindNextElement(element, "myMinStartSize");
	reader.ForceReadAttribute(element, "value", myStreakData.myMinStartSize);

	element = reader.ForceFindNextElement(element, "myMaxVelocity");
	reader.ForceReadAttribute(element, "x", myStreakData.myMaxVelocity.x);
	reader.ForceReadAttribute(element, "y", myStreakData.myMaxVelocity.y);
	reader.ForceReadAttribute(element, "z", myStreakData.myMaxVelocity.z);

	element = reader.ForceFindNextElement(element, "myMinVelocity");
	reader.ForceReadAttribute(element, "x", myStreakData.myMinVelocity.x);
	reader.ForceReadAttribute(element, "y", myStreakData.myMinVelocity.y);
	reader.ForceReadAttribute(element, "z", myStreakData.myMinVelocity.z);

	element = reader.ForceFindNextElement(element, "mySizeDelta");
	reader.ForceReadAttribute(element, "value", myStreakData.mySizeDelta);

	element = reader.ForceFindNextElement(element, "myStartAlpha");
	reader.ForceReadAttribute(element, "value", myStreakData.myStartAlpha);

	reader.CloseDocument();

	myTexture = TextureContainer::GetInstance()->GetTexture(myTextureName);
	myEffect = EffectContainer::GetInstance()->GetEffect(myEffectName);

	return CreateInputLayout();
}

bool Prism::StreakEmitterData::CreateInputLayout()
{
	D3DX11_PASS_DESC passDesc;
	HRESULT hr = myEffect->GetTechnique(false)->GetPassByIndex(0)->GetDesc(&passDesc);
	DL_ASSERT_EXP(SUCCEEDED(hr) == true, "Unable to get pass description.");

	const D3D11_INPUT_ELEMENT_DESC StreakLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "VELOCITY", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "SIZE", 0, DXGI_FORMAT_R32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "ALPHA", 0, DXGI_FORMAT_R32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TIME", 0, DXGI_FORMAT_R32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "ROTATION", 0, DXGI_FORMAT_R32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "ROTATION_DELTA", 0, DXGI_FORMAT_R32_FLOAT, 0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT nrOfElements = ARRAYSIZE(StreakLayout);

	hr = Engine::GetInstance()->GetDevice()->CreateInputLayout(StreakLayout
		, nrOfElements
		, passDesc.pIAInputSignature
		, passDesc.IAInputSignatureSize
		, &myStreakInputLayout);

	if (FAILED(hr) == true)
	{
		DL_ASSERT("Failed to create input layout.");
		return false;
	}

	Engine::GetInstance()->SetDebugName(myStreakInputLayout, "StreakEmitterData::myStreakInputLayout");

	return true;
}