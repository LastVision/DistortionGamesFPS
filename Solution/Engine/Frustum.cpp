#include "stdafx.h"
#include "Frustum.h"
#include "Portal.h"

Prism::Frustum::Frustum(const CU::Matrix44<float>& anOrientation, float aNear, float aFar)
	: myOrientation(anOrientation)
	, myIntersectionFrustum(aNear, aFar)
{
	Update();
}

void Prism::Frustum::Update()
{
	myOrientationInverted = CU::InverseSimple(myOrientation);
}

void Prism::Frustum::Resize(Portal* aPortal)
{
	CU::Vector4<float> bottomLeft(CU::Vector4<float>(aPortal->GetBottomLeft(), 1.f) * myOrientationInverted);
	CU::Vector4<float> topRight(CU::Vector4<float>(aPortal->GetTopRight(), 1.f) * myOrientationInverted);

	if (bottomLeft.x > topRight.x)
	{
		float temp = bottomLeft.x;
		bottomLeft.x = topRight.x;
		topRight.x = temp;
	}

	if (bottomLeft.z > 0 && topRight.z > 0)
	{
		CU::Vector3<float> bottomLeft3(bottomLeft.x, bottomLeft.y, bottomLeft.z);
		CU::Vector3<float> topRight3(topRight.x, topRight.y, topRight.z);

		myIntersectionFrustum.Resize(bottomLeft3, topRight3);
	}
}

bool Prism::Frustum::Inside(const CU::Vector3<float>& aPosition, float aRadius) const
{
	CU::Vector4<float> position = CU::Vector4<float>(aPosition.x, aPosition.y, aPosition.z, 1.f) * myOrientationInverted;
	return myIntersectionFrustum.Inside({ position.x, position.y, position.z }, aRadius);
}

void Prism::Frustum::OnResize(float aNearPlane, float aFarPlane)
{
	myIntersectionFrustum.OnResize(aNearPlane, aFarPlane);
	Update();
}