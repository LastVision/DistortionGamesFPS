#pragma once
#include "Component.h"

struct UpgradeComponentData;

class UpgradeComponent : public Component
{
public:
	UpgradeComponent(Entity& aEntity, const UpgradeComponentData& aData);
	~UpgradeComponent();

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

	const UpgradeComponentData& GetData() const;

private:
	const UpgradeComponentData& myData;

};

inline eComponentType UpgradeComponent::GetTypeStatic()
{
	return eComponentType::UPGRADE;
}

inline eComponentType UpgradeComponent::GetType()
{
	return GetTypeStatic();
}

inline const UpgradeComponentData& UpgradeComponent::GetData() const
{
	return myData;
}