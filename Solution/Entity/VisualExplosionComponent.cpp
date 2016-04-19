#include "stdafx.h"
#include "Entity.h"
#include "VisualExplosionComponent.h"
#include "VisualExplosionComponentData.h"
#include <MathHelper.h>
#include "GraphicsComponent.h"

VisualExplosionComponent::VisualExplosionComponent(Entity& anEntity, const VisualExplosionComponentData& someData, CU::Matrix44<float>& anOrientation)
	: Component(anEntity)
	, myData(someData)
	, myOrientation(anOrientation)
	, myTotalTime(100.f)
{
	DL_ASSERT_EXP(myEntity.GetIsClient() == true, "Can't visual explode on server.");
}

VisualExplosionComponent::~VisualExplosionComponent()
{
}

void VisualExplosionComponent::Update(float aDeltaTime)
{
	myTotalTime += aDeltaTime;
	float toSin = myTotalTime * 8.f;

	toSin = fminf(3.15f, toSin);
	float scale = fmaxf(0, sinf(toSin) * 5.f);

	myEntity.GetComponent<GraphicsComponent>()->SetScale(CU::Vector3<float>(scale, scale, scale));
}

void VisualExplosionComponent::SetPosition(const CU::Vector3<float>& aPosition)
{
	myTotalTime = 0;
	myOrientation.SetPos(aPosition);
}