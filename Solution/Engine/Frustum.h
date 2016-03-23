#pragma once
#include <Matrix.h>
#include <Vector.h>
#include <Fov90Frustum.h>

namespace Prism
{
	class Portal;

	class Frustum
	{
	public:
		Frustum(const CU::Matrix44<float>& anOrientation, float aNear, float aFar);

		const CU::Vector3<float>& GetCornerMin() const;
		const CU::Vector3<float>& GetCornerMax() const;

		void Update();
		void Resize(Portal* aPortal, const CU::Matrix44<float>& aCameraOrientation, bool aDebugDraw);

		bool Inside(const CU::Vector3<float>& aPosition, float aRadius) const;
		bool Inside(const CU::Vector3<float>& aPosition, float aRadius, int& aFailPlaneOut, bool& aPointBehind) const;
		bool CheckAABBInside(const CU::Vector3<float>& someMinValues, const CU::Vector3<float>& someMaxValues) const;

		void OnResize(float aNearPlane, float aFarPlane);

		const CU::Matrix44<float>& GetOrientation() const;
		CU::Vector3<float> GetBottomLeft() const;
		CU::Vector3<float> GetTopRight() const;

		void CalcWorldPlanes();

	private:
		void operator=(Frustum&) = delete;

		const CU::Matrix44<float>& myOrientation;
		CU::Matrix44<float> myOrientationInverted;

		CU::Intersection::Fov90Frustum myIntersectionFrustum;

		CU::Vector3<float> myBottomLeft;
		CU::Vector3<float> myTopRight;
	};

	inline const CU::Matrix44<float>& Frustum::GetOrientation() const
	{
		return myOrientation;
	}

	inline CU::Vector3<float> Frustum::GetBottomLeft() const
	{
		return myBottomLeft * myOrientation;
	}

	inline CU::Vector3<float> Frustum::GetTopRight() const
	{
		return myTopRight * myOrientation;
	}
}