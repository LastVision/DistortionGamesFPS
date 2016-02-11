#pragma once
#include "Component.h"
#include "GrenadeComponentData.h"
class GrenadeComponent : public Component
{
public:
	GrenadeComponent(Entity& anEntity, GrenadeComponentData& aGrenadeData);
	~GrenadeComponent();
	
	void Update(float aDeltaTime) override;
	void ThrowGrenade(const CU::Vector3f& aTargetPosition);
	static eComponentType GetTypeStatic();
	eComponentType GetType() override;
	const float& GetCooldown() const;
	const float& GetMaxCooldown() const;
private:
	void CheckUnitsForRemove(CU::GrowingArray<Entity*>& someUnits) const;
	void CheckUnitsForAdd(const CU::GrowingArray<Entity*>& someUnits, CU::GrowingArray<Entity*>& someUnitsOut) const;

	void Explosion();


	const float myOriginalDelay;
	const float myOriginalCooldown;
	const float myRange2;
	const float myRadius;
	const float myDamage;

	float myDelay;
	float myCurrentCooldown;
	bool myHasExploded;

	CU::GrowingArray<Entity*> myUnits;

	CU::Vector3f myPosition;
};

inline eComponentType GrenadeComponent::GetTypeStatic()
{
	return eComponentType::GRENADE;
}

inline eComponentType GrenadeComponent::GetType()
{
	return GetTypeStatic();
}