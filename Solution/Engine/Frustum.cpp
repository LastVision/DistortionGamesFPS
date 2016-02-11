#include "stdafx.h"
#include "Frustum.h"


Prism::Frustum::Frustum(const CU::Matrix44<float>& anOrientation, float aNear, float aFar)
	: myOrientation(anOrientation)
	, myIntersectionFrustum(aNear, aFar)
{
	myUpLeft = CU::Vector4<float>(-aFar, +aFar, +aFar, 1.f);
	myUpRight = CU::Vector4<float>(+aFar, +aFar, +aFar, 1.f);
	myDownLeft = CU::Vector4<float>(-aFar, -aFar, +aFar, 1.f);
	myDownRight = CU::Vector4<float>(+aFar, -aFar, +aFar, 1.f);

	Update();
}

void Prism::Frustum::Update()
{
	myOrientationInverted = CU::InverseSimple(myOrientation);
	myCornerMin = CalcCornerMin();
	myCornerMax = CalcCornerMax();
}

bool Prism::Frustum::Inside(const CU::Vector3<float>& aPosition, float aRadius) const
{
	CU::Vector4<float> position = CU::Vector4<float>(aPosition.x, aPosition.y, aPosition.z, 1.f) * myOrientationInverted;
	return myIntersectionFrustum.Inside({ position.x, position.y, position.z }, aRadius);
}

void Prism::Frustum::OnResize(float aNearPlane, float aFarPlane)
{
	myIntersectionFrustum.OnResize(aNearPlane, aFarPlane);
	myUpLeft = CU::Vector4<float>(-aFarPlane, +aFarPlane, +aFarPlane, 1.f);
	myUpRight = CU::Vector4<float>(+aFarPlane, +aFarPlane, +aFarPlane, 1.f);
	myDownLeft = CU::Vector4<float>(-aFarPlane, -aFarPlane, +aFarPlane, 1.f);
	myDownRight = CU::Vector4<float>(+aFarPlane, -aFarPlane, +aFarPlane, 1.f);

	Update();
}


CU::Vector3<float> Prism::Frustum::CalcCornerMin() const
{
	CU::Vector4<float> result = myOrientation.GetPos4();

	result.x = fminf(result.x, (myUpLeft * myOrientation).x);
	result.y = fminf(result.y, (myUpLeft * myOrientation).y);
	result.z = fminf(result.z, (myUpLeft * myOrientation).z);

	result.x = fminf(result.x, (myUpRight * myOrientation).x);
	result.y = fminf(result.y, (myUpRight * myOrientation).y);
	result.z = fminf(result.z, (myUpRight * myOrientation).z);

	result.x = fminf(result.x, (myDownLeft * myOrientation).x);
	result.y = fminf(result.y, (myDownLeft * myOrientation).y);
	result.z = fminf(result.z, (myDownLeft * myOrientation).z);

	result.x = fminf(result.x, (myDownRight * myOrientation).x);
	result.y = fminf(result.y, (myDownRight * myOrientation).y);
	result.z = fminf(result.z, (myDownRight * myOrientation).z);

	return CU::Vector3<float>(result.x, result.y, result.z);
}

CU::Vector3<float> Prism::Frustum::CalcCornerMax() const
{
	CU::Vector4<float> result = myOrientation.GetPos4();

	result.x = fmaxf(result.x, (myUpLeft * myOrientation).x);
	result.y = fmaxf(result.y, (myUpLeft * myOrientation).y);
	result.z = fmaxf(result.z, (myUpLeft * myOrientation).z);

	result.x = fmaxf(result.x, (myUpRight * myOrientation).x);
	result.y = fmaxf(result.y, (myUpRight * myOrientation).y);
	result.z = fmaxf(result.z, (myUpRight * myOrientation).z);

	result.x = fmaxf(result.x, (myDownLeft * myOrientation).x);
	result.y = fmaxf(result.y, (myDownLeft * myOrientation).y);
	result.z = fmaxf(result.z, (myDownLeft * myOrientation).z);

	result.x = fmaxf(result.x, (myDownRight * myOrientation).x);
	result.y = fmaxf(result.y, (myDownRight * myOrientation).y);
	result.z = fmaxf(result.z, (myDownRight * myOrientation).z);

	return CU::Vector3<float>(result.x, result.y, result.z);
}