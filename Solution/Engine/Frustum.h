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
		void Resize(Portal* aPortal);

		bool Inside(const CU::Vector3<float>& aPosition, float aRadius) const;

		void OnResize(float aNearPlane, float aFarPlane);

	private:
		void operator=(Frustum&) = delete;

		const CU::Matrix44<float>& myOrientation;
		CU::Matrix44<float> myOrientationInverted;

		CU::Intersection::Fov90Frustum myIntersectionFrustum;
	};
}