#include "stdafx.h"
#include "Line3D.h"


Line3D::Line3D(const CU::Vector3<float>& aFirstPoint, const CU::Vector3<float>& aSecondPoint
	, const CU::Vector4<float>& aColor, const CU::Vector4<float>& aSecondColor)
	: myFirstPoint(aFirstPoint, aColor)
	, mySecondPoint(aSecondPoint, aSecondColor.x == -1.f ? aColor : aSecondColor)
{
}