#pragma once
#include "Vector.h"
#include "Matrix33decl.h"
#include <cmath>


namespace CU
{
	template <typename T>
	Matrix33<T>::Matrix33()
	{
		for (unsigned char i = 0; i < 9; ++i)
		{
			myMatrix[i] = T(0);
		}

		myMatrix[0] = T(1);
		myMatrix[4] = T(1);
		myMatrix[8] = T(1);
	}

	template <typename T>
	Matrix33<T>::Matrix33(const Matrix33<T>& aMatrix)
	{
		for (unsigned char i = 0; i < 9; ++i)
		{
			myMatrix[i] = aMatrix.myMatrix[i];
		}
	}

	template <typename T>
	Matrix33<T>::Matrix33(const Matrix44<T>& aMatrix)
	{
		char j = 0;
		for (unsigned char i = 0; i < 9; ++i)
		{
			if (i > 0 && i % 3 == 0)
			{
				++j;
			}
			myMatrix[i] = aMatrix.myMatrix[i + j];
		}
	}

	template <typename T>
	Matrix33<T>::~Matrix33()
	{
	}


	template <typename T>
	Vector3<T> operator*(const Vector3<T>& aVector, const Matrix33<T>& aMatrix)
	{
		return Vector3<T>(
			aVector.x * aMatrix.myMatrix[0] + aVector.y * aMatrix.myMatrix[3] + aVector.z * aMatrix.myMatrix[6],
			aVector.x * aMatrix.myMatrix[1] + aVector.y * aMatrix.myMatrix[4] + aVector.z * aMatrix.myMatrix[7],
			aVector.x * aMatrix.myMatrix[2] + aVector.y * aMatrix.myMatrix[5] + aVector.z * aMatrix.myMatrix[8]);
	}


	template <typename T>
	Vector2<T> operator*(const Vector2<T>& aVector, const Matrix33<T>& aMatrix)
	{
		Vector3<T> v(aVector.x, aVector.y, 1);

		v = v * aMatrix;

		return Vector2<T>(v.x, v.y);
	}

	template <typename T>
	bool operator==(const Matrix33<T>& aFirstMatrix, const Matrix33<T>& aSecondMatrix)
	{
		for (unsigned char i = 0; i < 9; ++i)
		{
			if (aFirstMatrix.myMatrix[i] != aSecondMatrix.myMatrix[i])
			{
				return false;
			}
		}
		return true;
	}

	template <typename T>
	bool operator!=(const Matrix33<T>& aFirstMatrix, const Matrix33<T>& aSecondMatrix)
	{
		return !(operator==(aFirstMatrix, aSecondMatrix));
	}

	template <typename T>
	Matrix33<T> operator+=(Matrix33<T>& aFirstMatrix, const Matrix33<T>& aSecondMatrix)
	{
		for (unsigned char i = 0; i < 9; ++i)
		{
			aFirstMatrix.myMatrix[i] += aSecondMatrix.myMatrix[i];
		}
		return aFirstMatrix;
	}

	template <typename T>
	Matrix33<T> operator+(const Matrix33<T>& aFirstMatrix, const Matrix33<T>& aSecondMatrix)
	{
		Matrix33<T> result(aFirstMatrix);
		result += aSecondMatrix;
		return result;
	}

	template <typename T>
	Matrix33<T> operator-=(Matrix33<T>& aFirstMatrix, const Matrix33<T>& aSecondMatrix)
	{
		for (unsigned char i = 0; i < 9; ++i)
		{
			aFirstMatrix.myMatrix[i] -= aSecondMatrix.myMatrix[i];
		}
		return aFirstMatrix;
	}

	template <typename T>
	Matrix33<T> operator-(const Matrix33<T>& aFirstMatrix, const Matrix33<T>& aSecondMatrix)
	{
		Matrix33<T> result(aFirstMatrix);
		result -= aSecondMatrix;
		return result;
	}

	template <typename T>
	Matrix33<T> operator*(const Matrix33<T>& aFirstMatrix, const Matrix33<T>& aSecondMatrix)
	{
		Matrix33<T> result(aFirstMatrix);
		result *= aSecondMatrix;
		return result;
	}

