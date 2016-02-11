#pragma once
#include "Component.h"
#include "EnrageComponentData.h"
class EnrageComponent :
	public Component
{
public:
	EnrageComponent(Entity& anEntity, EnrageComponentData& aData);
	~EnrageComponent();

	void Update(float aDeltaTime);

	static eComponentType GetTypeStatic();
	virtual eComponentType GetType();
	bool IsActive();

	const float& GetCooldown() const;
	const float& GetMaxCooldown() const;

private:
	void Activate();
	void CheckUnitsForRemove(CU::GrowingArray<Entity*>& someUnits) const;
	void CheckUnitsForAdd(const CU::GrowingArray<Entity*>& someUnits, CU::GrowingArray<Entity*>& someUnitsOut) const;
	CU::GrowingArray<Entity*> myUnits;
	float myRadius;
	int myUnitActivationCount;

	float myCurrentDuration;
	float myCurrentCooldown;

	bool myIsActive;

	EnrageComponentData myData;

	float myOriginalArmor;
	//float myOriginalHealth;
	float myOriginalAttackDamage;
	float myOriginalRechargeTime;
	float myOriginalAttackRange2;
	float myOriginalMovementSpeed;

};

inline eComponentType EnrageComponent::GetTypeStatic()
{
	return eComponentType::ENRAGE;
}

inline eComponentType EnrageComponent::GetType()
{
	return GetTypeStatic();
}

inline bool EnrageComponent::IsActive()
{
	return myIsActive;
}

inline const float& EnrageComponent::GetCooldown() const
{
	return myCurrentCooldown;
}

inline const float& EnrageComponent::GetMaxCooldown() const
{
	return myData.myCooldown;
}