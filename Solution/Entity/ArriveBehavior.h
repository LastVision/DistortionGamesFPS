#pragma once
#include "Behavior.h"

class ArriveBehavior : public Behavior
{
public:
	ArriveBehavior(const Entity& anEntity);
	~ArriveBehavior();

	const CU::Vector2<float>& Update(float aDelta) override;
	void SetTarget(const CU::Vector2<float>& aTargetPosition) override;

private:

	float myTargetRadius;
	float mySlowRadius;
	float myTimeToTarget;
	float myMaxAcceleration;
	bool myActive;
};

inline void ArriveBehavior::SetTarget(const CU::Vector2<float>& aTargetPosition)
{
	myActive = true;
	Behavior::SetTarget(aTargetPosition);
}