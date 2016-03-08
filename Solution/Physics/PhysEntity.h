#pragma once
#include <GameEnum.h>
#include <Vector.h>
#include "PhysEntityData.h"
#include <Matrix.h>

namespace physx
{
	class PxRigidDynamic;
	class PxRigidStatic;
	class PxTriangleMesh;
	class PxShape;
}

class Entity;

namespace Prism
{

	class PhysEntity
	{
	public:
		PhysEntity(const PhysEntityData& aPhysData
			, const CU::Matrix44<float>& aOrientation, const std::string& aFBXPath,
			Entity* aEntity);
		~PhysEntity();

		void SwapOrientations();

		float* GetPosition();
		float* GetOrientation();
		Entity* GetEntity();

		void UpdateOrientation();

		void AddForce(const CU::Vector3<float>& aDirection, float aMagnitude);
		void SetVelocity(const CU::Vector3<float>& aVelocity);

		ePhysics GetPhysicsType() const;

		void RemoveFromScene();

	private:
		physx::PxTriangleMesh* GetPhysMesh(const std::string& aFBXPath);


		float myThread4x4Float[16];
		float my4x4Float[16];
		float myPosition[3];
		physx::PxRigidDynamic* myDynamicBody;
		physx::PxRigidStatic* myStaticBody;
		physx::PxShape** myShapes;
		ePhysics myPhysicsType;

		Entity* myEntity;
	};

	inline Entity* PhysEntity::GetEntity()
	{
		return myEntity;
	}

	inline ePhysics PhysEntity::GetPhysicsType() const
	{
		return myPhysicsType;
	}
}