#pragma once
#include "Component.h"
#include "GameEnum.h"

struct TriggerComponentData;

class TriggerComponent : public Component
{
public:
	TriggerComponent(Entity& anEntity, const TriggerComponentData& someData);
	~TriggerComponent();

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

	eTriggerType GetTriggerType();

private:

	const TriggerComponentData& myData;

	eTriggerType myTriggerType;
};

inline eComponentType TriggerComponent::GetTypeStatic()
{
	return eComponentType::TRIGGER;
}

inline eComponentType TriggerComponent::GetType()
{
	return GetTypeStatic();
}

inline eTriggerType TriggerComponent::GetTriggerType()
{
	return myTriggerType;
}