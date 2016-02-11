#ifndef MATRIX44_OPERATIONS_HEADER
#define MATRIX44_OPERATIONS_HEADER

#include "Matrix44.h"


namespace CommonLib
{
	template<typename TYPE>
	Matrix44<TYPE>& operator+=(Matrix44<TYPE>& aLeftMatrix,const Matrix44<TYPE>& aRightMatrix)
	{
		for(int i = 0;i < 16;i++)
		{
			aLeftMatrix.myData[i] += aRightMatrix.myData[i];
		}
		return (aLeftMatrix);
	}

	template<typename TYPE>
	Matrix44<TYPE> operator+(const Matrix44<TYPE>& aLeftMatrix,const Matrix44<TYPE>& aRightMatrix)
	{
		Matrix44<TYPE> temp(aLeftMatrix);
		temp += aRightMatrix;
		return temp;
	}

	template<typename TYPE>
	Matrix44<TYPE>& operator*=(Matrix44<TYPE>& aLeftMatrix,const TYPE aValue)
	{
		for(int i = 0;i < 16;i++)
		{
			aLeftMatrix.myData[i] *= aValue;
		}
		return (aLeftMatrix);
	}

	template<typename TYPE>
	Matrix44<TYPE> operator*(const Matrix44<TYPE>& aLeftMatrix,const TYPE aValue)
	{
		Matrix44<TYPE> temp(aLeftMatrix);
		temp *= aValue;
		return temp;
	}

	template<typename TYPE>
	Vector3<TYPE> GetAxisVector(const Matrix44<TYPE>& aMatrix, int aColumn)
	{
		return CL::Vector3f(aMatrix.myData[aColumn],aMatrix.myData[aColumn+4],aMatrix.myData[aColumn+8]);
	}
};

#endif