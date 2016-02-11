#pragma once
#include "Vector.h"

namespace CU
{
	template <typename T>
	class Line
	{
	public:
		Line();
		Line(Vector2<T> aFirstPoint, Vector2<T> aSecondPoint);
		~Line();
		bool operator==(const Line<T>& aLine);
		bool operator!=(const Line<T>& aLine);
		void InitWith2Points(Vector2<T> aFirstPoint, Vector2<T> aSecondPoint);
		void InitWithPointAndDirection(Vector2<T> aPoint, Vector2<T> aDirection);
		bool Inside(Vector2<T> aPosition);

		Vector3<T> myABC;
	};
}

namespace CU
{
	template <typename T>
	Line<T>::Line()
	{
	}

	template <typename T>
	Line<T>::Line(Vector2<T> aFirstPoint, Vector2<T> aSecondPoint)
	{
		InitWith2Points(aFirstPoint, aSecondPoint);
	}

	template <typename T>
	Line<T>::~Line()
	{
	}

	template <typename T>
	bool Line<T>::operator==(const Line<T>& aLine)
	{
		if (myABC == aLine.myABC)
		{
			return true;
		}
		return false;
	}

	template <typename T>
	bool Line<T>::operator!=(const Line<T>& aLine)
	{
		return !(*this == aLine);
	}

	template <typename T>
	void Line<T>::InitWith2Points(Vector2<T> aFirstPoint, Vector2<T> aSecondPoint)
	{
		Vector2<T> line = aSecondPoint - aFirstPoint;
		Vector2<T> normal = Vector2<T>(-line.y, line.x);
		Normalize(normal);

		myABC.x = normal.x;
		myABC.y = normal.y;
		myABC.z = Dot(aFirstPoint, normal);
	}

	template <typename T>
	void Line<T>::InitWithPointAndDirection(Vector2<T> aPoint, Vector2<T> aDirection)
	{
		Normalize(aDirection);
		myABC.x = -aDirection.y;
		myABC.y = aDirection.x;
		myABC.z = Dot(aPoint, Vector2<T>(myABC.x, myABC.y));
	}

	template <typename T>
	bool Line<T>::Inside(Vector2<T> aPosition)
	{
		if (Dot(aPosition, Vector2<T>(myABC.x, myABC.y)) < myABC.z)
		{
			return true;
		}
		return false;
	}
}