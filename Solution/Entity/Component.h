#pragma once
#include <GameEnum.h>

class Entity;

struct BehaviorNote;
struct ShootNote;

class Component
{
public:
	Component(Entity& aEntity);
	virtual ~Component();

	//TODO: Make = 0 ?
	virtual void Reset();

	virtual void ReceiveNote(const BehaviorNote&) {}
	virtual void ReceiveNote(const ShootNote&) {}

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