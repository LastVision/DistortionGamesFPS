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
}