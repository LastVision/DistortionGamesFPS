#pragma once
#include "Behavior.h"

class ArriveBehavior : public Behavior
{
public:
	ArriveBehavior(const Entity& anEntity, const AIComponentData& aData);
	~ArriveBehavior();

	const CU::Vector3<float>& Update(float aDelta) override;
	void SetTarget(const CU::Vector3<float>& aTargetPosition) override;

private:

	float myTimeToTarget;
	bool myActive;
};

inline void ArriveBehavior::SetTarget(const CU::Vector3<float>& aTargetPosition)
{
	myActive = true;
	Behavior::SetTarget(aTargetPosition);
}