	template <typename T>
	Matrix33<T> operator*=(Matrix33<T>& aFirstMatrix, const Matrix33<T>& aSecondMatrix)
	{
		Matrix33<T> originalFirst(aFirstMatrix);

		aFirstMatrix.myMatrix[0] =
			originalFirst.myMatrix[0] * aSecondMatrix.myMatrix[0] +
			originalFirst.myMatrix[1] * aSecondMatrix.myMatrix[3] +
			originalFirst.myMatrix[2] * aSecondMatrix.myMatrix[6];
		aFirstMatrix.myMatrix[1] =
			originalFirst.myMatrix[0] * aSecondMatrix.myMatrix[1] +
			originalFirst.myMatrix[1] * aSecondMatrix.myMatrix[4] +
			originalFirst.myMatrix[2] * aSecondMatrix.myMatrix[7];
		aFirstMatrix.myMatrix[2] =
			originalFirst.myMatrix[0] * aSecondMatrix.myMatrix[2] +
			originalFirst.myMatrix[1] * aSecondMatrix.myMatrix[5] +
			originalFirst.myMatrix[2] * aSecondMatrix.myMatrix[8];

		aFirstMatrix.myMatrix[3] =
			originalFirst.myMatrix[3] * aSecondMatrix.myMatrix[0] +
			originalFirst.myMatrix[4] * aSecondMatrix.myMatrix[3] +
			originalFirst.myMatrix[5] * aSecondMatrix.myMatrix[6];
		aFirstMatrix.myMatrix[4] =
			originalFirst.myMatrix[3] * aSecondMatrix.myMatrix[1] +
			originalFirst.myMatrix[4] * aSecondMatrix.myMatrix[4] +
			originalFirst.myMatrix[5] * aSecondMatrix.myMatrix[7];
		aFirstMatrix.myMatrix[5] =
			originalFirst.myMatrix[3] * aSecondMatrix.myMatrix[2] +
			originalFirst.myMatrix[4] * aSecondMatrix.myMatrix[5] +
			originalFirst.myMatrix[5] * aSecondMatrix.myMatrix[8];

		aFirstMatrix.myMatrix[6] =
			originalFirst.myMatrix[6] * aSecondMatrix.myMatrix[0] +
			originalFirst.myMatrix[7] * aSecondMatrix.myMatrix[3] +
			originalFirst.myMatrix[8] * aSecondMatrix.myMatrix[6];
		aFirstMatrix.myMatrix[7] =
			originalFirst.myMatrix[6] * aSecondMatrix.myMatrix[1] +
			originalFirst.myMatrix[7] * aSecondMatrix.myMatrix[4] +
			originalFirst.myMatrix[8] * aSecondMatrix.myMatrix[7];
		aFirstMatrix.myMatrix[8] =
			originalFirst.myMatrix[6] * aSecondMatrix.myMatrix[2] +
			originalFirst.myMatrix[7] * aSecondMatrix.myMatrix[5] +
			originalFirst.myMatrix[8] * aSecondMatrix.myMatrix[8];

		return aFirstMatrix;
	}

	template<typename Type>
	inline void operator*=(Vector3<Type> &aVector, const Matrix33<Type> &aMatrix)
	{
		aVector = aVector * aMatrix;
	}

	template<typename Type>
	inline void operator*=(Vector2<Type> &aVector, const Matrix33<Type> &aMatrix)
	{
		aVector = aVector * aMatrix;
	}

	template <typename T>
	Matrix33<T> operator*(const Matrix33<T>& aMatrix, float aScalar)
	{
		Matrix33<T> result(aMatrix);
		result *= aScalar;
		return result;
	}

	template <typename T>
	Matrix33<T> operator*=(Matrix33<T>& aMatrix, float aScalar)
	{
		aMatrix.myMatrix[0] *= aScalar;
		aMatrix.myMatrix[1] *= aScalar;
		aMatrix.myMatrix[2] *= aScalar;

		aMatrix.myMatrix[3] *= aScalar;
		aMatrix.myMatrix[4] *= aScalar;
		aMatrix.myMatrix[5] *= aScalar;

		aMatrix.myMatrix[6] *= aScalar;
		aMatrix.myMatrix[7] *= aScalar;
		aMatrix.myMatrix[8] *= aScalar;

		return aMatrix;
	}

	template <typename T>
	Matrix33<T> operator+(const Matrix33<T>& aMatrix, float aScalar)
	{
		Matrix33<T> result(aMatrix);
		result += aScalar;
		return result;
	}

	template <typename T>
	Matrix33<T> operator+=(Matrix33<T>& aMatrix, float aScalar)
	{
		aMatrix.myMatrix[0] += aScalar;
		aMatrix.myMatrix[1] += aScalar;
		aMatrix.myMatrix[2] += aScalar;

		aMatrix.myMatrix[3] += aScalar;
		aMatrix.myMatrix[4] += aScalar;
		aMatrix.myMatrix[5] += aScalar;

		aMatrix.myMatrix[6] += aScalar;
		aMatrix.myMatrix[7] += aScalar;
		aMatrix.myMatrix[8] += aScalar;

		return aMatrix;
	}

	template <typename T>
	Matrix33<T> Matrix33<T>::CreateRotateAroundX(T aAngleInRadians)
	{
		Matrix33<T> M;

		M.myMatrix[4] = cos(aAngleInRadians);
		M.myMatrix[5] = sin(aAngleInRadians);
		M.myMatrix[7] = -sin(aAngleInRadians);
		M.myMatrix[8] = cos(aAngleInRadians);

		return M;
	}

