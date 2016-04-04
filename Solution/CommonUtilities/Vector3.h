#pragma once

#include <cmath>
#include <cassert>
#include "Vector3decl.h"

namespace CU
{
	typedef Vector3<float> Vector3f;
	template <typename T>
	Vector3<T>::Vector3()
	{
		x = 0;
		y = 0;
		z = 0;
	}

	template <typename T>
	Vector3<T>::Vector3(T aX, T aY, T aZ)
	{
		x = aX;
		y = aY;
		z = aZ;
	}

	template <typename T>
	Vector3<T>::~Vector3()
	{
	}

	template <typename T>
	T Length(const Vector3<T>& aVector)
	{
		return sqrt(Length2(aVector));
	}

	template <typename T>
	T Length2(const Vector3<T>& aVector)
	{
		return Dot(aVector, aVector);
	}

	template <typename T>
	void Normalize(Vector3<T>& aVector)
	{
		if (aVector.x == 0 && aVector.y == 0 && aVector.z == 0)
		{
			//DL_DEBUG("Warning: Normalize zero vector.");
			return;
		}
		aVector = GetNormalized(aVector);
	}

	template <typename T>
	Vector3<T> GetNormalized(const Vector3<T>& aVector)
	{
		T length = Length(aVector);
		if (length == 0)
		{
			return aVector;
		}
		//assert(length != 0 && "Division by zero.");
		return Vector3<T>(aVector / length);
	}

	template <typename T>
	T Dot(const Vector3<T>& aFirstVector, const Vector3<T>& aSecondVector)
	{
		return aFirstVector.x * aSecondVector.x + aFirstVector.y * aSecondVector.y + aFirstVector.z * aSecondVector.z;
	}

	template <typename T>
	Vector3<T> Cross(const Vector3<T>& aFirstVector, const Vector3<T>& aSecondVector)
	{
		return Vector3<T>(
			aFirstVector.y * aSecondVector.z - aFirstVector.z * aSecondVector.y,
			aFirstVector.z * aSecondVector.x - aFirstVector.x * aSecondVector.z,
			aFirstVector.x * aSecondVector.y - aFirstVector.y * aSecondVector.x);
	}

	// operator with vector

	template <typename T>
	bool operator==(const Vector3<T>& aFirstVector, const Vector3<T>& aSecondVector)
	{
		return aFirstVector.x == aSecondVector.x && aFirstVector.y == aSecondVector.y && aFirstVector.z == aSecondVector.z;
	}

	template <typename T>
	bool operator!=(const Vector3<T>& aFirstVector, const Vector3<T>& aSecondVector)
	{
		return !(aFirstVector == aSecondVector);
	}

	template <typename T>
	Vector3<T> operator+(const Vector3<T>& aFirstVector, const Vector3<T>& aSecondVector)
	{
		Vector3<T> result(aFirstVector);
		result += aSecondVector;
		return result;
	}
	template <typename T>
	Vector3<T> operator+=(Vector3<T>& aFirstVector, const Vector3<T>& aSecondVector)
	{
		aFirstVector.x += aSecondVector.x;
		aFirstVector.y += aSecondVector.y;
		aFirstVector.z += aSecondVector.z;
		return aFirstVector;
	}

	template <typename T>
	Vector3<T> operator-(const Vector3<T>& aFirstVector, const Vector3<T>& aSecondVector)
	{
		Vector3<T> result(aFirstVector);
		result -= aSecondVector;
		return result;
	}
	template <typename T>
	Vector3<T> operator-=(Vector3<T>& aFirstVector, const Vector3<T>& aSecondVector)
	{
		aFirstVector.x -= aSecondVector.x;
		aFirstVector.y -= aSecondVector.y;
		aFirstVector.z -= aSecondVector.z;
		return aFirstVector;
	}

	template <typename T>
	Vector3<T> operator*(const Vector3<T>& aFirstVector, const Vector3<T>& aSecondVector)
	{
		Vector3<T> result(aFirstVector);
		result *= aSecondVector;
		return result;
	}
	template <typename T>
	Vector3<T> operator*=(Vector3<T>& aFirstVector, const Vector3<T>& aSecondVector)
	{
		aFirstVector.x *= aSecondVector.x;
		aFirstVector.y *= aSecondVector.y;
		aFirstVector.z *= aSecondVector.z;
		return aFirstVector;
	}

	template <typename T>
	Vector3<T> operator/(const Vector3<T>& aFirstVector, const Vector3<T>& aSecondVector)
	{
		Vector3<T> result(aFirstVector);
		result /= aSecondVector;
		return result;
	}
	template <typename T>
	Vector3<T> operator/=(Vector3<T>& aFirstVector, const Vector3<T>& aSecondVector)
	{
		assert(aSecondVector.x != 0 && aSecondVector.y != 0  && aSecondVector.z != 0 && "Division by zero.");
		aFirstVector.x /= aSecondVector.x;
		aFirstVector.y /= aSecondVector.y;
		aFirstVector.z /= aSecondVector.z;
		return aFirstVector;
	}

	// operator with scalar

	template <typename T>
	Vector3<T> operator+(const Vector3<T>& aVector, T aScalar)
	{
		Vector3<T> result(aVector);
		result += aScalar;
		return result;
	}
	template <typename T>
	Vector3<T> operator+(T aScalar, const Vector3<T>& aVector)
	{
		return aVector + aScalar;
	}
	template <typename T>
	Vector3<T> operator+=(Vector3<T>& aVector, T aScalar)
	{
		aVector.x += aScalar;
		aVector.y += aScalar;
		aVector.z += aScalar;
		return aVector;
	}

	template <typename T>
	Vector3<T> operator-(const Vector3<T>& aVector, T aScalar)
	{
		Vector3<T> result(aVector);
		result -= aScalar;
		return result;
	}
	template <typename T>
	Vector3<T> operator-=(Vector3<T>& aVector, T aScalar)
	{
		aVector.x -= aScalar;
		aVector.y -= aScalar;
		aVector.z -= aScalar;
		return aVector;
	}

	template <typename T>
	Vector3<T> operator*(const Vector3<T>& aVector, T aScalar)
	{
		Vector3<T> result(aVector);
		result *= aScalar;
		return result;
	}
	template <typename T>
	Vector3<T> operator*(T aScalar, const Vector3<T>& aVector)
	{
		return aVector * aScalar;
	}
	template <typename T>
	Vector3<T> operator*=(Vector3<T>& aVector, T aScalar)
	{
		aVector.x *= aScalar;
		aVector.y *= aScalar;
		aVector.z *= aScalar;
		return aVector;
	}

	template <typename T>
	Vector3<T> operator/(const Vector3<T>& aVector, T aScalar)
	{
		Vector3<T> result(aVector);
		result /= aScalar;
		return result;
	}
	template <typename T>
	Vector3<T> operator/=(Vector3<T>& aVector, T aScalar)
	{
		//assert(aScalar != 0 && "Division by zero.");
		aVector.x /= aScalar;
		aVector.y /= aScalar;
		aVector.z /= aScalar;
		return aVector;
	}

	template <typename T>
	Vector3<T> operator-(const Vector3<T>& aVector)
	{
		return Vector3<T>(-aVector.x, -aVector.y, -aVector.z);
	}

	template <typename T>
	Vector3<T> Reflect(const CU::Vector3<T>& aDirection, const CU::Vector3<T>& aNormal)
	{
		return (aDirection - 2.f * CU::Dot(aDirection, aNormal) * aNormal);
	}


}