#include "stdafx.h"

#include "Camera.h"
#include <CommonHelper.h>
#include <d3dx11effect.h>
#include <MathHelper.h>
#include "ParticleEmitterInstance.h"
#include "VertexBufferWrapper.h"
#include <Effect.h>
#include <TextureContainer.h>
#include <Engine.h>

namespace Prism
{
	ParticleEmitterInstance::ParticleEmitterInstance(ParticleEmitterData* someData, bool anAllowManyParticles)
		: myVertexWrapper(nullptr)
		, myEmissionTime(0)
		, myParticleIndex(0)
		, myLiveParticleCount(0)
		, myAlwaysShow(false)
		, myHasEmitted(false)
		, myParticleToGraphicsCard(256)
	{
		myStates.reset();
		myParticleEmitterData = someData;
		myEmitterPath = myParticleEmitterData->myFileName;

		int particleCount = static_cast<int>(myParticleEmitterData->myParticlesPerEmitt * myParticleEmitterData->myParticlesLifeTime / myParticleEmitterData->myEmissionRate) + 1;


		DL_DEBUG(("Loading :" + myEmitterPath).c_str());
		DL_ASSERT_EXP(anAllowManyParticles == true || particleCount <= 201, "Can't have more than 201 particles in an emitter!");

		myGraphicalParticles.Init(particleCount);
		myLogicalParticles.Init(particleCount);

		

		for (int i = 0; i < particleCount; ++i)
		{
			GraphicalParticle tempGraphics;
			myGraphicalParticles.Add(tempGraphics);
			LogicalParticle tempLogic;
			myLogicalParticles.Add(tempLogic);
		}

		Reset();

		CreateVertexBuffer();
	}

	ParticleEmitterInstance::~ParticleEmitterInstance()
	{
		if (myVertexWrapper != nullptr && myVertexWrapper->myVertexBuffer != nullptr)
		{
			myVertexWrapper->myVertexBuffer->Release();
		}

		SAFE_DELETE(myVertexWrapper);
	}

	void ParticleEmitterInstance::ReleaseData()
	{
		myParticleEmitterData = nullptr;
	}

	void ParticleEmitterInstance::Render()
	{
		/*int toGraphicsCard =*/ UpdateVertexBuffer();
		myParticleEmitterData->myEffect->SetTexture(TextureContainer::GetInstance()->GetTexture(myParticleEmitterData->myTextureName));
		//myParticleEmitterData->myEffect->SetWorldMatrix(myOrientation);

		ID3D11DeviceContext* context = Engine::GetInstance()->GetContex();
		context->IASetVertexBuffers(
			myVertexWrapper->myStartSlot
			, myVertexWrapper->myNumberOfBuffers
			, &myVertexWrapper->myVertexBuffer
			, &myVertexWrapper->myStride
			, &myVertexWrapper->myByteOffset);

		for (UINT i = 0; i < myParticleEmitterData->myTechniqueDesc->Passes; ++i)
		{
			myParticleEmitterData->myEffect->GetTechnique()->GetPassByIndex(i)->Apply(0, context);
			context->Draw(myGraphicalParticles.Size(), 0);
		}

	}

	void ParticleEmitterInstance::Update(float aDeltaTime, const CU::Matrix44f& aWorldMatrix)
	{
		UpdateEmitter(aDeltaTime, aWorldMatrix);
	}


	void ParticleEmitterInstance::Reset()
	{

		for (int i = 0; i < myGraphicalParticles.GetCapacity(); ++i)
		{
			myGraphicalParticles[i] = GraphicalParticle();
			myLogicalParticles[i] = LogicalParticle();
		}

		
		myLiveParticleCount = 0;

		myEmissionTime = myParticleEmitterData->myEmissionRate;
		myDiffColor = (myParticleEmitterData->myData.myEndColor - myParticleEmitterData->myData.myStartColor)
			/ myParticleEmitterData->myParticlesLifeTime;

		if (myParticleEmitterData->myUseAlphaDelta == true)
		{
			myStates[ALPHADELTA] = TRUE;
		}
		else
		{
			myStates[ALPHADELTA] = FALSE;
		}

		if (myParticleEmitterData->myIsActiveAtStart == true)
		{
			myStates[ACTIVE] = TRUE;
		}
		else
		{
			myStates[ACTIVE] = FALSE;
		}

		if (myParticleEmitterData->myIsHollow == true)
		{
			myStates[HOLLOW] = TRUE;
		}
		else
		{
			myStates[HOLLOW] = FALSE;
		}

		if (myParticleEmitterData->myIsCircle == true)
		{
			myStates[CIRCLE] = TRUE;
		}
		else
		{
			myStates[CIRCLE] = FALSE;
		}

		if (myParticleEmitterData->myUseEmitterLifeTime == true)
		{
			myStates[EMITTERLIFE] = TRUE;
		}
		else
		{
			myStates[EMITTERLIFE] = FALSE;
		}

		myEmitterLife = myParticleEmitterData->myEmitterLifeTime;
	}

