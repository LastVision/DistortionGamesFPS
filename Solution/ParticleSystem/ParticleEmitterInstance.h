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
		void SetPosition(const CU::Vector3f& aPosition);
		void SetEmitterLifeTime(float aLifeTime);
		void Activate();
		bool IsActive();
		void SetEntity(Entity* anEntity);
		Entity* GetEntity();
		void SetGPUData(Camera* aCamera);

		CU::Vector2<float> GetPosition();

		bool GetShouldAlwaysShow();
		void SetShouldAlwaysShow(bool aShouldAlwaysShow);

		void SetRadius(float aRadius);
		void SetSize(const CU::Vector3f& aSize);
		void KillEmitter(float aKillTime);
		void SetCamera(Camera* aCamera);
		Camera* GetCamera();
	private:
		void Reset();
		CU::Vector3f CalculateDirection(float aYVariation, float aZVariation);
		void CreateVertexBuffer();
		int UpdateVertexBuffer();

		void UpdateEmitter(float aDeltaTime, const CU::Matrix44f& aWorldMatrix);
		void UpdateParticle(float aDeltaTime);

		void EmitParticle(const CU::Matrix44f& aWorldMatrix);

		CU::Vector3<float> CreateCirclePositions();
		CU::Vector3<float> CreateSpherePositions();

		CU::Vector3<float> CreateHollowSquare();


		CU::GrowingArray<GraphicalParticle> myParticleToGraphicsCard;

		CU::GrowingArray<LogicalParticle> myLogicalParticles;
		CU::GrowingArray<GraphicalParticle> myGraphicalParticles;
		CU::Vector3f myDiffColor;
		CU::Matrix44f myOrientation;

		ParticleEmitterData* myParticleEmitterData;
		VertexBufferWrapper* myVertexWrapper;

		float myEmissionTime;
		float myEmitterLife;
		float myParticleScaling;

		int myParticleIndex;
		int myLiveParticleCount;

		bool myAlwaysShow;
		bool myHasEmitted;

		Camera* myCamera;

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


}
