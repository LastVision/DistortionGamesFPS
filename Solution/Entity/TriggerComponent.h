#pragma once
#include "Component.h"
#include "GameEnum.h"

struct TriggerComponentData;

class TriggerComponent : public Component
{
public:
	TriggerComponent(Entity& anEntity, const TriggerComponentData& someData);
	~TriggerComponent();

	void Update(float aDelta) override;

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

	eTriggerType GetTriggerType() const;
	int GetValue() const;
	const std::string& GetID() const;

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

inline eTriggerType TriggerComponent::GetTriggerType() const
{
	return myTriggerType;
}

inline int TriggerComponent::GetValue() const
{
	return myData.myValue;
}

inline const std::string& TriggerComponent::GetID() const
{
	return myData.myID;
}