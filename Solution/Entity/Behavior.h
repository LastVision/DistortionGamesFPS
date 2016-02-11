#pragma once

class Entity;

class Behavior
{
public:
	virtual ~Behavior();

	virtual const CU::Vector2<float>& Update(float aDelta) = 0;
	virtual void SetTarget(const CU::Vector2<float>& aTargetPosition);
	virtual bool GetDone() const;

protected:
	Behavior(const Entity& anEntity);
	void operator=(Behavior&) = delete;

	CU::Vector2<float> myAcceleration;
	CU::Vector2<float> myTarget;
	const Entity& myEntity;
	bool myDone;
};

inline void Behavior::SetTarget(const CU::Vector2<float>& aTargetPosition)
{
	myTarget = aTargetPosition;
}

inline bool Behavior::GetDone() const
{
	return myDone;
}