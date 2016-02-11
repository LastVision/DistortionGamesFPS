#pragma once
#include <Vector.h>

struct Line3DVertex
{
	Line3DVertex(){};
	Line3DVertex(const CU::Vector3<float>& aPoint, const CU::Vector4<float>& aColor)
		: myPoint(aPoint, 1.f)
		, myColor(aColor)
	{
	}
	CU::Vector4<float> myPoint;
	CU::Vector4<float> myColor;
};
struct Line3D
{
	Line3D(){};
	Line3D(const CU::Vector3<float>& aFirstPoint, const CU::Vector3<float>& aSecondPoint
		, const CU::Vector4<float>& aColor, const CU::Vector4<float>& aSecondColor = { -1.f, -1.f, -1.f, -1.f });


	Line3DVertex myFirstPoint;
	Line3DVertex mySecondPoint;

};

