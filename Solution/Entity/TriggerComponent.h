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

	void Activate();

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

	eTriggerType GetTriggerType() const;
	int GetValue() const;
	bool IsClientSide() const;
	bool GetIsActiveFromStart() const;
	void SetRespawnValue(int aValue);
	int GetRespawnValue() const;

private:

	const TriggerComponentData& myData;
	eTriggerType myTriggerType;

	bool myHasTriggered;
	int myPlayersInside;
	
	int myRespawnValue;
	float myRespawnTime;
	int myLastRespawnValue;
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

inline void TriggerComponent::SetRespawnValue(int aValue)
{
	myRespawnValue = aValue;
}

inline int TriggerComponent::GetRespawnValue() const
{
	return myRespawnValue;
}
