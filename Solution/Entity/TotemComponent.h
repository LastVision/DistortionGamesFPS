#pragma once
#include "Component.h"
class TotemComponent :
	public Component
{
public:
	TotemComponent(Entity& aEntity, TotemComponentData& aData);
	~TotemComponent();

	void Update(float aDelta) override;

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;
	eOwnerType ModifyOwnership(eOwnerType anOwner, float aModifyValue);
	void SetTargetPosition(const CU::Vector3f& aTargetPosition);
	const float& GetCurrentCooldown() const;
	const float& GetMaxCooldown() const;

	bool CanActivate() const;
	bool GetIsInside(Entity* aEntity);

private:
	void operator=(TotemComponent&) = delete;

	void CheckUnitsForRemove(CU::GrowingArray<Entity*>& someUnits) const;
	void CheckUnitsForAdd(const CU::GrowingArray<Entity*>& someUnits, CU::GrowingArray<Entity*>& someUnitsOut) const;

	const float myRadius;
	const float myRadiusSquared;
	const float myHealPerSecond;
	const float myOriginalCooldown;
	
	const float myEndTime;
	float myDuration;
	float myCurrentCooldown;


	float myAlpha;
	bool myHasReachedTarget;
	bool myHasJustLanded;

	bool myActive;
	bool myEffectActive;

	CU::Vector3f myOriginalPosition;
	CU::Vector3f myTargetPosition;

	CU::GrowingArray<Entity*> myUnits;
	bool myDisapearing;
	CU::Vector3<float> myDisapearVector;

	bool myPlaced;
};

inline eComponentType TotemComponent::GetTypeStatic()
{
	return eComponentType::TOTEM;
}

inline eComponentType TotemComponent::GetType()
{
	return GetTypeStatic();
}

inline const float& TotemComponent::GetCurrentCooldown() const
{
	return myCurrentCooldown;
}

inline const float& TotemComponent::GetMaxCooldown() const
{
	return myOriginalCooldown;
}

inline bool TotemComponent::CanActivate() const
{
	return myCurrentCooldown <= 0.f;
}