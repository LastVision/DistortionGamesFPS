#pragma once
#include "Behavior.h"

class BlendedBehavior : public Behavior
{
public:
	BlendedBehavior(const Entity& anEntity);
	~BlendedBehavior();

	const CU::Vector2<float>& Update(float aDelta) override;
	void SetTarget(const CU::Vector2<float>& aTargetPosition) override;
	bool GetDone() const override;

private:
	CU::GrowingArray<Behavior*> myBehaviors;
	CU::Vector2<float> myPreviousAcceleration;
	float myMaxAcceleration;
};