	template <typename T>
	Matrix33<T> Matrix33<T>::CreateRotateAroundY(T aAngleInRadians)
	{
		Matrix33<T> M;

		M.myMatrix[0] = cos(aAngleInRadians);
		M.myMatrix[2] = -sin(aAngleInRadians);
		M.myMatrix[6] = sin(aAngleInRadians);
		M.myMatrix[8] = cos(aAngleInRadians);

		return M;
	}

	template <typename T>
	Matrix33<T> Matrix33<T>::CreateRotateAroundZ(T aAngleInRadians)
	{
		Matrix33<T> M;

		M.myMatrix[0] = cos(aAngleInRadians);
		M.myMatrix[1] = sin(aAngleInRadians);
		M.myMatrix[3] = -sin(aAngleInRadians);
		M.myMatrix[4] = cos(aAngleInRadians);

		return M;
	}

	template<class T>
	Matrix33<T> Matrix33<T>::CreateReflectionMatrixAboutAxis(CU::Vector3<T> aReflectionVector)
	{
		Matrix33<T> reflectionMatrix;
		reflectionMatrix.myMatrix[0] = 1 - 2 * (aReflectionVector.x*aReflectionVector.x);
		reflectionMatrix.myMatrix[1] = -2 * (aReflectionVector.x*aReflectionVector.y);
		reflectionMatrix.myMatrix[2] = -2 * (aReflectionVector.x*aReflectionVector.z);

		reflectionMatrix.myMatrix[3] = -2 * (aReflectionVector.y*aReflectionVector.x);
		reflectionMatrix.myMatrix[4] = 1 - 2 * (aReflectionVector.y*aReflectionVector.y);
		reflectionMatrix.myMatrix[5] = -2 * (aReflectionVector.y*aReflectionVector.z);

		reflectionMatrix.myMatrix[6] = -2 * (aReflectionVector.z*aReflectionVector.x);
		reflectionMatrix.myMatrix[7] = -2 * (aReflectionVector.z*aReflectionVector.y);
		reflectionMatrix.myMatrix[8] = 1 - 2 * (aReflectionVector.z*aReflectionVector.z);
		return reflectionMatrix;
	};

	template <typename T>
	void Matrix33<T>::Rotate2D(T aRadian)
	{
		CU::Matrix33<T> temp = CreateRotateAroundZ(aRadian);
		CU::Matrix33<T> temp2 = *this;

		temp2 *= temp;

		myMatrix[0] = temp2.myMatrix[0];
		myMatrix[1] = temp2.myMatrix[1];
		myMatrix[3] = temp2.myMatrix[3];
		myMatrix[4] = temp2.myMatrix[4];
	}

	template <typename T>
	void Matrix33<T>::SetRotation2D(T aAngleInRadians)
	{
		myMatrix[0] = cos(aAngleInRadians);
		myMatrix[1] = sin(aAngleInRadians);
		myMatrix[3] = -sin(aAngleInRadians);
		myMatrix[4] = cos(aAngleInRadians);
	}

	template <typename T>
	Matrix33<T> Transpose(const Matrix33<T>& aMatrix)
	{
		Matrix33<T> result(aMatrix);

		result.myMatrix[1] = aMatrix.myMatrix[3];
		result.myMatrix[2] = aMatrix.myMatrix[6];
		result.myMatrix[5] = aMatrix.myMatrix[7];

		result.myMatrix[3] = aMatrix.myMatrix[1];
		result.myMatrix[6] = aMatrix.myMatrix[2];
		result.myMatrix[7] = aMatrix.myMatrix[5];

		return result;
	}

	template <typename T>
	Matrix33<T> InverseSimple(const Matrix33<T>& aMatrix)
	{
		CU::Matrix33<float> inverse(aMatrix);

		CU::Vector3<float> translation = inverse.GetPos3();
		inverse.SetPos(CU::Vector3<float>(0, 0, 1.f));
		translation *= -1.f;
		translation.z = 1.f;
		inverse = CU::Transpose(inverse);
		translation = translation * inverse;

		inverse.SetPos(translation);
		return inverse;
	}

	template <typename T>
	void Matrix33<T>::SetPos(const CU::Vector2<T>& aPos)
	{
		myMatrix[6] = aPos.x;
		myMatrix[7] = aPos.y;
	}

	template <typename T>
	void Matrix33<T>::SetPos(const CU::Vector3<T>& aPos)
	{
		myMatrix[6] = aPos.x;
		myMatrix[7] = aPos.y;
		myMatrix[8] = aPos.z;
	}

	template <typename T>
	CU::Vector2<T> Matrix33<T>::GetPos() const
	{
		return CU::Vector2<T>(myMatrix[6], myMatrix[7]);
	}

	template <typename T>
	CU::Vector3<T> Matrix33<T>::GetPos3() const
	{
		return CU::Vector3<T>(myMatrix[6], myMatrix[7], myMatrix[8]);
	}
}