	void ParticleEmitterInstance::CreateVertexBuffer()
	{

		myVertexWrapper = new VertexBufferWrapper();
		myVertexWrapper->myStride = sizeof(GraphicalParticle);
		myVertexWrapper->myByteOffset = 0;
		myVertexWrapper->myStartSlot = 0;
		myVertexWrapper->myNumberOfBuffers = 1;

		HRESULT hr;
		D3D11_BUFFER_DESC vertexBufferDesc;
		ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

		vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		vertexBufferDesc.MiscFlags = 0;
		vertexBufferDesc.StructureByteStride = 0;

		if (myVertexWrapper->myVertexBuffer != nullptr)
		{
			myVertexWrapper->myVertexBuffer->Release();
		}

		vertexBufferDesc.ByteWidth = sizeof(GraphicalParticle) * myGraphicalParticles.Size();

		hr = Engine::GetInstance()->GetDevice()->CreateBuffer(&vertexBufferDesc, nullptr, &myVertexWrapper->myVertexBuffer);
		DL_ASSERT_EXP(hr == S_OK, "[ParticleEmitterInstance](CreateVertexBuffer) : Failed to create VertexBuffer");

		Engine::GetInstance()->SetDebugName(myVertexWrapper->myVertexBuffer
			, "ParticleEmitterInstance::myVertexWrapper->myVertexBuffer");

	}

