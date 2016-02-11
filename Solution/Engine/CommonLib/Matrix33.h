#ifndef MATRIX_HEADER
#define MATRIX_HEADER

#include "Vector3.h"

namespace CommonLib
{
	template<class TYPE>
	class Matrix33
	{
	public:
		Matrix33();
		~Matrix33();

		Matrix33(const Matrix33& aMatrix);
		Matrix33(const Vector3<TYPE>& col1, const Vector3<TYPE>& col2, const Vector3<TYPE>& col3);
		Matrix33(TYPE a11,TYPE a12,TYPE a13,
				TYPE a21,TYPE a22,TYPE a23,
				TYPE a31,TYPE a32,TYPE a33);
		Matrix33(TYPE* data);

		void SetRotationAngles(TYPE aBank,TYPE aPitch,TYPE aHeading);//this sets the inverse of the angles
		void SetToIdentity();

		inline const TYPE& operator()(const int aRow,const int aColumn) const;
		inline TYPE& operator()(const int aRow,const int aColumn);

		void Invert(TYPE aDeterminant);
		void Transpose();

		TYPE GetDeterminant();

		inline void GetCofactor(TYPE cofactor[2][2],int aRow,int aColumn);

		void SetTranslation(const CL::Vector2f& aTranslation);
		void SetTranslation(TYPE aX, TYPE aY);

		CL::Vector3f GetColumn(int i);

		CL::Vector2f GetTranslation() const;

		void SetRotation2D(TYPE aRotation);

		Matrix33& operator=(const Matrix33& aRightMatrix);
		Matrix33& operator*=(const Matrix33& aRightMatrix);
		Vector3<TYPE> operator*(const Vector3<TYPE>& aVector);

		union
		{
			TYPE myData[3*3];
			struct
			{
				TYPE myRows[3][3];
			};
			struct
			{
				CL::Vector3<TYPE> myVectorRows[3];
			};
			struct
			{
				TYPE	_11,_12,_13,
						_21,_22,_23,
						_31,_32,_33;
			};
		};
	private:
	};

	template<class TYPE>
	Matrix33<TYPE>::Matrix33()
	{
	}

	template<class TYPE>
	Matrix33<TYPE>::~Matrix33()
	{
	}

	template<class TYPE>
	void Matrix33<TYPE>::SetToIdentity()
	{
		_11 = 1;
		_12 = 0;
		_13 = 0;

		_21 = 0;
		_22 = 1;
		_23 = 0;

		_31 = 0;
		_32 = 0;
		_33 = 1;
	}

	template<class TYPE>
	Matrix33<TYPE>::Matrix33(TYPE a11,TYPE a12,TYPE a13,
			TYPE a21,TYPE a22,TYPE a23,
			TYPE a31,TYPE a32,TYPE a33)
	{
		_11 = a11;
		_12 = a12;
		_13 = a13;
		_21 = a21;
		_22 = a22;
		_23 = a23;
		_31 = a31;
		_32 = a32;
		_33 = a33;
	}

	template<class TYPE>
	Matrix33<TYPE>::Matrix33(const Vector3<TYPE>& col1, const Vector3<TYPE>& col2, const Vector3<TYPE>& col3)
	{
		_11 = col1.x;
		_12 = col2.x;
		_13 = col3.x;
		_21 = col1.y;
		_22 = col2.y;
		_23 = col3.y;
		_31 = col1.z;
		_32 = col2.z;
		_33 = col3.z;
	}

	template<class TYPE>
	Matrix33<TYPE>::Matrix33(TYPE* data)
	{
		for(int i = 0;i < 9;i++)
		{
			myData[i] = data[i];
		}
	}

	template<class TYPE>
	Matrix33<TYPE>::Matrix33(const Matrix33& aMatrix)
	{
		(*this) = aMatrix;
	}

	template<class TYPE>
	Matrix33<TYPE>& Matrix33<TYPE>::operator=(const Matrix33& aRightMatrix)
	{
		for(int i = 0;i < 9;i++)
		{
			myData[i] = aRightMatrix.myData[i];
		}
		return (*this);
	}

	template<class TYPE>
	const TYPE& Matrix33<TYPE>::operator()(const int aRow,const int aColumn) const
	{
		return myData[aRow*3+aColumn];
	}

	template<class TYPE>
	TYPE& Matrix33<TYPE>::operator()(const int aRow,const int aColumn)
	{
		return myData[aRow*3+aColumn];
	}

	template<class TYPE>
	Matrix33<TYPE>& Matrix33<TYPE>::operator*=(const Matrix33<TYPE>& aRightMatrix)
	{
		Matrix33<TYPE> tempMatrix;
		for(int row = 0;row < 3;row++)
		{
			for(int column = 0;column < 3;++column)
			{
				TYPE currentValue = 0;
				for(int k = 0;k < 3;k++)
				{
					currentValue += myRows[k][column] * aRightMatrix(row,k);
				}
				tempMatrix(row,column) = currentValue;
			}
		}
		(*this) = tempMatrix;
		return (*this);
	}

