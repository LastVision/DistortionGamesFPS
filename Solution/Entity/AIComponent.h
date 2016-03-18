#pragma once
#include "Component.h"

class AIComponent : public Component
{
public:
	AIComponent(Entity& anEntity, const AIComponentData& aData);
	~AIComponent();

	void Update(float aDelta) override;

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

private:
	void operator=(AIComponent&) = delete;
	const AIComponentData& myData;

};

inline eComponentType AIComponent::GetTypeStatic()
{
	return eComponentType::AI;
}

inline eComponentType AIComponent::GetType()
{
	return GetTypeStatic();
}