#ifndef VECTOR_MATH_HEADER
#define VECTOR_MATH_HEADER

namespace CommonLib
{
	template<typename T>
	Vector2<T>& operator*=(Vector2<T>& leftVector, const Matrix22<T>& aMatrix)
	{
		float currentData[2];
		currentData[0] = leftVector.myData[0];
		currentData[1] = leftVector.myData[1];
		for(int y = 0;y < 2;y++)
		{
			float currentValue = 0;
			for(int x = 0;x < 2;x++)
			{
				currentValue += currentData[x] * aMatrix[y][x];
			}
			leftVector.myData[y] = currentValue;
		}
		return leftVector;
	}

		template <class TYPE>
	Vector2<TYPE> operator*(const CL::Vector2<TYPE>& aLeftVector,const Matrix22<TYPE>& aRightMatrix)
	{
		CL::Vector2<T> newVector(aLeftVector);
		newVector *= aRightMatrix;
		return newVector;
	}
};

namespace CL = CommonLib;

#endif