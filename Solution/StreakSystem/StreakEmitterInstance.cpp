#include "stdafx.h"

#include "Camera.h"
#include <d3dx11effect.h>
#include <DL_Debug.h>
#include "Effect.h"
#include "StreakEmitterData.h"
#include "StreakEmitterInstance.h"
#include "Engine.h"
#include <MathHelper.h>


Prism::StreakEmitterInstance::StreakEmitterInstance(StreakEmitterData& anEmitter)
	: myEmitter(anEmitter)
	, myTimeSinceLastSpawn(0)
	, myStreaks(128)
	, myStreakIndices(128)
{
	DL_ASSERT_EXP(myEmitter.myMaxNrOfStreaks <= MAX_NR_OF_StreakS_IN_EMITTER,
		"Max number of Streaks too high.");

	CreateVertexBuffer();
	CreateIndexBuffer();

	D3D11_BLEND_DESC blendDesc;
	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.IndependentBlendEnable = false;
	blendDesc.RenderTarget[0].BlendEnable = false;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;

	Engine::GetInstance()->GetDevice()->CreateBlendState(&blendDesc, &myBlendStateNoBlend);

	Engine::GetInstance()->SetDebugName(myBlendStateNoBlend, "StreakEmitterInstance::myBlendStateNoBlend");
}

Prism::StreakEmitterInstance::~StreakEmitterInstance()
{
	if (myVertexBufferWrapper.myVertexBuffer != nullptr)
	{
		myVertexBufferWrapper.myVertexBuffer->Release();
		myVertexBufferWrapper.myVertexBuffer = nullptr;
	}

	if (myIndexBufferWrapper.myIndexBuffer != nullptr)
	{
		myIndexBufferWrapper.myIndexBuffer->Release();
		myIndexBufferWrapper.myIndexBuffer = nullptr;
	}

	myBlendStateNoBlend->Release();
	myBlendStateNoBlend = nullptr;
}


void Prism::StreakEmitterInstance::Render(Camera* aCamera)
{
	UpdateVertexBuffer();

	myEmitter.myEffect->SetViewMatrix(CU::InverseSimple(aCamera->GetOrientation()));
	myEmitter.myEffect->SetProjectionMatrix(aCamera->GetProjection());
	if (myEmitter.myStreakData.myStreaksFollowEmitter == true)
	{
		myEmitter.myEffect->SetWorldMatrix(myOrientation);
	}
	else
	{
		myEmitter.myEffect->SetWorldMatrix(CU::Matrix44f());
	}

	myEmitter.myEffect->SetStreakAlphaDelta(myEmitter.myStreakData.myAlphaDelta);
	myEmitter.myEffect->SetStreakSizeDelta(myEmitter.myStreakData.mySizeDelta);
	myEmitter.myEffect->SetStreakStartAlpha(myEmitter.myStreakData.myStartAlpha);

	Engine::GetInstance()->GetContex()->IASetInputLayout(myEmitter.myStreakInputLayout);
	Engine::GetInstance()->GetContex()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ);
	Engine::GetInstance()->GetContex()->IASetVertexBuffers(myVertexBufferWrapper.myStartSlot
		, myVertexBufferWrapper.myNumberOfBuffers
		, &myVertexBufferWrapper.myVertexBuffer
		, &myVertexBufferWrapper.myStride
		, &myVertexBufferWrapper.myByteOffset);
	Engine::GetInstance()->GetContex()->IASetIndexBuffer(myIndexBufferWrapper.myIndexBuffer
		, myIndexBufferWrapper.myIndexBufferFormat
		, myIndexBufferWrapper.myByteOffset);

	D3DX11_TECHNIQUE_DESC td;

	HRESULT hr = myEmitter.myEffect->GetTechnique()->GetDesc(&td);
	DL_ASSERT_EXP(SUCCEEDED(hr) == true, "Failed to get technique description.");

	for (UINT i = 0; i < td.Passes; ++i)
	{
		myEmitter.myEffect->SetStreakTexture(myEmitter.myTexture);
		hr = myEmitter.myEffect->GetTechnique()->GetPassByIndex(i)->Apply(0, Engine::GetInstance()->GetContex());
		DL_ASSERT_EXP(SUCCEEDED(hr) == true, "Failed to get pass by index.");
		Engine::GetInstance()->GetContex()->Draw(myStreaks.Size(), 0);
	}

	float floatArray[4];
	floatArray[0] = 0;
	floatArray[1] = 0;
	floatArray[2] = 0;
	floatArray[3] = 0;
	Engine::GetInstance()->GetContex()->OMSetBlendState(myBlendStateNoBlend, floatArray, 0xFFFFFFFF);
}

void Prism::StreakEmitterInstance::Update(float aDeltaTime)
{
	EmitterUpdate(aDeltaTime);
	StreakUpdate(aDeltaTime);
}

