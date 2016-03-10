#pragma once
#include "Component.h"
#include <PhysicsCallbackStruct.h>

struct PhysicsComponentData;

namespace physx
{
	class PxRigidDynamic;
	class PxRigidStatic;
	class PxTriangleMesh;
	class PxShape;
}

class PhysicsComponent : public Component
{
public:
	PhysicsComponent(Entity& aEntity, const PhysicsComponentData& aPhysicsComponentData, const std::string& aFBXPath);
	~PhysicsComponent();

	void SwapOrientations();

	float* GetPosition();
	float* GetOrientation();

	void UpdateOrientation();

	void AddForce(const CU::Vector3<float>& aDirection, float aMagnitude);
	void SetVelocity(const CU::Vector3<float>& aVelocity);
	void SetPosition(const CU::Vector3<float>& aPosition);

	ePhysics GetPhysicsType() const;

	void RemoveFromScene();

	const int GetCapsuleControllerId() const;

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

private:
	const PhysicsComponentData& myData;
	Prism::PhysicsCallbackStruct myCallbackStruct;
	physx::PxTriangleMesh* GetPhysMesh(const std::string& aFBXPath);


	float myThread4x4Float[16];
	float my4x4Float[16];
	float myPosition[3];
	physx::PxRigidDynamic* myDynamicBody;
	physx::PxRigidStatic* myStaticBody;
	physx::PxShape** myShapes;
	ePhysics myPhysicsType;

	int myCapsuleControllerId;
};

inline const int PhysicsComponent::GetCapsuleControllerId() const
{
	return myCapsuleControllerId;
}

inline eComponentType PhysicsComponent::GetTypeStatic()
{
	return eComponentType::PHYSICS;
}

inline eComponentType PhysicsComponent::GetType()
{
	return GetTypeStatic();
}

inline ePhysics PhysicsComponent::GetPhysicsType() const
{
	return myPhysicsType;
}
