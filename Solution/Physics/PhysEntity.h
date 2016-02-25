#pragma once
#include <GameEnum.h>
#include <Vector.h>
#include "PhysEntityData.h"

namespace physx
{
	class PxRigidDynamic;
	class PxRigidStatic;
	class PxTriangleMesh;
}

namespace Prism
{
	class PhysEntity
	{
	public:
		PhysEntity(const PhysEntityData& aPhysData
			, const CU::Matrix44<float>& aOrientation, const std::string& aFBXPath);
		~PhysEntity();

		float* GetPosition();
		float* GetOrientation();

		void UpdateOrientation();

		void AddForce(const CU::Vector3<float>& aDirection, float aMagnitude);
		void SetPosition(const CU::Vector3<float>& aPosition);

	private:
		physx::PxTriangleMesh* GetPhysMesh(const std::string& aFBXPath);


		float myThread4x4Float[16];
		float my4x4Float[16];
		float myPosition[3];
		physx::PxRigidDynamic* myDynamicBody;
		physx::PxRigidStatic* myStaticBody;
		ePhysics myPhysicsType;
	};
}