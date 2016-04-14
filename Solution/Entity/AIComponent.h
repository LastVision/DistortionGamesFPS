#pragma once
#include "Component.h"

class Behavior;
class PhysicsComponent;

class AIComponent : public Component
{
public:
	AIComponent(Entity& anEntity, const AIComponentData& aData, CU::Matrix44<float>& anOrientation, const BulletComponentData& aBulletData);
	~AIComponent();

	void Reset() override;

	void Update(float aDelta) override;

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

	void HandleRaycast(PhysicsComponent* aComponent, const CU::Vector3<float>& aDirection, const CU::Vector3<float>& aHitPosition, const CU::Vector3<float>& aHitNormal);

	void SetDefendTarget(Entity* aEntityToTarget);
	void ResetDefendTarget();

private:
	void operator=(AIComponent&) = delete;
	std::function<void(PhysicsComponent*, const CU::Vector3<float>&, const CU::Vector3<float>&, const CU::Vector3<float>&)> myRaycastHandler;

	void Move(float aDelta, Entity* aClosestPlayer);
	void SetOrientation(const CU::Vector3<float>& aLookInDirection, float aDelta, bool aOnlyRotation = false);

	void Shoot(Entity* aClosestPlayer);

	const AIComponentData& myData;
	const BulletComponentData& myBulletData;

	Behavior* myBehavior;

	CU::Matrix44<float>& myOrientation;

	float myShootTimer;

	CU::GrowingArray<Entity*> myBullets;
	int myBulletIndex;
	bool myHasRaycasted;

	float myAttackAnimationTimeCurrent;

	//Entity* myTarget;
	Entity* myDefendTarget;
	Entity* myTargetPlayer;

	CU::Vector3<float> myPreviousTargetPosition;

	bool myHasJustSpawned;
};

inline eComponentType AIComponent::GetTypeStatic()
{
	return eComponentType::AI;
}

inline eComponentType AIComponent::GetType()
{
	return GetTypeStatic();
}

inline void AIComponent::SetDefendTarget(Entity* aEntityToTarget)
{
	myDefendTarget = aEntityToTarget;
}

inline void AIComponent::ResetDefendTarget()
{
	myDefendTarget = nullptr;
}