#pragma once

#include "Matrix22Ops.h"
#include "Matrix44.h"
#include "Angle3.h"

namespace CommonLib
{
	template<typename TYPE>
	Matrix33<TYPE> operator*(TYPE aValue,const Matrix33<TYPE>& aMatrix)
	{
		Matrix33 temp = aMatrix;
		temp *= aValue;
		return temp;
	}

	template<typename TYPE>
	Matrix33<TYPE>& operator*=(Matrix33<TYPE>& aMatrix,TYPE aValue)
	{
		for(int i = 0;i < 3*3;++i)
		{
			aMatrix.myData[i]*=aValue;
		}
		return aMatrix;
	}

	template<typename TYPE>
	void MatrixMultiply(TYPE* aLeftData, TYPE* aRightData, int SizeSqrt, TYPE* outData)
	{
		for(int y = 0;y < SizeSqrt;++y)
		{
			for(int x = 0;x < SizeSqrt;++x)
			{
				int currentSpot = y*SizeSqrt+x;
				auto& currentValue = outData[currentSpot];
				currentValue = 0;
				for( int i = 0; i < SizeSqrt;++i)
				{
					currentValue += aLeftData[y*SizeSqrt+i] * aRightData[i*SizeSqrt+x];
				}
			}
		}
	}

	const int SupportedInverseSize = 5;

	template<typename TYPE>
	void GetInverse(TYPE* data,int SizeSqrt,TYPE* outData)
	{
		assert(SizeSqrt <= SupportedInverseSize && "only matrix of size 5 or smaller is supported");
		float determinant = GetDeterminant(data,SizeSqrt,SizeSqrt);
		determinant = 1.0f/determinant;
		float cofactorMatrix[SupportedInverseSize*SupportedInverseSize];
		for(int i = 0;i < SizeSqrt;++i)
		{
			for(int j = 0;j < SizeSqrt;++j)
			{
				float minorMatrix[(SupportedInverseSize-1)*(SupportedInverseSize-1)];
				GetMinor(data,minorMatrix,j,i,SizeSqrt);
				int currentIndex = i*SizeSqrt+j;
				cofactorMatrix[currentIndex] = GetDeterminant(minorMatrix,SizeSqrt-1,SizeSqrt-1);
				cofactorMatrix[currentIndex]*=-1;
				if(currentIndex%2 == 0)
				{
					cofactorMatrix[currentIndex]*=-1;
				}
				cofactorMatrix[currentIndex]*=determinant;
			}
		}
		for(int i = 0;i < SizeSqrt;++i)
		{
			for(int j = 0;j < SizeSqrt;++j)
			{
				int currentIndex = j*SizeSqrt+i;
				int currentoutIndex = i*SizeSqrt+j;
				outData[currentoutIndex] = cofactorMatrix[currentIndex];
			}
		}
	}

	template<typename TYPE>
	Matrix33<TYPE> GetInverse(const Matrix33<TYPE>& aMatrix, float determinant)
	{
		Matrix33<TYPE> C = GetCofactorMatrix(aMatrix);
		C.Transpose();
		return C*(1.0f/determinant);
	}

	template<typename TYPE>
	Matrix33<TYPE> GetCofactorMatrix(const Matrix33<TYPE>& aMatrix)
	{
		Matrix33<TYPE> cofactorMatrix(GetDeterminant(GetMinorMatrix(aMatrix,0,0)),
			GetDeterminant(GetMinorMatrix(aMatrix,0,1)),
			GetDeterminant(GetMinorMatrix(aMatrix,0,2)),
			GetDeterminant(GetMinorMatrix(aMatrix,1,0)),
			GetDeterminant(GetMinorMatrix(aMatrix,1,1)),
			GetDeterminant(GetMinorMatrix(aMatrix,1,2)),
			GetDeterminant(GetMinorMatrix(aMatrix,2,0)),
			GetDeterminant(GetMinorMatrix(aMatrix,2,1)),
			GetDeterminant(GetMinorMatrix(aMatrix,2,2)));

		static const Matrix33<TYPE> checkerMatrix(+1,-1,+1,
			-1,+1,-1,
			+1,-1,+1);

		for(int i = 0;i < 3;i++)
		{
			for(int j = 0;j < 3;j++)
			{
				cofactorMatrix.myRows[i][j] *= checkerMatrix.myRows[i][j];
			}
		}
		return cofactorMatrix;
	}

	template<typename TYPE>
	Matrix22<TYPE> GetMinorMatrix(const Matrix33<TYPE>& aMatrix,int aRow, int aCol)
	{
		Matrix22<TYPE> minor;
		int currentRow = 0;
		
		for(int i = 0;i < 3;i++)
		{
			if(i != aRow)
			{
				int currentCol = 0;
				for(int j = 0;j < 3;j++)
				{
					if(j != aCol)
					{
						minor.myData[currentRow*2+currentCol] = aMatrix.myRows[i][j];
						++currentCol;
					}
				}
				++currentRow;
			}
		}
		return minor;
	}

	template<typename TYPE>
	void GetMinor(TYPE* inData, TYPE* outData,int indexX,int indexY,int Size)
	{
		int currentRow = 0;
		for(int y = 0;y < Size;++y)
		{
			if( y != indexY)
			{
				int currentCol = 0;
				for(int x = 0;x < Size;++x)
				{
					if(x != indexX)
					{
						outData[currentRow*(Size-1)+currentCol] = inData[y*Size+x];
						++currentCol;
					}
				}
				++currentRow;
			}
		}
	}

	template<typename TYPE>
	float GetDeterminant(TYPE* data, int sizeX,int sizeY)
	{
		if(sizeX == 1)
		{
			return data[0];
		}
		
		float value = 0;
		for(int i = 0;i < sizeX;++i)
		{
			if(i%2 == 0)
			{
				TYPE tmpData[128];
				GetMinor(data,tmpData,i,0,sizeX);
				value += data[i]*GetDeterminant(tmpData,(sizeX-1),(sizeY-1));
			}
			else
			{
				TYPE tmpData[128];
				GetMinor(data,tmpData,i,0,sizeX);
				value -= data[i]*GetDeterminant(tmpData,(sizeX-1),(sizeY-1));
			}
		}
		return value;
	};

	#define PI 3.141593f
	#define dPI 3.14159265

	template<typename TYPE>
	void GetAnglesFromMatrix(const Matrix33<TYPE> aMatrix, CL::Angle3<TYPE>& someOutAngles)
	{
		float sp = -aMatrix._32;
		if(sp <= -1.0f)
		{
			someOutAngles.p = -PI/2.0f;
		}
		else if(sp >= 1.0f)
		{
			someOutAngles.p = PI/2.0f;
		}
		else
		{
			someOutAngles.p = asin(sp);
		}

		if(fabs(sp) > 0.999f)
		{
			someOutAngles.b = 0.0f;
			someOutAngles.h = atan2(-aMatrix._13,aMatrix._11);
		}
		else
		{
			someOutAngles.h = atan2(aMatrix._31,aMatrix._33);
			someOutAngles.b = atan2(aMatrix._12,aMatrix._22);
		}
	}
};