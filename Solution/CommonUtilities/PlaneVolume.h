#pragma once
#include "GrowingArray.h"
#include "Plane.h"
#include <vector>

namespace CU
{
	template <typename T>
	class PlaneVolume
	{
	public:
		PlaneVolume(GrowingArray<Plane<T>> aPlaneList);
		PlaneVolume(std::vector<Plane<T>> aPlaneList);
		~PlaneVolume();
		void AddPlane(Plane<T > aPlane);
		void RemovePlane(Plane<T> aPlane);
		bool Inside(Vector3<T> aPosition) const;
		bool Inside(Vector3<T> aPosition, T aRadius) const;
		bool Inside(Vector3<T> aPosition, T aRadius, int& aFailPlaneOut) const;
		bool CheckAABBInside(const Vector3<T>& someMinValues, const Vector3<T>& someMaxValues) const;

		void CalcWorldPlanes(PlaneVolume& aCopyFromVolume, const CU::Matrix44<float>& anOrientation);

		GrowingArray<Plane<T>> myPlanes;
	};
}

namespace CU
{
	template <typename T>
	PlaneVolume<T>::PlaneVolume(GrowingArray<Plane<T>> aPlaneList)
	{
		myPlanes = aPlaneList;
	}

	template <typename T>
	PlaneVolume<T>::PlaneVolume(std::vector<Plane<T>> aPlaneList)
	{
		myPlanes.Init(aPlaneList.size() + 1);
		for (unsigned int i = 0; i < aPlaneList.size(); ++i)
		{
			myPlanes.Add(aPlaneList[i]);
		}
	}

	template <typename T>
	PlaneVolume<T>::~PlaneVolume()
	{
	}

	template <typename T>
	void PlaneVolume<T>::AddPlane(Plane<T> aPlane)
	{
		myPlanes.Add(aPlane);
	}

	template <typename T>
	void PlaneVolume<T>::RemovePlane(Plane<T> aPlane)
	{
		myPlanes.RemoveCyclic(aPlane);
	}

	template <typename T>
	bool PlaneVolume<T>::Inside(Vector3<T> aPosition) const
	{
		for (unsigned short i = 0; i < myPlanes.Size(); ++i)
		{
			if (myPlanes[i].Inside(aPosition) == false)
			{
				return false;
			}
		}
		return true;
	}

	template <typename T>
	bool PlaneVolume<T>::Inside(Vector3<T> aPosition, T aRadius) const
	{
		for (unsigned short i = 0; i < myPlanes.Size(); ++i)
		{
			if (myPlanes[i].ClassifySpherePlane(aPosition, aRadius) > 0)
			{
				return false;
			}
		}
		return true;
	}

	template <typename T>
	bool PlaneVolume<T>::Inside(Vector3<T> aPosition, T aRadius, int& aFailPlaneOut) const
	{
		for (unsigned short i = 0; i < myPlanes.Size(); ++i)
		{
			if (myPlanes[i].ClassifySpherePlane(aPosition, aRadius) > 0)
			{
				aFailPlaneOut = i;
				return false;
			}
		}
		return true;
	}

	template <typename T>
	bool PlaneVolume<T>::CheckAABBInside(const Vector3<T>& someMinValues, const Vector3<T>& someMaxValues) const
	{
		Vector3<T> minValue;
		Vector3<T> maxValue;
		Vector3<T> normal;

		bool intersection = false;

		for (int i = 0; i < 6; i++)
		{
			maxValue = someMaxValues;
			minValue = someMinValues;

			normal = myPlanes[i].GetNormal();

			if (normal.x < 0)
			{
				minValue.x = someMaxValues.x;
				maxValue.x = someMinValues.x;
			}

			if (normal.y < 0)
			{
				minValue.y = someMaxValues.y;
				maxValue.y = someMinValues.y;
			}

			if (normal.z < 0)
			{
				minValue.z = someMaxValues.z;
				maxValue.z = someMinValues.z;
			}

			if (myPlanes[i].Inside(minValue) == false)
			{
				return false;
			}
			else if (myPlanes[i].Inside(maxValue) == true)
			{
				intersection = true;
			}
		}

		return true;
	}

	template <typename T>
	void PlaneVolume<T>::CalcWorldPlanes(PlaneVolume& aCopyFromVolume, const CU::Matrix44<float>& anOrientation)
	{
		myPlanes.RemoveAll();

		for each (const Plane<T>& plane in aCopyFromVolume.myPlanes)
		{
			Vector3<T> normal(plane.myABCD.x, plane.myABCD.y, plane.myABCD.z);

			Vector4<T> point(normal * plane.myABCD.w, 1.f);

			normal = normal * anOrientation;
			point = point * anOrientation;

			Plane<T> newPlane;
			newPlane.InitWithPointAndNormal(Vector3<T>(point.x, point.y, point.z), normal);

			myPlanes.Add(newPlane);
		}
	}
}