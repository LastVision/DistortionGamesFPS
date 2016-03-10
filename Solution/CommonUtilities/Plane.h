#pragma once
#include "Vector.h"

namespace CU
{
	template <typename T>
	class Plane
	{
	public:
		Plane();
		Plane(Vector3<T> aFirstPoint, Vector3<T> aSecondPoint, Vector3<T> aThirdPoint);
		Plane(Vector3<T> aPoint, Vector3<T> aNormal);
		~Plane();
		bool operator==(const Plane<T>& aPlane) const;
		bool operator!=(const Plane<T>& aPlane) const;
		void InitWith3Points(Vector3<T> aFirstPoint, Vector3<T> aSecondPoint, Vector3<T> aThirdPoint);
		void InitWithPointAndNormal(Vector3<T> aPoint, Vector3<T> aDirection);
		bool Inside(Vector3<T> aPosition) const;
		int ClassifySpherePlane(Vector3<T> aSpherePosition, float aSphereRadius) const;
		Vector3<T> GetNormal() const;

		Vector3<T> IntersectionThreePlanes(const Plane<T>& aSecondPlane, const Plane<T>& aThirdPlane) const;

		Vector4<T> myABCD;
	};
}

namespace CU
{
	template <typename T>
	Plane<T>::Plane()
	{
	}

	template <typename T>
	Plane<T>::Plane(Vector3<T> aFirstPoint, Vector3<T> aSecondPoint, Vector3<T> aThirdPoint)
	{
		InitWith3Points(aFirstPoint, aSecondPoint, aThirdPoint);
	}

	template <typename T>
	Plane<T>::Plane(Vector3<T> aFirstPoint, Vector3<T> aNormal)
	{
		InitWithPointAndNormal(aFirstPoint, aNormal);
	}

	template <typename T>
	bool Plane<T>::operator==(const Plane<T>& aPlane) const
	{
		if (myABCD == aPlane.myABCD)
		{
			return true;
		}
		return false;
	}

	template <typename T>
	bool Plane<T>::operator!=(const Plane<T>& aPlane) const
	{
		return !(*this == aPlane);
	}

	template <typename T>
	Plane<T>::~Plane()
	{
	}

	template <typename T>
	void Plane<T>::InitWith3Points(Vector3<T> aFirstPoint, Vector3<T> aSecondPoint, Vector3<T> aThirdPoint)
	{
		Vector3<T> normal = Cross(aSecondPoint - aFirstPoint, aFirstPoint - aThirdPoint);
		CU::Normalize(normal);

		myABCD.x = normal.x;
		myABCD.y = normal.y;
		myABCD.z = normal.z;
		myABCD.w = Dot(aFirstPoint, normal);
	}

	template <typename T>
	void Plane<T>::InitWithPointAndNormal(Vector3<T> aPoint, Vector3<T> aDirection)
	{
		myABCD.x = aDirection.x;
		myABCD.y = aDirection.y;
		myABCD.z = aDirection.z;
		myABCD.w = Dot(aPoint, Vector3<T>(myABCD.x, myABCD.y, myABCD.z));
	}

	template <typename T>
	bool Plane<T>::Inside(Vector3<T> aPosition) const
	{
		if (Dot(aPosition, Vector3<T>(myABCD.x, myABCD.y, myABCD.z)) < myABCD.w)
		{
			return true;
		}
		return false;
	}

	template <typename T>
	inline int Plane<T>::ClassifySpherePlane(Vector3<T> aSpherePosition, float aSphereRadius) const
	{
		float distance = Dot(GetNormal(), aSpherePosition) - myABCD.w;

		// completely on the front side
		if (distance >= aSphereRadius)
		{
			return 1;
		}

		// completely on the backside (aka "inside")
		if (distance <= -aSphereRadius)
		{
			return -1;
		}

		//sphere intersects the plane
		return 0;
	}

	template <typename T>
	inline Vector3<T> Plane<T>::GetNormal() const
	{
		return Vector3<T>(myABCD.x, myABCD.y, myABCD.z);
	}

	template <typename T>
	Vector3<T> Plane<T>::IntersectionThreePlanes(const Plane<T>& aSecondPlane, const Plane<T>& aThirdPlane) const
	{
		Vector3<T> firstPlaneNormal = GetNormal();
		Vector3<T> secondPlaneNormal = aSecondPlane.GetNormal();
		Vector3<T> thirdPlaneNormal = aThirdPlane.GetNormal();

		T firstPlaneD = myABCD.w;// Dot(firstPlaneNormal, myPoint);
		T secondPlaneD = aSecondPlane.myABCD.w;// Dot(secondPlaneNormal, aSecondPlane.myPoint);
		T thirdPlaneD = aThirdPlane.myABCD.w;// Dot(thirdPlaneNormal, aThirdPlane.myPoint);

		Vector3<T> interSectionPoint = (firstPlaneD * Cross(secondPlaneNormal, thirdPlaneNormal));
		interSectionPoint += (secondPlaneD * Cross(thirdPlaneNormal, firstPlaneNormal));
		interSectionPoint += (thirdPlaneD * Cross(firstPlaneNormal, secondPlaneNormal));
		interSectionPoint /= Dot(Cross(firstPlaneNormal, secondPlaneNormal), thirdPlaneNormal);
		return interSectionPoint;
	}
}