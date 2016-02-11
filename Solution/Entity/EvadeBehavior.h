#pragma once
#include "Behavior.h"

class EvadeBehavior : public Behavior
{
public:
	EvadeBehavior(const Entity& anEntity);
	~EvadeBehavior();

	const CU::Vector2<float>& Update(float aDelta) override;

private:
	void EvadeEntities(float aDelta, float aEvadeSpeed);

	float myMaxAcceleration;
	CU::GrowingArray<Entity*> myEntitiesToEvade;

	float myPropSpeed;
	float myPropMod;
};

