#pragma once

#include <cmath>
#include <cassert>
#include "Vector2decl.h"

namespace CU
{
	template <typename T>
	Vector2<T>::Vector2()
	{
		x = 0;
		y = 0;
	}

	template <typename T>
	Vector2<T>::Vector2(T aX, T aY)
	{
		x = aX;
		y = aY;
	}

	//template <typename T>
	//Vector2<T>::Vector2(Vector3<T> aVector3)
	//{
	//	x = aVector3.x;
	//	y = aVector3.y;
	//}

	template <typename T>
	Vector2<T>::~Vector2()
	{
	}

	template <typename T>
	T Length(const Vector2<T>& aVector)
	{
		return T(sqrt(Length2(aVector)));
	}

	template <typename T>
	T Length2(const Vector2<T>& aVector)
	{
		return Dot(aVector, aVector);
	}

	template <typename T>
	void Normalize(Vector2<T>& aVector)
	{
		aVector = GetNormalized(aVector);
	}

	template <typename T>
	Vector2<T> GetNormalized(const Vector2<T>& aVector)
	{
		T length = Length(aVector);
		if (length == 0)
		{
			return Vector2<T>();
		}
		return Vector2<T>(aVector / length);
	}

	template <typename T>
	T Dot(const Vector2<T>& aFirstVector, const Vector2<T>& aSecondVector)
	{
		return aFirstVector.x * aSecondVector.x + aFirstVector.y * aSecondVector.y;
	}

	// operator with vector

	template <typename T>
	bool operator==(const Vector2<T>& aFirstVector, const Vector2<T>& aSecondVector)
	{
		return aFirstVector.x == aSecondVector.x && aFirstVector.y == aSecondVector.y;
	}

	template <typename T>
	bool operator!=(const Vector2<T>& aFirstVector, const Vector2<T>& aSecondVector)
	{
		return !(aFirstVector == aSecondVector);
	}

	template <typename T>
	Vector2<T> operator+(const Vector2<T>& aFirstVector, const Vector2<T>& aSecondVector)
	{
		Vector2<T> result(aFirstVector);
		result += aSecondVector;
		return result;
	}
	template <typename T>
	Vector2<T> operator+=(Vector2<T>& aFirstVector, const Vector2<T>& aSecondVector)
	{
		aFirstVector.x += aSecondVector.x;
		aFirstVector.y += aSecondVector.y;
		return aFirstVector;
	}

	template <typename T>
	Vector2<T> operator-(const Vector2<T>& aFirstVector, const Vector2<T>& aSecondVector)
	{
		Vector2<T> result(aFirstVector);
		result -= aSecondVector;
		return result;
	}
	template <typename T>
	Vector2<T> operator-=(Vector2<T>& aFirstVector, const Vector2<T>& aSecondVector)
	{
		aFirstVector.x -= aSecondVector.x;
		aFirstVector.y -= aSecondVector.y;
		return aFirstVector;
	}

	template <typename T>
	Vector2<T> operator*(const Vector2<T>& aFirstVector, const Vector2<T>& aSecondVector)
	{
		Vector2<T> result(aFirstVector);
		result *= aSecondVector;
		return result;
	}
	template <typename T>
	Vector2<T> operator*=(Vector2<T>& aFirstVector, const Vector2<T>& aSecondVector)
	{
		aFirstVector.x *= aSecondVector.x;
		aFirstVector.y *= aSecondVector.y;
		return aFirstVector;
	}

	template <typename T>
	Vector2<T> operator/(const Vector2<T>& aFirstVector, const Vector2<T>& aSecondVector)
	{
		Vector2<T> result(aFirstVector);
		result /= aSecondVector;
		return result;
	}
	template <typename T>
	Vector2<T> operator/=(Vector2<T>& aFirstVector, const Vector2<T>& aSecondVector)
	{
		assert(aSecondVector.x != 0 && aSecondVector.y != 0 && "Division by zero.");
		aFirstVector.x /= aSecondVector.x;
		aFirstVector.y /= aSecondVector.y;
		return aFirstVector;
	}

	// operator with scalar

	template <typename T>
	Vector2<T> operator+(const Vector2<T>& aVector, T aScalar)
	{
		Vector2<T> result(aVector);
		result += aScalar;
		return result;
	}
	template <typename T>
	Vector2<T> operator+(T aScalar, const Vector2<T>& aVector)
	{
		return aVector + aScalar;
	}
	template <typename T>
	Vector2<T> operator+=(Vector2<T>& aVector, T aScalar)
	{
		aVector.x += aScalar;
		aVector.y += aScalar;
		return aVector;
	}

	template <typename T>
	Vector2<T> operator-(const Vector2<T>& aVector, T aScalar)
	{
		Vector2<T> result(aVector);
		result -= aScalar;
		return result;
	}

	template <typename T>
	Vector2<T> operator-=(Vector2<T>& aVector, T aScalar)
	{
		aVector.x -= aScalar;
		aVector.y -= aScalar;
		return aVector;
	}

	template <typename T>
	Vector2<T> operator*(const Vector2<T>& aVector, T aScalar)
	{
		Vector2<T> result(aVector);
		result *= aScalar;
		return result;
	}
	template <typename T>
	Vector2<T> operator*(T aScalar, const Vector2<T>& aVector)
	{
		return aVector * aScalar;
	}
	template <typename T>
	Vector2<T> operator*=(Vector2<T>& aVector, T aScalar)
	{
		aVector.x *= aScalar;
		aVector.y *= aScalar;
		return aVector;
	}

	template <typename T>
	Vector2<T> operator/(const Vector2<T>& aVector, T aScalar)
	{
		Vector2<T> result(aVector);
		result /= aScalar;
		return result;
	}
	template <typename T>
	Vector2<T> operator/=(Vector2<T>& aVector, T aScalar)
	{
		assert(aScalar != 0 && "Division by zero.");
		aVector.x /= aScalar;
		aVector.y /= aScalar;
		return aVector;
	}

	template <typename T>
	Vector2<T> operator-(const Vector2<T>& aVector)
	{
		return Vector2<T>(-aVector.x, -aVector.y);
	}
}