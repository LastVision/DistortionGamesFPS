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

	void ReceiveNote(const CollisionNote& aNote) override;

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

	eTriggerType GetTriggerType() const;
	int GetValue() const;
	const std::string& GetID() const;

private:

	const TriggerComponentData& myData;

	eTriggerType myTriggerType;

	bool myHasTriggered;
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