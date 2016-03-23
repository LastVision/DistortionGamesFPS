#pragma once
#include "Component.h"
#include <NetworkSubscriber.h>
struct BulletComponentData;

class BulletComponent : public Component, public NetworkSubscriber
{
public:
	BulletComponent(Entity& anEntity, const BulletComponentData& aData, CU::Matrix44<float>& anOrientation);
	~BulletComponent();

	void Activate(const CU::Matrix44<float>& anOrientation);

	void Update(float aDelta) override;

	int GetDamage() const;

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

private:
	const BulletComponentData& myData;
	CU::Matrix44<float>& myOrientation;

	float myLifetimeLeft;
};


inline eComponentType BulletComponent::GetTypeStatic()
{
	return eComponentType::BULLET;
}

inline eComponentType BulletComponent::GetType()
{
	return GetTypeStatic();
}