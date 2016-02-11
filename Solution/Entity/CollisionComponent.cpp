#include "stdafx.h"
#include "CollisionComponent.h"
#include <Intersection.h>

CollisionComponent::CollisionComponent(Entity& aEntity, CollisionComponentData& aData)
	: Component(aEntity)
{
	mySphere.myRadius = (aData.myDiameter / 2.f);
	mySphere.myRadiusSquared = mySphere.myRadius * mySphere.myRadius;
	mySphere.myCenterPosition = myEntity.GetOrientation().GetPos();
}


CollisionComponent::~CollisionComponent()
{
}

void CollisionComponent::Update(float)
{
	mySphere.myCenterPosition = myEntity.GetOrientation().GetPos();
}

bool CollisionComponent::Collide(const CU::Intersection::LineSegment3D& aLine) const
{
	CU::Vector3<float> notUsed;
	return CU::Intersection::LineVsSphere(aLine, mySphere, notUsed);
}

bool CollisionComponent::Collide(const CU::Vector3<float>& aPosition) const
{
	return CU::Intersection::PointInsideSphere(mySphere, aPosition);
}

bool CollisionComponent::Collide(const CU::Vector2<float>& aFirstPosition, const CU::Vector2<float>& aSecondPosition) const
{
	CU::Vector2<float> position(mySphere.myCenterPosition.x, mySphere.myCenterPosition.z);

	return CU::Intersection::CircleVsRect(position, mySphere.myRadius, aFirstPosition, aSecondPosition);
}