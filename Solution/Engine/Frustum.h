#pragma once
#include <Matrix.h>
#include <Vector.h>
#include <Fov90Frustum.h>

namespace Prism
{
	class Frustum
	{
	public:
		Frustum(const CU::Matrix44<float>& anOrientation, float aNear, float aFar);

		const CU::Vector3<float>& GetCornerMin() const;
		const CU::Vector3<float>& GetCornerMax() const;

		void Update();

		bool Inside(const CU::Vector3<float>& aPosition, float aRadius) const;

		void OnResize(float aNearPlane, float aFarPlane);

	private:
		void operator=(Frustum&) = delete;

		CU::Vector3<float> CalcCornerMin() const;
		CU::Vector3<float> CalcCornerMax() const;

		const CU::Matrix44<float>& myOrientation;
		CU::Matrix44<float> myOrientationInverted;

		CU::Intersection::Fov90Frustum myIntersectionFrustum;

		CU::Vector4<float> myUpLeft;
		CU::Vector4<float> myUpRight;
		CU::Vector4<float> myDownLeft;
		CU::Vector4<float> myDownRight;
		CU::Vector3<float> myCornerMin;
		CU::Vector3<float> myCornerMax;
	};


	inline const CU::Vector3<float>& Frustum::GetCornerMin() const
	{
		return myCornerMin;
	}

	inline const CU::Vector3<float>& Frustum::GetCornerMax() const
	{
		return myCornerMax;
	}
}