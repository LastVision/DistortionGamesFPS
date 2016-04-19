#pragma once
#include <Matrix.h>
#include "Component.h"
#include <NetworkSubscriber.h>
struct BulletComponentData;

class BulletComponent : public Component, public NetworkSubscriber
{
public:
	BulletComponent(Entity& anEntity, CU::Matrix44<float>& anOrientation);
	~BulletComponent();

	void Activate(const CU::Matrix44<float>& anOrientation);

	void Update(float aDelta) override;

	int GetDamage() const;

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

	void SetData(const BulletComponentData& aData);

private:
	const BulletComponentData* myData;
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

inline void BulletComponent::SetData(const BulletComponentData& aData)
{
	myData = &aData;
}