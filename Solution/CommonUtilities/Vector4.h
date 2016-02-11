#pragma once

#include <cmath>
#include <cassert>
#include "Vector4decl.h"

namespace CU
{
	typedef Vector4<float> Vector4f;

	template <typename T>
	Vector4<T>::Vector4()
	{
		x = 0;
		y = 0;
		z = 0;
		w = 0;
	}

	template <typename T>
	Vector4<T>::Vector4(T aX, T aY, T aZ, T aW)
	{
		x = aX;
		y = aY;
		z = aZ;
		w = aW;
	}

	template <typename T>
	Vector4<T>::Vector4(Vector3<T> aVector, T aW)
	{
		x = aVector.x;
		y = aVector.y;
		z = aVector.z;
		w = aW;
	}

	template <typename T>
	Vector4<T>::~Vector4()
	{
	}

	template <typename T>
	T Length(const Vector4<T>& aVector)
	{
		return sqrt(Length2(aVector));
	}

	template <typename T>
	T Length2(const Vector4<T>& aVector)
	{
		return Dot(aVector, aVector);
	}

	template <typename T>
	void Normalize(Vector4<T>& aVector)
	{
		if (aVector.x == 0 && aVector.y == 0 && aVector.z == 0 && aVector.w == 0)
		{
			//DL_DEBUG("Warning: Normalize zero vector.");
			return;
		}
		aVector = GetNormalized(aVector);
	}

	template <typename T>
	Vector4<T> GetNormalized(const Vector4<T>& aVector)
	{
		T length = Length(aVector);
		if (length == 0)
		{
			return aVector;
		}
		//assert(length != 0 && "Division by zero.");
		return Vector4<T>(aVector / length);
	}

	template <typename T>
	T Dot(const Vector4<T>& aFirstVector, const Vector4<T>& aSecondVector)
	{
		return aFirstVector.x * aSecondVector.x + aFirstVector.y * aSecondVector.y + aFirstVector.z * aSecondVector.z + aFirstVector.w * aSecondVector.w;
	}

	// operator with vector

	template <typename T>
	bool operator==(const Vector4<T>& aFirstVector, const Vector4<T>& aSecondVector)
	{
		return aFirstVector.x == aSecondVector.x && aFirstVector.y == aSecondVector.y && aFirstVector.z == aSecondVector.z && aFirstVector.w == aSecondVector.w;
	}

	template <typename T>
	bool operator!=(const Vector4<T>& aFirstVector, const Vector4<T>& aSecondVector)
	{
		return !(aFirstVector == aSecondVector);
	}

	template <typename T>
	Vector4<T> operator+(const Vector4<T>& aFirstVector, const Vector4<T>& aSecondVector)
	{
		Vector4<T> result(aFirstVector);
		result += aSecondVector;
		return result;
	}
	template <typename T>
	Vector4<T> operator+=(Vector4<T>& aFirstVector, const Vector4<T>& aSecondVector)
	{
		aFirstVector.x += aSecondVector.x;
		aFirstVector.y += aSecondVector.y;
		aFirstVector.z += aSecondVector.z;
		aFirstVector.w += aSecondVector.w;
		return aFirstVector;
	}

	template <typename T>
	Vector4<T> operator-(const Vector4<T>& aFirstVector, const Vector4<T>& aSecondVector)
	{
		Vector4<T> result(aFirstVector);
		result -= aSecondVector;
		return result;
	}
	template <typename T>
	Vector4<T> operator-=(Vector4<T>& aFirstVector, const Vector4<T>& aSecondVector)
	{
		aFirstVector.x -= aSecondVector.x;
		aFirstVector.y -= aSecondVector.y;
		aFirstVector.z -= aSecondVector.z;
		aFirstVector.w -= aSecondVector.w;
		return aFirstVector;
	}

	template <typename T>
	Vector4<T> operator*(const Vector4<T>& aFirstVector, const Vector4<T>& aSecondVector)
	{
		Vector4<T> result(aFirstVector);
		result *= aSecondVector;
		return result;
	}
	template <typename T>
	Vector4<T> operator*=(Vector4<T>& aFirstVector, const Vector4<T>& aSecondVector)
	{
		aFirstVector.x *= aSecondVector.x;
		aFirstVector.y *= aSecondVector.y;
		aFirstVector.z *= aSecondVector.z;
		aFirstVector.w *= aSecondVector.w;
		return aFirstVector;
	}

	template <typename T>
	Vector4<T> operator/(const Vector4<T>& aFirstVector, const Vector4<T>& aSecondVector)
	{
		Vector4<T> result(aFirstVector);
		result /= aSecondVector;
		return result;
	}
	template <typename T>
	Vector4<T> operator/=(Vector4<T>& aFirstVector, const Vector4<T>& aSecondVector)
	{
		assert(aSecondVector.x != 0 && aSecondVector.y != 0 && aSecondVector.z != 0 && aSecondVector.w != 0 && "Division by zero.");
		aFirstVector.x /= aSecondVector.x;
		aFirstVector.y /= aSecondVector.y;
		aFirstVector.z /= aSecondVector.z;
		aFirstVector.w /= aSecondVector.w;
		return aFirstVector;
	}

	// operator with scalar

	template <typename T>
	Vector4<T> operator+(const Vector4<T>& aVector, T aScalar)
	{
		Vector4<T> result(aVector);
		result += aScalar;
		return result;
	}
	template <typename T>
	Vector4<T> operator+(T aScalar, const Vector4<T>& aVector)
	{
		return aVector + aScalar;
	}
	template <typename T>
	Vector4<T> operator+=(Vector4<T>& aVector, T aScalar)
	{
		aVector.x += aScalar;
		aVector.y += aScalar;
		aVector.z += aScalar;
		aVector.w += aScalar;
		return aVector;
	}

	template <typename T>
	Vector4<T> operator-(const Vector4<T>& aVector, T aScalar)
	{
		Vector4<T> result(aVector);
		result -= aScalar;
		return result;
	}
	template <typename T>
	Vector4<T> operator-=(Vector4<T>& aVector, T aScalar)
	{
		aVector.x -= aScalar;
		aVector.y -= aScalar;
		aVector.z -= aScalar;
		aVector.w -= aScalar;
		return aVector;
	}

	template <typename T>
	Vector4<T> operator*(const Vector4<T>& aVector, T aScalar)
	{
		Vector4<T> result(aVector);
		result *= aScalar;
		return result;
	}
	template <typename T>
	Vector4<T> operator*(T aScalar, const Vector4<T>& aVector)
	{
		return aVector * aScalar;
	}
	template <typename T>
	Vector4<T> operator*=(Vector4<T>& aVector, T aScalar)
	{
		aVector.x *= aScalar;
		aVector.y *= aScalar;
		aVector.z *= aScalar;
		aVector.w *= aScalar;
		return aVector;
	}

	template <typename T>
	Vector4<T> operator/(const Vector4<T>& aVector, T aScalar)
	{
		Vector4<T> result(aVector);
		result /= aScalar;
		return result;
	}
	template <typename T>
	Vector4<T> operator/=(Vector4<T>& aVector, T aScalar)
	{
		//assert(aScalar != 0 && "Division by zero.");
		aVector.x /= aScalar;
		aVector.y /= aScalar;
		aVector.z /= aScalar;
		aVector.w /= aScalar;
		return aVector;
	}

	template <typename T>
	Vector4<T> operator-(const Vector4<T>& aVector)
	{
		return Vector4<T>(-aVector.x, -aVector.y, -aVector.z, -aVector.w);
	}
}