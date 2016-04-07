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
		, myOverrideDirection(false)
		, myParticleToGraphicsCard(256)
		, myDrawDebugLines(false)
		, myShouldRender(true)
	{
		myStates.reset();
		myParticleEmitterData = someData;
		myEmitterPath = myParticleEmitterData->myFileName;

		int particleCount = static_cast<int>(myParticleEmitterData->myParticlesPerEmitt * myParticleEmitterData->myData.myParticleLifeTime / myParticleEmitterData->myEmissionRate) + 1;


		DL_PRINT(("Loading :" + myEmitterPath).c_str());
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
		myParticleSpeed = myParticleEmitterData->myData.mySpeed;

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
		int toGraphicsCard = UpdateVertexBuffer();
		myParticleEmitterData->myEffect->SetTexture(TextureContainer::GetInstance()->GetTexture(myParticleEmitterData->myTextureName));

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
			context->Draw(toGraphicsCard, 0);
		}

#ifdef _DEBUG
		if (myDrawDebugLines == true)
		{
			Prism::DebugDrawer::GetInstance()->RenderLine3D(myPoints[0], myPoints[1], eColorDebug::RED, eColorDebug::BLUE);
			Prism::DebugDrawer::GetInstance()->RenderLine3D(myPoints[1], myPoints[3], eColorDebug::BLUE, eColorDebug::GREEN);
			Prism::DebugDrawer::GetInstance()->RenderLine3D(myPoints[3], myPoints[2], eColorDebug::GREEN, eColorDebug::YELLOW);
			Prism::DebugDrawer::GetInstance()->RenderLine3D(myPoints[2], myPoints[0], eColorDebug::YELLOW, eColorDebug::RED);

			Prism::DebugDrawer::GetInstance()->RenderLine3D(myPoints[0], myPoints[7], eColorDebug::RED, eColorDebug::BLUE);
			Prism::DebugDrawer::GetInstance()->RenderLine3D(myPoints[1], myPoints[6], eColorDebug::BLUE, eColorDebug::GREEN);
			Prism::DebugDrawer::GetInstance()->RenderLine3D(myPoints[3], myPoints[4], eColorDebug::GREEN, eColorDebug::YELLOW);
			Prism::DebugDrawer::GetInstance()->RenderLine3D(myPoints[2], myPoints[5], eColorDebug::YELLOW, eColorDebug::RED);

			Prism::DebugDrawer::GetInstance()->RenderLine3D(myPoints[4], myPoints[5], eColorDebug::RED, eColorDebug::BLUE);
			Prism::DebugDrawer::GetInstance()->RenderLine3D(myPoints[5], myPoints[7], eColorDebug::BLUE, eColorDebug::GREEN);
			Prism::DebugDrawer::GetInstance()->RenderLine3D(myPoints[7], myPoints[6], eColorDebug::GREEN, eColorDebug::YELLOW);
			Prism::DebugDrawer::GetInstance()->RenderLine3D(myPoints[6], myPoints[4], eColorDebug::YELLOW, eColorDebug::RED);
		}
#endif
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
			/ myParticleEmitterData->myData.myParticleLifeTime;

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
#ifdef _DEBUG
		CreatePoints();
