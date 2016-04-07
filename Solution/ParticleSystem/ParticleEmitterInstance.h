#pragma once

#include <GrowingArray.h>
#include "ParticleEmitterData.h"
#include "ParticleData.h"
#include <bitset>

class Entity;

namespace Prism
{
	class Camera;
	struct VertexBufferWrapper;

	class ParticleEmitterInstance
	{
	public:
		ParticleEmitterInstance(ParticleEmitterData* someData, bool anAllowManyParticles = false);
		~ParticleEmitterInstance();
		void ReleaseData();
		void Render();
		void Update(float aDeltaTime, const CU::Matrix44f& aWorldMatrix);
		void Activate();
		bool IsActive();

		void SetPosition(const CU::Vector3f& aPosition);

		void SetEmitterLifeTime(float aLifeTime);
		void KillEmitter(float aKillTime);

		void SetEntity(Entity* anEntity);
		Entity* GetEntity();

		void SetGPUData(Camera* aCamera);

		void SetRadius(float aRadius);
		void SetSize(const CU::Vector3f& aSize);

		void SetDirection(const CU::Vector3<float>& aDirection);
		void ToggleDebugLines();

		bool GetShouldRender();
		void SetShouldRender(bool aShouldRender);

		void SetRotation(const CU::Vector3<float>& aRotation);
	private:

		void Reset();
		void CreatePoints();
		void CreateVertexBuffer();

		void UpdateEmitter(float aDeltaTime, const CU::Matrix44f& aWorldMatrix);
		void UpdateParticle(float aDeltaTime);

		void EmitParticle(const CU::Matrix44f& aWorldMatrix);
		void Reflect(CU::Vector3<float>&  aOutputVector, const CU::Vector3<float>& aIncidentVector, const CU::Vector3<float>& aNormal);

		int UpdateVertexBuffer();

		CU::Vector3<float> CalculateDirection(float aYVariation, float aZVariation);
		CU::Vector3<float> CreateCirclePositions();
		CU::Vector3<float> CreateSpherePositions();
		CU::Vector3<float> CreateHollowSquare();

		CU::Matrix44f myOrientation;

		CU::GrowingArray<GraphicalParticle> myParticleToGraphicsCard;

		CU::GrowingArray<LogicalParticle> myLogicalParticles;
		CU::GrowingArray<GraphicalParticle> myGraphicalParticles;

		CU::Vector3<float> myDiffColor;
		CU::Vector3<float> myDirection;
		CU::Vector3<float> myRotation;
		CU::Vector3<float> myPoints[8];
		
		ParticleEmitterData* myParticleEmitterData;
		VertexBufferWrapper* myVertexWrapper;

		float myEmissionTime;
		float myEmitterLife;
		float myParticleScaling;
		float myParticleSpeed;

		int myParticleIndex;
		int myLiveParticleCount;

		bool myOverrideDirection;
		bool myShouldRender;
		bool myDrawDebugLines;




		Entity* myEntity;

		enum eEmitterStates
		{
			HOLLOW,
			ACTIVE,
			CIRCLE,
			EMITTERLIFE,
			_COUNT
		};

		std::bitset<_COUNT> myStates;
		std::string myEmitterPath;
	};

	inline Entity* ParticleEmitterInstance::GetEntity()
	{
		return myEntity;
	}

	inline void ParticleEmitterInstance::SetShouldRender(bool aShouldRender)
	{
		myShouldRender = aShouldRender;
	}

	inline void ParticleEmitterInstance::ToggleDebugLines()
	{
		myDrawDebugLines = !myDrawDebugLines;
	}

	inline bool ParticleEmitterInstance::GetShouldRender()
	{
		return myShouldRender;
	}

	inline void ParticleEmitterInstance::SetDirection(const CU::Vector3<float>& aDirection)
	{
		myOverrideDirection = true;
		myDirection = aDirection;
	}

	inline void ParticleEmitterInstance::SetSize(const CU::Vector3f& aSize)
	{
		myParticleEmitterData->myEmitterSize = aSize * 0.5f;
#ifdef _DEBUG
		CreatePoints();
#endif
	}

	inline bool ParticleEmitterInstance::IsActive()
	{
		return myStates[ACTIVE];
	}


	inline void ParticleEmitterInstance::SetRadius(float aRadius)
	{
		myParticleEmitterData->myEmitterSize.x = aRadius;
		myParticleEmitterData->myEmitterSize.z = aRadius;
		myParticleEmitterData->myEmitterSize.y = 0.f;
	}

	inline void ParticleEmitterInstance::SetPosition(const CU::Vector3f& aPosition)
	{
		myOrientation.SetPos(aPosition);
	}

	inline void ParticleEmitterInstance::SetEmitterLifeTime(float aLifeTime)
	{
		myEmitterLife = aLifeTime;
	}

	inline void ParticleEmitterInstance::SetRotation(const CU::Vector3<float>& aRotation)
	{
		myRotation = aRotation;
	}

}
