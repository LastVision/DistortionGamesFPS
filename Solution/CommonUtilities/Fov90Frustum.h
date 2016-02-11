#pragma once
#include "PlaneVolume.h"
#include "Matrix.h"

namespace CU
{
	namespace Intersection
	{
		class Fov90Frustum
		{
		public:
			Fov90Frustum(float aNear, float aFar);
			bool Inside(const Vector3<float>& aPos, float aRadius) const;

			void OnResize(float aNearPlane, float aFarPlane);

			CU::PlaneVolume<float> myVolume;
			float myNear;
			float myFar;
		};
	}

	inline Intersection::Fov90Frustum::Fov90Frustum(float aNear, float aFar) :
		myVolume(CU::GrowingArray<CU::Plane<float>>())
	{
		myNear = aNear;
		myFar = aFar;
		float rotated45 = sqrt(2.f) / 2.f;

		CU::Plane<float> nearPlane(CU::Vector3<float>(0, 0, aNear)
			, CU::Vector3<float>(0, 0, -1));
		CU::Plane<float> farPlane(CU::Vector3<float>(0, 0, aFar)
			, CU::Vector3<float>(0, 0, 1));

		CU::Plane<float> right(CU::Vector3<float>(0, 0, 0), CU::Vector3<float>(rotated45, 0, -rotated45));
		CU::Plane<float> left(CU::Vector3<float>(0, 0, 0), CU::Vector3<float>(-rotated45, 0, -rotated45));
		CU::Plane<float> up(CU::Vector3<float>(0, 0, 0), CU::Vector3<float>(0, rotated45, -rotated45));
		CU::Plane<float> down(CU::Vector3<float>(0, 0, 0), CU::Vector3<float>(0, -rotated45, -rotated45));

		myVolume.myPlanes.Init(6);
		myVolume.AddPlane(nearPlane);
		myVolume.AddPlane(farPlane);
		myVolume.AddPlane(right);
		myVolume.AddPlane(left);
		myVolume.AddPlane(up);
		myVolume.AddPlane(down);
	}

	inline bool CU::Intersection::Fov90Frustum::Inside(const Vector3<float>& aPos, float aRadius) const
	{
		if (myVolume.Inside(aPos, aRadius) == true)
		{
			return true;
		}
		return false;
	}

	inline void CU::Intersection::Fov90Frustum::OnResize(float aNearPlane, float aFarPlane)
	{
		myVolume.myPlanes.RemoveAll();
		myNear = aNearPlane;
		myFar = aFarPlane;
		float rotated45 = sqrt(2.f) / 2.f;

		CU::Plane<float> nearPlane(CU::Vector3<float>(0, 0, aNearPlane)
			, CU::Vector3<float>(0, 0, -1));
		CU::Plane<float> farPlane(CU::Vector3<float>(0, 0, aFarPlane)
			, CU::Vector3<float>(0, 0, 1));

		CU::Plane<float> right(CU::Vector3<float>(0, 0, 0), CU::Vector3<float>(rotated45, 0, -rotated45));
		CU::Plane<float> left(CU::Vector3<float>(0, 0, 0), CU::Vector3<float>(-rotated45, 0, -rotated45));
		CU::Plane<float> up(CU::Vector3<float>(0, 0, 0), CU::Vector3<float>(0, rotated45, -rotated45));
		CU::Plane<float> down(CU::Vector3<float>(0, 0, 0), CU::Vector3<float>(0, -rotated45, -rotated45));

		myVolume.AddPlane(nearPlane);
		myVolume.AddPlane(farPlane);
		myVolume.AddPlane(right);
		myVolume.AddPlane(left);
		myVolume.AddPlane(up);
		myVolume.AddPlane(down);
	}
}