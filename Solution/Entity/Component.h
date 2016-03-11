#pragma once
#include <GameEnum.h>

class Entity;

struct BehaviorNote;
struct ShootNote;
struct DamageNote;
struct HealthNote;
struct CollisionNote;
struct UpgradeNote;

class Component
{
public:
	Component(Entity& aEntity);
	virtual ~Component();

	//TODO: Make = 0 ?
	virtual void Reset();

	virtual void ReceiveNote(const BehaviorNote&) {}
	virtual void ReceiveNote(const ShootNote&) {}
	virtual void ReceiveNote(const DamageNote&) {}
	virtual void ReceiveNote(const HealthNote&) {}
	virtual void ReceiveNote(const CollisionNote&) {}
	virtual void ReceiveNote(const UpgradeNote&) {}

	//TODO: remove?
	virtual void Init();

	virtual void Update(float aDeltaTime);

	Entity& GetEntity();

	static eComponentType GetTypeStatic();
	virtual eComponentType GetType();


protected:
	void operator=(Component&) = delete;

	Entity& myEntity;
};

inline eComponentType Component::GetTypeStatic()
{
	return eComponentType::NOT_USED;
}

inline eComponentType Component::GetType()
{
	return GetTypeStatic();
}


inline Entity& Component::GetEntity()
{
	return myEntity;
}