#endif
	}

	CU::Vector3f ParticleEmitterInstance::CalculateDirection(float aYVariation, float aZVariation)
	{
		float radius = myParticleEmitterData->myEmitterSize.z;

		if (myParticleEmitterData->myEmitterSize.x > myParticleEmitterData->myEmitterSize.z)
		{
			radius = myParticleEmitterData->myEmitterSize.x;
		}

		float toRad = aYVariation * 0.5f;
		float toRad2 = aZVariation * 0.5f;


		CU::Vector3<float> toReturn;
		int toRand = static_cast<int>(CU::Math::DegreeToRad(toRad) * 100.f);
		int toRand2 = static_cast<int>(CU::Math::DegreeToRad(toRad2) * 100.f);

		float angle = 0.f;
		float otherAngle = 0.f;

		if (toRand > 0.f)
		{
			angle = static_cast<float>(rand() % toRand) / 100.f;
		}
		if (toRand2 > 0.f)
		{
			otherAngle = static_cast<float>(rand() % toRand2) / 100.f;
		}

		toReturn.x = radius * cosf(angle);
		toReturn.y = radius * sinf(angle);
		toReturn.z = radius * sinf(otherAngle);

		if (aYVariation >= 90.f && aYVariation <= 270.f)
		{
			toReturn.x = CU::Math::RandomRange(-toReturn.x, toReturn.x);
		}
		toReturn.y = CU::Math::RandomRange(-toReturn.y, toReturn.y);

		return toReturn;
	}

	void ParticleEmitterInstance::CreatePoints()
	{
		myPoints[0] = myOrientation.GetPos() - myParticleEmitterData->myEmitterSize;

		myPoints[1] = myOrientation.GetPos() - myParticleEmitterData->myEmitterSize;
		myPoints[1].x = myOrientation.GetPos().x + myParticleEmitterData->myEmitterSize.x;

		myPoints[2] = myOrientation.GetPos() - myParticleEmitterData->myEmitterSize;
		myPoints[2].y = myOrientation.GetPos().y + myParticleEmitterData->myEmitterSize.y;

		myPoints[3] = myOrientation.GetPos() + myParticleEmitterData->myEmitterSize;
		myPoints[3].z = myOrientation.GetPos().z - myParticleEmitterData->myEmitterSize.z;


		myPoints[4] = myOrientation.GetPos() + myParticleEmitterData->myEmitterSize;

		myPoints[5] = myOrientation.GetPos() + myParticleEmitterData->myEmitterSize;
		myPoints[5].x = myOrientation.GetPos().x - myParticleEmitterData->myEmitterSize.x;

		myPoints[6] = myOrientation.GetPos() + myParticleEmitterData->myEmitterSize;
		myPoints[6].y = myOrientation.GetPos().y - myParticleEmitterData->myEmitterSize.y;

		myPoints[7] = myOrientation.GetPos() - myParticleEmitterData->myEmitterSize;
		myPoints[7].z = myOrientation.GetPos().z + myParticleEmitterData->myEmitterSize.z;
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
		myParticleToGraphicsCard.RemoveAll();
		for (int i = 0; i < myGraphicalParticles.Size(); ++i)
		{
			if (myGraphicalParticles[i].myAlpha > 0.0f)
			{
				myParticleToGraphicsCard.Add(myGraphicalParticles[i]);
			}
		}

		if (myParticleToGraphicsCard.Size() <= 0)
		{
			return 0;
		}

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		Engine::GetInstance()->GetContex()->Map(myVertexWrapper->myVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

		if (mappedResource.pData != nullptr)
		{
			GraphicalParticle *data = static_cast<GraphicalParticle*>(mappedResource.pData);

			bool isSafe = sizeof(GraphicalParticle) == sizeof(myParticleToGraphicsCard[0]);
			DL_ASSERT_EXP(isSafe, "[ParticleEmitter](UpdateVertexBuffer) : Not safe to copy.");
			memcpy(data, &myParticleToGraphicsCard[0], sizeof(GraphicalParticle)* myParticleToGraphicsCard.Size());
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

			if (myRotation.x > 0.f)
			{
				myOrientation = CU::Matrix44f::CreateRotateAroundX(CU::Math::DegreeToRad(myRotation.x) * aDeltaTime) * myOrientation;
			}

			if (myRotation.y > 0.f)
			{
				myOrientation = CU::Matrix44f::CreateRotateAroundX(CU::Math::DegreeToRad(myRotation.y) * aDeltaTime) * myOrientation;
			}


			if (myRotation.z > 0.f)
			{
				myOrientation = CU::Matrix44f::CreateRotateAroundX(CU::Math::DegreeToRad(myRotation.z) * aDeltaTime) * myOrientation;
			}


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
					myDrawDebugLines = false;
				}
			}
		}
	}

	void ParticleEmitterInstance::UpdateParticle(float aDeltaTime)
	{
		for (int i = 0; i < myLogicalParticles.Size(); ++i)
		{
			LogicalParticle& logicParticle = myLogicalParticles[i];
			
			if (logicParticle.myIsAlive == false)
			{
				continue;
			}

			GraphicalParticle& gfxParticle = myGraphicalParticles[i];
			ParticleData& particleData = myParticleEmitterData->myData;

			gfxParticle.myLifeTime -= aDeltaTime;
			
			if (particleData.mySpeedDelta > 0.f)
			{
				particleData.mySpeed += particleData.mySpeedDelta * aDeltaTime;
			}

			gfxParticle.myPosition += (logicParticle.myDirection * particleData.mySpeed) * aDeltaTime;

			if (gfxParticle.mySize >= 0.f)
			{
				gfxParticle.mySize += particleData.mySizeDelta * aDeltaTime;
			}

			gfxParticle.myAlpha += particleData.myAlphaDelta * aDeltaTime;

			gfxParticle.myAlpha = CU::Math::CapValue(0.f, 1.f, gfxParticle.myAlpha);

			gfxParticle.myColor += myDiffColor  * aDeltaTime;

			gfxParticle.myRotation += gfxParticle.myRotation * (logicParticle.myRotationDelta * aDeltaTime);


			if (gfxParticle.myLifeTime < 0.0f && logicParticle.myIsAlive == true)
			{
				myLiveParticleCount--;
				logicParticle.myIsAlive = false;
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

			GraphicalParticle &gfxParticle = myGraphicalParticles[myParticleIndex];
			LogicalParticle &logicParticle = myLogicalParticles[myParticleIndex];

			gfxParticle.myColor = myParticleEmitterData->myData.myStartColor;

			logicParticle.mySpeed = myParticleSpeed;

			logicParticle.myDirection = myDirection;
			if (myOverrideDirection == false)
			{
				logicParticle.myDirection = CalculateDirection(myParticleEmitterData->myVariation.x,
					myParticleEmitterData->myVariation.y);
			}

#pragma	region		Shape
			if (myStates[CIRCLE] == TRUE && myStates[HOLLOW] == TRUE)
			{
				CU::Vector3<float> pos = CreateCirclePositions();
				gfxParticle.myPosition = aWorldMatrix.GetPos() + pos;
			}
			else if (myStates[CIRCLE] == TRUE)
			{
				CU::Vector3<float> pos = CreateCirclePositions();
				gfxParticle.myPosition = CU::Math::RandomVector(aWorldMatrix.GetPos() - pos
					, aWorldMatrix.GetPos() + pos);
			}
			else if (myStates[HOLLOW] == TRUE)
			{
				CU::Vector3<float> pos = CreateHollowSquare();
				gfxParticle.myPosition = aWorldMatrix.GetPos() + pos;
			}
			else
			{
				gfxParticle.myPosition =
					CU::Math::RandomVector(aWorldMatrix.GetPos() - myParticleEmitterData->myEmitterSize
					, aWorldMatrix.GetPos() + myParticleEmitterData->myEmitterSize);
			}
#pragma endregion

			gfxParticle.myLifeTime = myParticleEmitterData->myData.myParticleLifeTime;

			gfxParticle.myAlpha = myParticleEmitterData->myData.myStartAlpha;

			myParticleScaling = CU::Math::RandomRange(myParticleEmitterData->myData.myMinStartSize
				, myParticleEmitterData->myData.myMaxStartSize);

			gfxParticle.mySize = myParticleScaling;

			logicParticle.myIsAlive = true;

			logicParticle.myRotation = CU::Math::RandomRange(-360.f, 360.f);

			gfxParticle.myRotation = logicParticle.myRotation;

			logicParticle.myRotationDelta = myParticleEmitterData->myRotationDelta;


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

	void ParticleEmitterInstance::Activate()
	{
		Reset();
		myStates[ACTIVE] = TRUE;
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

	void ParticleEmitterInstance::SetEntity(Entity* anEntity)
	{
		myEntity = anEntity;
	}


}