	template<class TYPE>
	TYPE Matrix33<TYPE>::GetDeterminant()
	{
		float determinant = 0;
		int row = 0;
		for(int column = 0;column < 3;++column)
		{
			float currentValue = myRows[row][column];

			TYPE currentMatrix[2][2];

			GetCofactor(currentMatrix,row,column);

			currentValue *= (currentMatrix[0][0]*currentMatrix[1][1] - currentMatrix[0][1]*currentMatrix[1][0]);

			determinant += currentValue * 1-2*((column+row) % 2);
		}
		return determinant;
	}

	template<class TYPE>
	void Matrix33<TYPE>::GetCofactor(TYPE aCofactor[2][2],int aRow,int aColumn)
	{
		int cX = 0;
		for(int x = 0;x < 2;++x)
		{
			if(cX == aRow)
			{
				++cX;
			}
			int cY = 0;
			for(int y = 0;y < 2;++y)
			{
				if(cY == aColumn)
				{
					++cY;
				}
				aCofactor[x][y] = (*this)(cX,cY);
				++cY;
			}
			++cX;
		}
	}

	template<class TYPE>
	CL::Vector2f Matrix33<TYPE>::GetTranslation() const
	{
		return CL::Vector2f(_13,_23);
	};

	template<class TYPE>
	void Matrix33<TYPE>::SetTranslation(const CL::Vector2f& aTranslation)
	{
		_13 = aTranslation.x;
		_23 = aTranslation.y;
	}

	template<class TYPE>
	CL::Vector3f Matrix33<TYPE>::GetColumn(int i)
	{
		return CL::Vector3f(myData[i],myData[i+3],myData[i+6]);
	}

	template<class TYPE>
	void Matrix33<TYPE>::SetTranslation(TYPE aX, TYPE aY)
	{
		_13 = aX;
		_23 = aX;
	}

	template<class TYPE>
	void Matrix33<TYPE>::SetRotation2D(TYPE aRotation)
	{
		TYPE cosinus = cos(aRotation);
		TYPE sinus = sin(aRotation);
		_11 = cosinus;
		_12 = -sinus;
		_21 = sinus;
		_22 = cosinus;
	}

	template<class TYPE>
	void Matrix33<TYPE>::Invert(TYPE aDeterminant)
	{
		Matrix33 inverse;
		for(int row = 0;row < 3;++row)
		{
			for(int column = 0;column < 3;++column)
			{
				TYPE cofactor[2][2];
			
				GetCofactor(cofactor,row,column);

				inverse(row,column) = (cofactor[0][0]*cofactor[1][1] - cofactor[0][1]*cofactor[1][0]) * 1-2*((column+row) % 2);
			}
		}
		inverse.Transpose();
		for(int i = 0;i < 9;++i)
		{
			inverse.myData[i]/= aDeterminant;
		}
		(*this) = inverse;
	}

	template<class TYPE>
	void Matrix33<TYPE>::Transpose()
	{
		Matrix33<TYPE> temp;
		temp._11 = _11;
		temp._21 = _12;
		temp._31 = _13;

		temp._12 = _21;
		temp._22 = _22;
		temp._32 = _23;

		temp._13 = _31;
		temp._23 = _32;
		temp._33 = _33;
		(*this) = temp;
	}

	template<class TYPE>
	Vector3<TYPE> Matrix33<TYPE>::operator*(const Vector3<TYPE>& aVector)
	{
		Vector3<TYPE> temp;
		temp[0] = aVector.Dot(Vector3<TYPE>(myRows[0]));
		temp[1] = aVector.Dot(Vector3<TYPE>(myRows[1]));
		temp[2] = aVector.Dot(Vector3<TYPE>(myRows[2]));
		return temp;
	}

	template<class TYPE>
	Vector3<TYPE> operator*(const Vector3<TYPE>& aVector, const Matrix33<TYPE>& aMatrix)
	{
		Vector3<TYPE> temp;
		temp[0] = aVector.Dot(Vector3<TYPE>(aMatrix.myRows[0]));
		temp[1] = aVector.Dot(Vector3<TYPE>(aMatrix.myRows[1]));
		temp[2] = aVector.Dot(Vector3<TYPE>(aMatrix.myRows[2]));
		return temp;
	}

	template<class TYPE>
	void Matrix33<TYPE>::SetRotationAngles(TYPE aBank,TYPE aPitch,TYPE aHeading) //this sets the inverse of the angles
	{
		TYPE ch = cos(aHeading);
		TYPE cb = cos(aBank);
		TYPE cp = cos(aPitch);
		TYPE sh = sin(aHeading);
		TYPE sb = sin(aBank);
		TYPE sp = sin(aPitch);
		
		_11 = ch*cb + sh*sp*sb;
		_12 = sb*cp;
		_13 = -sh*cb + ch*sp*sb;
		_21 = -ch*sb + sh*sp*cb;
		_22 = cb*cp;
		_23 = sb*sh + ch*sp*cb;
		_31 = sh*cp;
		_32 = -sp;
		_33 = ch*cp;
	}

	typedef Matrix33<float> Matrix33f;

}

#endif