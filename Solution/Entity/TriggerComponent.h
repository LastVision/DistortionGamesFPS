#pragma once
#include "Component.h"

struct TriggerComponentData;

class TriggerComponent : public Component
{
public:
	TriggerComponent(Entity& anEntity, const TriggerComponentData& someData);
	~TriggerComponent();

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

private:

	const TriggerComponentData& myData;
};

inline eComponentType TriggerComponent::GetTypeStatic()
{
	return eComponentType::TRIGGER;
}

inline eComponentType TriggerComponent::GetType()
{
	return GetTypeStatic();
}