void Prism::StreakEmitterInstance::CreateIndexBuffer()
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(unsigned int) * myEmitter.myMaxNrOfStreaks;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	HRESULT hr = Engine::GetInstance()->GetDevice()->CreateBuffer(&bd, nullptr, &myIndexBufferWrapper.myIndexBuffer);
	DL_ASSERT_EXP(SUCCEEDED(hr) == true, "Failed to create index buffer.");

	Engine::GetInstance()->SetDebugName(myIndexBufferWrapper.myIndexBuffer
		, "StreakEmitterInstance::myIndexBufferWrapper->myIndexBuffer");

	myIndexBufferWrapper.myByteOffset = 0;
	myIndexBufferWrapper.myIndexBufferFormat = DXGI_FORMAT_R32_UINT;
}

void Prism::StreakEmitterInstance::CreateVertexBuffer()
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(StreakInstance) * myEmitter.myMaxNrOfStreaks;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	HRESULT hr = Engine::GetInstance()->GetDevice()->CreateBuffer(&bd, nullptr, &myVertexBufferWrapper.myVertexBuffer);
	DL_ASSERT_EXP(SUCCEEDED(hr) == true, "Failed to create vertex buffer.");

	Engine::GetInstance()->SetDebugName(myVertexBufferWrapper.myVertexBuffer
		, "StreakEmitterInstance::myVertexBufferWrapper->myVertexBuffer");

	myVertexBufferWrapper.myStride = sizeof(StreakInstance);
	myVertexBufferWrapper.myByteOffset = 0;
	myVertexBufferWrapper.myStartSlot = 0;
	myVertexBufferWrapper.myNumberOfBuffers = 1;
}

void Prism::StreakEmitterInstance::EmitterUpdate(float aDeltaTime)
{

	//if (myEmitter.myEmissionLifeTime > 0)
	//{
		if (myTimeSinceLastSpawn > myEmitter.myEmissionRate && myStreaks.Size() < myEmitter.myMaxNrOfStreaks)
		{
			EmitStreak();
			myTimeSinceLastSpawn = 0;
		}

		myTimeSinceLastSpawn += aDeltaTime;
	//}

	//myEmitter.myEmissionLifeTime -= aDeltaTime;
}

void Prism::StreakEmitterInstance::EmitStreak()
{
	StreakInstance Streak;

	Streak.mySize = CU::Math::RandomRange<float>(myEmitter.myStreakData.myMinStartSize
		, myEmitter.myStreakData.myMaxStartSize);
	Streak.myTime = 0;

	Streak.myPosition = CU::Math::RandomVector(myPosition - myEmitter.myEmissionExtents * 0.5f, myPosition + myEmitter.myEmissionExtents * 0.5f);

	Streak.myVelocity = CU::Math::RandomVector(myEmitter.myMinVelocity, myEmitter.myMaxVelocity);

	Streak.myRotationDelta = CU::Math::RandomRange<float>(myEmitter.myMinRotationDelta, myEmitter.myMaxRotationDelta);

	Streak.myAlpha = myEmitter.myStreakData.myStartAlpha;

	Streak.myRotation = 0;

	myStreakIndices.Add(myStreaks.Size());
	myStreaks.Add(Streak);
}

void Prism::StreakEmitterInstance::StreakUpdate(float aDeltaTime)
{
	for (int i = myStreaks.Size() - 1; i >= 0; --i)
	{
		myStreaks[i].myPosition += myStreaks[i].myVelocity * aDeltaTime;
		myStreaks[i].myTime += aDeltaTime;
		myStreaks[i].myRotation += myStreaks[i].myRotationDelta * aDeltaTime;
		myStreaks[i].mySize += myEmitter.myStreakData.mySizeDelta * aDeltaTime;

		myStreaks[i].mySize = fmaxf(myStreaks[i].mySize, 0);

		if (myEmitter.myStreakData.myAffectedByGravity == true)
		{
			myStreaks[i].myVelocity += myEmitter.myGravity * aDeltaTime;
		}

		if (myStreaks[i].myTime > myEmitter.myStreakData.myLifeTime)
		{
			myStreaks.RemoveNonCyclicAtIndex(i);
			myStreakIndices.RemoveNonCyclicAtIndex(i);
		}
	}
}

void Prism::StreakEmitterInstance::UpdateVertexBuffer()
{
	if (myStreaks.Size() > 0)
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;

		HRESULT hr = Engine::GetInstance()->GetContex()->Map(myVertexBufferWrapper.myVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		DL_ASSERT_EXP(SUCCEEDED(hr) == true, "Failed to map buffer.");
		if (mappedResource.pData != nullptr)
		{
			StreakInstance* data = static_cast<StreakInstance*>(mappedResource.pData);
			memcpy(data, &myStreaks[0], sizeof(StreakInstance)*myStreaks.Size());
		}
		Engine::GetInstance()->GetContex()->Unmap(myVertexBufferWrapper.myVertexBuffer, 0);
	}
}

void Prism::StreakEmitterInstance::Reset()
{
	myStreaks.RemoveAll();
	myStreakIndices.RemoveAll();
}