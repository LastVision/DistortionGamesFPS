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
			bool Inside(const Vector3<float>& aPos, float aRadius, int& aFailPlaneOut) const;
			bool CheckAABBInside(const CU::Vector3<float>& someMinValues, const CU::Vector3<float>& someMaxValues) const;
			void Resize(const CU::Vector3<float>& aBottomLeft, const CU::Vector3<float>& aTopRight);
			void ResizeLeft(const CU::Vector3<float>& aLeft);
			void ResizeRight(const CU::Vector3<float>& aRight);


			void OnResize(float aNearPlane, float aFarPlane);

			void CalcWorldPlanes(const CU::Matrix44<float>& anOrientation);

			CU::PlaneVolume<float> myVolume;
			CU::PlaneVolume<float> myWorldVolume;
			float myNear;
			float myFar;
		};
	}

	inline Intersection::Fov90Frustum::Fov90Frustum(float, float aFar)
		: myVolume(CU::GrowingArray<CU::Plane<float>>())
		, myWorldVolume(CU::GrowingArray<CU::Plane<float>>())
	{
		myNear = 0;
		myFar = aFar;
		float rotated45 = sqrt(2.f) / 2.f;

		CU::Plane<float> nearPlane(CU::Vector3<float>(0, 0, myNear)
			, CU::Vector3<float>(0, 0, -1));
		CU::Plane<float> farPlane(CU::Vector3<float>(0, 0, aFar)
			, CU::Vector3<float>(0, 0, 1));

		CU::Plane<float> right(CU::Vector3<float>(0, 0, 0), CU::Vector3<float>(rotated45, 0, -rotated45));
		CU::Plane<float> left(CU::Vector3<float>(0, 0, 0), CU::Vector3<float>(-rotated45, 0, -rotated45));
		CU::Plane<float> up(CU::Vector3<float>(0, 0, 0), CU::Vector3<float>(0, rotated45, -rotated45));
		CU::Plane<float> down(CU::Vector3<float>(0, 0, 0), CU::Vector3<float>(0, -rotated45, -rotated45));

		myVolume.myPlanes.Init(6);
		myWorldVolume.myPlanes.Init(6);

		myVolume.AddPlane(right);
		myVolume.AddPlane(left);
		myVolume.AddPlane(up);
		myVolume.AddPlane(down);
		myVolume.AddPlane(nearPlane);
		myVolume.AddPlane(farPlane);
	}

	inline bool CU::Intersection::Fov90Frustum::Inside(const Vector3<float>& aPos, float aRadius) const
	{
		if (myVolume.Inside(aPos, aRadius) == true)
		{
			return true;
		}
		return false;
	}

	inline bool CU::Intersection::Fov90Frustum::Inside(const Vector3<float>& aPos, float aRadius, int& aFailPlaneOut) const
	{
		if (myVolume.Inside(aPos, aRadius, aFailPlaneOut) == true)
		{
			return true;
		}
		return false;
	}

	inline bool CU::Intersection::Fov90Frustum::CheckAABBInside(const CU::Vector3<float>& someMinValues, const CU::Vector3<float>& someMaxValues) const
	{
		return myWorldVolume.CheckAABBInside(someMinValues, someMaxValues);
	}

	//inline bool CU::Intersection::Fov90Frustum::AABBInside(const Vector3<float>& someMinValues, const Vector3<float>& someMaxValues) const
	//{
	//	Vector3<float> minValue;
	//	Vector3<float> maxValue;
	//	Vector3<float> normal;

	//	bool intersection = false;

	//	for (int i = 0; i < 6; i++)
	//	{
	//		maxValue = someMaxValues;
	//		minValue = someMinValues;

	//		normal = myVolume.myPlanes[i].GetNormal();

	//		if (normal.x < 0)
	//		{
	//			minValue.x = someMaxValues.x;
	//			maxValue.x = someMinValues.x;
	//		}

	//		if (normal.y < 0)
	//		{
	//			minValue.y = someMaxValues.y;
	//			maxValue.y = someMinValues.y;
	//		}

	//		if (normal.z < 0)
	//		{
	//			minValue.z = someMaxValues.z;
	//			maxValue.z = someMinValues.z;
	//		}

	//		if (myVolume.myPlanes[i].Inside(minValue) == false)
	//		{
	//			return false;
	//		}
	//		else if (myVolume.myPlanes[i].Inside(maxValue) == true)
	//		{
	//			intersection = true;
	//		}
	//	}

	//	return true;
	//}

	inline void CU::Intersection::Fov90Frustum::Resize(const CU::Vector3<float>& aBottomLeft, const CU::Vector3<float>& aTopRight)
	{
		CU::Vector3<float> zero;
		CU::Vector3<float> x(1.f, 0, 0);
		CU::Vector3<float> y(0, 1.f, 0);
		CU::Vector3<float> z(0, 0, 1.f);
		myVolume.myPlanes[0].InitWith3Points(zero, aTopRight, y);
		myVolume.myPlanes[1].InitWith3Points(zero, aBottomLeft, -y);
	}

	inline void CU::Intersection::Fov90Frustum::ResizeLeft(const CU::Vector3<float>& aLeft)
	{

		CU::Vector3<float> zero;
		CU::Vector3<float> x(1.f, 0, 0);
		CU::Vector3<float> y(0, 1.f, 0);
		CU::Vector3<float> z(0, 0, 1.f);
		CU::Plane<float> newPlane;
		newPlane.InitWith3Points(zero, aLeft, -y);

		if (newPlane.myABCD.z > myVolume.myPlanes[1].myABCD.z)
		{
			myVolume.myPlanes[1] = newPlane;
		}
		//myVolume.myPlanes[1].InitWith3Points(zero, aLeft, -y);
	}

	inline void CU::Intersection::Fov90Frustum::ResizeRight(const CU::Vector3<float>& aRight)
	{

		CU::Vector3<float> zero;
		CU::Vector3<float> x(1.f, 0, 0);
		CU::Vector3<float> y(0, 1.f, 0);
		CU::Vector3<float> z(0, 0, 1.f);

		CU::Plane<float> newPlane;
		newPlane.InitWith3Points(zero, aRight, y);

		if (newPlane.myABCD.z > myVolume.myPlanes[0].myABCD.z)
		{
			myVolume.myPlanes[0] = newPlane;
		}
		//myVolume.myPlanes[0].InitWith3Points(zero, aRight, y);
	}

	inline void CU::Intersection::Fov90Frustum::OnResize(float, float aFarPlane)
	{
		myVolume.myPlanes.RemoveAll();
		myNear = 0;
		myFar = aFarPlane;
		float rotated45 = sqrt(2.f) / 2.f;

		CU::Plane<float> nearPlane(CU::Vector3<float>(0, 0, myNear)
			, CU::Vector3<float>(0, 0, -1));
		CU::Plane<float> farPlane(CU::Vector3<float>(0, 0, aFarPlane)
			, CU::Vector3<float>(0, 0, 1));

		CU::Plane<float> right(CU::Vector3<float>(0, 0, 0), CU::Vector3<float>(rotated45, 0, -rotated45));
		CU::Plane<float> left(CU::Vector3<float>(0, 0, 0), CU::Vector3<float>(-rotated45, 0, -rotated45));
		CU::Plane<float> up(CU::Vector3<float>(0, 0, 0), CU::Vector3<float>(0, rotated45, -rotated45));
		CU::Plane<float> down(CU::Vector3<float>(0, 0, 0), CU::Vector3<float>(0, -rotated45, -rotated45));
		myVolume.AddPlane(right);
		myVolume.AddPlane(left);
		myVolume.AddPlane(up);
		myVolume.AddPlane(down);
		myVolume.AddPlane(nearPlane);
		myVolume.AddPlane(farPlane);
	}

	inline void CU::Intersection::Fov90Frustum::CalcWorldPlanes(const CU::Matrix44<float>& anOrientation)
	{
		myWorldVolume.CalcWorldPlanes(myVolume, anOrientation);
	}
}