	int ParticleEmitterInstance::UpdateVertexBuffer()
	{
		/*for (unsigned int i = 0; i < myGraphicalParticles.Size(); ++i)
		{
		if (myGraphicalParticles[i].myAlpha > 0.0f)
		{
		myParticleToGraphicsCard.Add(myGraphicalParticles[i]);
		}
		}

		if (myParticleToGraphicsCard.Size() <= 0)
		{
		return 0;
		}*/

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		Engine::GetInstance()->GetContex()->Map(myVertexWrapper->myVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

		if (mappedResource.pData != nullptr)
		{
			GraphicalParticle *data = (GraphicalParticle*)mappedResource.pData;

			bool isSafe = sizeof(GraphicalParticle) == sizeof(myGraphicalParticles[0]);
			DL_ASSERT_EXP(isSafe, "[ParticleEmitter](UpdateVertexBuffer) : Not safe to copy.");
			memcpy(data, &myGraphicalParticles[0], sizeof(GraphicalParticle)* myGraphicalParticles.Size());
		}
		Engine::GetInstance()->GetContex()->Unmap(myVertexWrapper->myVertexBuffer, 0);


		return myParticleToGraphicsCard.Size();
	}

	void ParticleEmitterInstance::UpdateEmitter(float aDeltaTime, const CU::Matrix44f& aWorldMatrix)
	{
		aWorldMatrix;
		if (myStates[ACTIVE] == TRUE)
		{
			myEmissionTime -= aDeltaTime;
			myEmitterLife -= aDeltaTime;


			if (myEmissionTime <= 0.f && (myEmitterLife > 0.f || myStates[EMITTERLIFE] == FALSE))
			{
				EmitParticle(myOrientation);
				myEmissionTime = myParticleEmitterData->myEmissionRate;
			}

			UpdateParticle(aDeltaTime);

			if (myStates[EMITTERLIFE] == TRUE)
			{
				if (myEmitterLife <= 0.f && myLiveParticleCount <= 0)
				{
					myStates[ACTIVE] = FALSE;
				}
			}
		}
	}

	void ParticleEmitterInstance::UpdateParticle(float aDeltaTime)
	{
		for (int i = 0; i < myLogicalParticles.Size(); ++i)
		{
			myGraphicalParticles[i].myLifeTime -= aDeltaTime;

			myGraphicalParticles[i].myPosition += myLogicalParticles[i].myVelocity * aDeltaTime;

			if (myStates[ALPHADELTA] == TRUE)
			{
				myGraphicalParticles[i].myAlpha += myParticleEmitterData->myData.myAlphaDelta * aDeltaTime;
			}
			else
			{

				myGraphicalParticles[i].myAlpha = myGraphicalParticles[i].myLifeTime / myParticleEmitterData->myParticlesLifeTime;
			}

			if (myGraphicalParticles[i].mySize >= 0.f)
			{
				myGraphicalParticles[i].mySize += myParticleEmitterData->myData.mySizeDelta * aDeltaTime;
			}

			myGraphicalParticles[i].myAlpha = CU::Math::CapValue(0.f, 1.f, myGraphicalParticles[i].myAlpha);

			myGraphicalParticles[i].myColor += myDiffColor  * aDeltaTime;

			myGraphicalParticles[i].myRotation += myGraphicalParticles[i].myRotation * (myLogicalParticles[i].myRotationDelta * aDeltaTime);


			if (myGraphicalParticles[i].myLifeTime < 0.0f && myLogicalParticles[i].myIsAlive == true)
			{
				myLiveParticleCount--;
				myLogicalParticles[i].myIsAlive = false;
				continue;
			}
		}
	}

	void ParticleEmitterInstance::EmitParticle(const CU::Matrix44f& aWorldMatrix)
	{
		for (int i = 0; i < myParticleEmitterData->myParticlesPerEmitt; ++i)
		{
			if (myParticleIndex == myLogicalParticles.Size() - 1)
			{
				myParticleIndex = 0;
			}
			myLiveParticleCount++;

			myGraphicalParticles[myParticleIndex].myColor = myParticleEmitterData->myData.myStartColor;

#pragma	region		Shape
			if (myStates[CIRCLE] == TRUE && myStates[HOLLOW] == TRUE)
			{
				CU::Vector3<float> pos = CreateCirclePositions();
				myGraphicalParticles[myParticleIndex].myPosition = aWorldMatrix.GetPos() + pos;
			}
			else if (myStates[CIRCLE] == TRUE)
			{
				CU::Vector3<float> pos = CreateCirclePositions();
				myGraphicalParticles[myParticleIndex].myPosition = CU::Math::RandomVector(aWorldMatrix.GetPos() - pos
					, aWorldMatrix.GetPos() + pos);
			}
			else if (myStates[HOLLOW] == TRUE)
			{
				CU::Vector3<float> pos = CreateHollowSquare();
				myGraphicalParticles[myParticleIndex].myPosition = aWorldMatrix.GetPos() + pos;
			}
			else
			{
				myGraphicalParticles[myParticleIndex].myPosition =
					CU::Math::RandomVector(aWorldMatrix.GetPos() - myParticleEmitterData->myEmitterSize
					, aWorldMatrix.GetPos() + myParticleEmitterData->myEmitterSize);
			}
#pragma endregion

			myGraphicalParticles[myParticleIndex].myLifeTime = myParticleEmitterData->myParticlesLifeTime;

			myGraphicalParticles[myParticleIndex].myAlpha = myParticleEmitterData->myData.myStartAlpha;

			myParticleScaling = CU::Math::RandomRange(myParticleEmitterData->myData.myMinStartSize
				, myParticleEmitterData->myData.myMaxStartSize);

			myGraphicalParticles[myParticleIndex].mySize = myParticleScaling;

			myLogicalParticles[myParticleIndex].myIsAlive = true;

			myLogicalParticles[myParticleIndex].myVelocity = CU::Math::RandomVector(myParticleEmitterData->myData.myMinVelocity
				, myParticleEmitterData->myData.myMaxVelocity);

			myLogicalParticles[myParticleIndex].myRotation = CU::Math::RandomRange(myParticleEmitterData->myMinRotation
				, myParticleEmitterData->myMaxRotation);

			myGraphicalParticles[myParticleIndex].myRotation = myLogicalParticles[myParticleIndex].myRotation;

			myLogicalParticles[myParticleIndex].mySpeed = myParticleEmitterData->mySpeedMultiplier;

			myLogicalParticles[myParticleIndex].myRotationDelta = myParticleEmitterData->myRotationDelta;

			myParticleIndex += 1;
		}
	}

	CU::Vector3<float> ParticleEmitterInstance::CreateCirclePositions()
	{
		float radius = myParticleEmitterData->myEmitterSize.z;

		if (myParticleEmitterData->myEmitterSize.x > myParticleEmitterData->myEmitterSize.z)
		{
			radius = myParticleEmitterData->myEmitterSize.x;
		}

		CU::Vector3<float> toReturn;

		int a = static_cast<int>((4 * M_PI_2)) * 100;

		float angle = static_cast<float>(rand() % a) / 100.f;

		toReturn.x = radius * cosf(angle);
		toReturn.y = 0.f;
		toReturn.z = radius * sinf(angle);

		return toReturn;
	}

	CU::Vector3<float> ParticleEmitterInstance::CreateSpherePositions()
	{
		float radius = 10.f;
		CU::Vector3<float> toReturn;

		int a = static_cast<int>((4 * M_PI_2)) * 100;

		float angleZ = static_cast<float>(rand() % a) / 100.f;

		//float angle = CU::Math::RandomRange(0.f, );

		toReturn.x = radius * cosf(angleZ);
		toReturn.y = radius * sinf(static_cast<float>(a)) * sinf(angleZ);
		toReturn.z = radius * sinf(angleZ);

		return toReturn;
	}

	CU::Vector3<float> ParticleEmitterInstance::CreateHollowSquare()
	{
		CU::Vector3<float> toReturn;
		CU::Vector3<float> size = myParticleEmitterData->myEmitterSize;
		int randomPoint = rand() % 4 + 1;

		switch (randomPoint)
		{
		case 1: //top
			toReturn.x = CU::Math::RandomRange(-size.x, size.x);
			toReturn.y = 0.f;
			toReturn.z = size.z;
			break;
		case 2: //bot
			toReturn.x = -size.x;
			toReturn.y = 0.f;
			toReturn.z = CU::Math::RandomRange(-size.z, size.z);
			break;
		case 3: //bot
			toReturn.x = CU::Math::RandomRange(-size.x, size.x);
			toReturn.y = 0.f;
			toReturn.z = -size.z;
			break;
		case 4: //top
			toReturn.x = size.x;
			toReturn.y = 0.f;
			toReturn.z = CU::Math::RandomRange(-size.z, size.z);
			break;
		default:
			break;
		}

		return toReturn;
	}

	void ParticleEmitterInstance::SetPosition(const CU::Vector3f& aPosition)
	{
		myOrientation.SetPos(aPosition);
	}

	void ParticleEmitterInstance::SetEmitterLifeTime(float aLifeTime)
	{
		myEmitterLife = aLifeTime;
	}

	void ParticleEmitterInstance::Activate()
	{
		Reset();
		myStates[ACTIVE] = TRUE;
	}

	bool ParticleEmitterInstance::IsActive()
	{
		if (myStates[ACTIVE] == TRUE)
		{
			return true;
		}

		return false;
	}

	void ParticleEmitterInstance::SetEntity(Entity* anEntity)
	{
		myEntity = anEntity;
	}


	Entity* ParticleEmitterInstance::GetEntity()
	{
		return myEntity;
	}


	CU::Vector2<float> ParticleEmitterInstance::GetPosition()
	{
		return CU::Vector2<float>(myOrientation.GetPos().x, myOrientation.GetPos().z);
	}


	bool ParticleEmitterInstance::GetShouldAlwaysShow()
	{
		return myAlwaysShow;
	}

	void ParticleEmitterInstance::SetShouldAlwaysShow(bool aShouldAlwaysShow)
	{
		myAlwaysShow = aShouldAlwaysShow;
	}

	void ParticleEmitterInstance::SetRadius(float aRadius)
	{
		myParticleEmitterData->myEmitterSize.x = aRadius;
		myParticleEmitterData->myEmitterSize.z = aRadius;
		myParticleEmitterData->myEmitterSize.y = 0.f;
	}

	void ParticleEmitterInstance::SetSize(const CU::Vector3f& aSize)
	{
		myParticleEmitterData->myEmitterSize = aSize;
	}


	void ParticleEmitterInstance::KillEmitter(float aKillTime)
	{
		myEmitterLife = aKillTime;
		myStates[EMITTERLIFE] = TRUE;
		if (myEntity != nullptr)
		{
			myEntity = nullptr;
		}
	}


	void ParticleEmitterInstance::SetCamera(Camera* aCamera)
	{
		myCamera = aCamera;
	}


	Camera* ParticleEmitterInstance::GetCamera()
	{
		return myCamera;
	}

}