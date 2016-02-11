#ifndef MATRIX44_HEADER
#define MATRIX44_HEADER

#include "Vector4.h"
#include "Matrix33.h"

namespace CommonLib
{
	template<class TYPE>
	class Matrix44
	{
	public:
		Matrix44();
		~Matrix44();

		Matrix44(const Matrix44& aMatrix);
		Matrix44(TYPE a11,TYPE a12,TYPE a13,TYPE a14,
				TYPE a21,TYPE a22,TYPE a23,TYPE a24,
				TYPE a31,TYPE a32,TYPE a33,TYPE a34,
				TYPE a41,TYPE a42,TYPE a43,TYPE a44);
		Matrix44(const TYPE* data);

		void SetToIdentity();

		void Set(const Matrix33<TYPE>& aMatrix33,const Vector4<TYPE>& aVector4);
		void Set(TYPE a11,TYPE a12,TYPE a13,TYPE a14,
				TYPE a21,TYPE a22,TYPE a23,TYPE a24,
				TYPE a31,TYPE a32,TYPE a33,TYPE a34,
				TYPE a41,TYPE a42,TYPE a43,TYPE a44);
		void Set(TYPE* data);

		void SetFromVolatile(volatile Matrix44& aMatrix);

		void CreateProjectionMatrixLH(TYPE aNearZ, TYPE aFarZ, TYPE aAspectRatio,TYPE aFovAngle);
		void CreateOrthographicMatrixLH(TYPE aWidth,TYPE aHeight,TYPE aNear,TYPE aFar);

		void Set33(const Matrix33<TYPE>& aMatrix33);

		void SetTranslation(const Vector4<TYPE>& aVector4);

		inline const TYPE& operator()(const int aRow,const int aColumn) const;
		inline TYPE& operator()(const int aRow,const int aColumn);

		inline void GetCofactor(Matrix33<TYPE>& aMatrixOut, int aRow, int aColumn);
		void RealInvert(TYPE aDeterminant);
		void Inverse();
		void Transpose();

		Matrix44 GetInverse() const;
		Matrix44 GetRealInverse() const;
		Matrix44 GetTranspose();

		TYPE GetDeterminant();

		Vector4<TYPE> GetTranslation() const;

		void SetVolatile(const Matrix44& aRightMatrix) volatile;

		Matrix44& operator=(const Matrix44& aRightMatrix);
		Matrix44& operator=(const TYPE* data);
		Matrix44& operator*=(const Matrix44& aRightMatrix);
		Matrix44& operator*=(const Matrix33<TYPE>& aRightMatrix);

		void GetMatrix33(Matrix33<TYPE>& aOutMatrix) const;
		Matrix33<TYPE> Get33() const;

		union
		{
			__m128 myAlignedData[4];
			struct
			{
				TYPE myData[4*4];
			};
			struct
			{
				TYPE myRows[4][4];
			};
			struct
			{
				CL::Vector4<TYPE> myVectorRows[4];
			};
			struct
			{
				TYPE	_11,_12,_13,_14,
						_21,_22,_23,_24,
						_31,_32,_33,_34,
						_41,_42,_43,_44;
			};
		};
	private:
	};

	template<class TYPE>
	Matrix44<TYPE>::Matrix44()
	{
	}

	template<class TYPE>
	Matrix44<TYPE>::~Matrix44()
	{
	}

	template<class TYPE>
	void Matrix44<TYPE>::SetFromVolatile(volatile Matrix44& aMatrix)
	{
		for(int i = 0;i < 16;++i)
		{
			myData[i] = aMatrix.myData[i];
		}
	}

	template<class TYPE>
	Matrix44<TYPE>::Matrix44(const Matrix44& aMatrix)
	{
		for(int i = 0;i < 16;++i)
		{
			myData[i] = aMatrix.myData[i];
		}
	}

	template<class TYPE>
	Matrix44<TYPE>::Matrix44(TYPE a11,TYPE a12,TYPE a13,TYPE a14,
				TYPE a21,TYPE a22,TYPE a23,TYPE a24,
				TYPE a31,TYPE a32,TYPE a33,TYPE a34,
				TYPE a41,TYPE a42,TYPE a43,TYPE a44)
	{
		Set(a11,a12,a13,a14,
			a21,a22,a23,a24,
			a31,a32,a33,a34,
			a41,a42,a43,a44);
	}

	template<class TYPE>
	Matrix44<TYPE>::Matrix44(const TYPE* data)
	{
		(*this) = data;
	}

	template<class TYPE>
	void Matrix44<TYPE>::SetToIdentity()
	{
		_11 = 1;
		_12 = 0;
		_13 = 0;
		_14 = 0;

		_21 = 0;
		_22 = 1;
		_23 = 0;
		_24 = 0;

		_31 = 0;
		_32 = 0;
		_33 = 1;
		_34 = 0;

		_41 = 0;
		_42 = 0;
		_43 = 0;
		_44 = 1;
	}

	template<class TYPE>
	void Matrix44<TYPE>::CreateProjectionMatrixLH(TYPE aNearZ, TYPE aFarZ, TYPE aAspectRatio,TYPE aFovAngle)
	{
		//DirectX::XMMATRIX matrix = DirectX::XMMatrixPerspectiveFovLH(aFovAngle,aAspectRatio,aNearZ,aFarZ);
		TYPE    SinFov;
		TYPE    CosFov;
		TYPE    Height;
		TYPE    Width;

		SinFov = sin(0.5f * aFovAngle);
		CosFov = cos(0.5f * aFovAngle);

		Height = CosFov / SinFov;
		Width = Height / aAspectRatio;

		TYPE scaling = aFarZ / (aFarZ - aNearZ);

		this->myVectorRows[0].Set(Width, 0.0f, 0.0f, 0.0f);
		this->myVectorRows[1].Set(0.0f, Height, 0.0f, 0.0f);
		this->myVectorRows[2].Set(0.0f, 0.0f, scaling, -scaling * aNearZ);
		this->myVectorRows[3].Set(0.0f, 0.0f, 1.0f, 0.0f);
	}

	template<class TYPE>
	void Matrix44<TYPE>::CreateOrthographicMatrixLH(TYPE aWidth,TYPE aHeight,TYPE aNear,TYPE aFar)
	{
		TYPE range = 1.0f / (aFar-aNear);
		this->myVectorRows[0].Set(2.0f / aWidth, 0.0f, 0.0f, 0.0f);
		this->myVectorRows[1].Set(0.0f, 2.0f / aHeight, 0.0f, 0.0f);
		this->myVectorRows[2].Set(0.0f, 0.0f, range, -range * aNear);
		this->myVectorRows[3].Set(0.0f, 0.0f, 0.0f, 1.0f);
	}

	template<class TYPE>
	void Matrix44<TYPE>::Set(const Matrix33<TYPE>& aMatrix33,const Vector4<TYPE>& aVector4)
	{
		Set33(aMatrix33);
		SetTranslation(aVector4);
	}

	template<class TYPE>
	void Matrix44<TYPE>::Set(TYPE a11,TYPE a12,TYPE a13,TYPE a14,
				TYPE a21,TYPE a22,TYPE a23,TYPE a24,
				TYPE a31,TYPE a32,TYPE a33,TYPE a34,
				TYPE a41,TYPE a42,TYPE a43,TYPE a44)
	{
		_11 = a11;
		_12 = a12;
		_13 = a13;
		_14 = a14;

		_21 = a21;
		_22 = a22;
		_23 = a23;
		_24 = a24;

		_31 = a31;
		_32 = a32;
		_33 = a33;
		_34 = a34;

		_41 = a41;
		_42 = a42;
		_43 = a43;
		_44 = a44;
	}

	template<class TYPE>
	void Matrix44<TYPE>::Set(TYPE* data)
	{
		(*this) = data;
	}

	template<class TYPE>
	void Matrix44<TYPE>::Set33(const Matrix33<TYPE>& aMatrix33)
	{
		for(int row = 0;row < 3;row++)
		{
			for(int column = 0;column < 3;column++)
			{
				myRows[row][column] = aMatrix33(row,column);
			}
		}
	}

	template<class TYPE>
	void Matrix44<TYPE>::SetTranslation(const Vector4<TYPE>& aVector4)
	{
		_14 = aVector4.x;
		_24 = aVector4.y;
		_34 = aVector4.z;
		_44 = aVector4.w;
	}

	template<class TYPE>
	const TYPE& Matrix44<TYPE>::operator()(const int aRow,const int aColumn) const
	{
		return myRows[aRow][aColumn];
	}

	template<class TYPE>
	TYPE& Matrix44<TYPE>::operator()(const int aRow,const int aColumn)
	{
		return myRows[aRow][aColumn];
	}

	template<class TYPE>
	Vector4<TYPE> Matrix44<TYPE>::GetTranslation() const
	{
		return Vector4<TYPE>(_14,_24,_34,_44);
	}

	template<class TYPE>
	void Matrix44<TYPE>::GetCofactor(Matrix33<TYPE>& aMatrixOut, int aRow, int aColumn)
	{
		int cX = 0;
		for(int x = 0;x < 3;++x)
		{
			if(cX == aRow)
			{
				++cX;
			}
			int cY = 0;
			for(int y = 0;y < 3;++y)
			{
				if(cY == aColumn)
				{
					++cY;
				}
				aMatrixOut(x,y) = (*this)(cX,cY);
				++cY;
			}
			++cX;
		}
	}

	template<class TYPE>
	Matrix44<TYPE> Matrix44<TYPE>::GetTranspose()
	{
		Matrix44<TYPE> tempMatrix(myData);
		tempMatrix.Transpose();
		return tempMatrix;
	}

	template<class TYPE>
	Matrix44<TYPE> Matrix44<TYPE>::GetInverse() const
	{
		Matrix44<TYPE> tempMatrix(myData);
		tempMatrix.Inverse();
		return tempMatrix;
	}

	template<class TYPE>
	Matrix44<TYPE> Matrix44<TYPE>::GetRealInverse() const
	{
		Matrix44<TYPE> tempMatrix(myData);
		tempMatrix.RealInvert(tempMatrix.GetDeterminant());
		return tempMatrix;
	}

	template<class TYPE>
	void Matrix44<TYPE>::RealInvert(TYPE aDeterminant)
	{
		Matrix44<TYPE> inverse;
		for(int row = 0;row < 4;row++)
		{
			for(int column = 0;column < 4;column++)
			{
				Matrix33<TYPE> cofactor;
			
				GetCofactor(cofactor,row,column);

				inverse(row,column) = cofactor.GetDeterminant() * 1-2*((column+row) % 2);
			}
		}
		inverse.Transpose();
		for(int i = 0;i < 16;i++)
		{
			inverse.myData[i]/= aDeterminant;
		}
		(*this) = inverse;
	}

	template<class TYPE>
	void Matrix44<TYPE>::Inverse()
	{
		Vector4<TYPE> translation = GetTranslation();
		SetTranslation(Vector4<TYPE>(0.0f,0.0f,0.0f));
		translation*=-1;
		translation.w = 1;
		(*this).Transpose();
		translation *= (*this);
		_14 = translation.x;
		_24 = translation.y;
		_34 = translation.z;
		_44 = translation.w;
	}

	template<class TYPE>
	void Matrix44<TYPE>::GetMatrix33(Matrix33<TYPE>& aOutMatrix) const
	{
		aOutMatrix._11 = _11;
		aOutMatrix._12 = _12;
		aOutMatrix._13 = _13;

		aOutMatrix._21 = _21;
		aOutMatrix._22 = _22;
		aOutMatrix._23 = _23;

		aOutMatrix._31 = _31;
		aOutMatrix._32 = _32;
		aOutMatrix._33 = _33;
	}

	template<class TYPE>
	Matrix33<TYPE> Matrix44<TYPE>::Get33() const
	{
		Matrix33<TYPE> newMatrix;
		GetMatrix33(newMatrix);
		return newMatrix;
	}

	template<class TYPE>
	void Matrix44<TYPE>::Transpose()
	{
		Matrix44<TYPE> temp;
		temp._11 = _11;
		temp._21 = _12;
		temp._31 = _13;
		temp._41 = _14;

		temp._12 = _21;
		temp._22 = _22;
		temp._32 = _23;
		temp._42 = _24;

		temp._13 = _31;
		temp._23 = _32;
		temp._33 = _33;
		temp._43 = _34;

		temp._14 = _41;
		temp._24 = _42;
		temp._34 = _43;
		temp._44 = _44;
		(*this) = temp;
	}

	template<class TYPE>
	TYPE Matrix44<TYPE>::GetDeterminant()
	{
		float determinant = 0;
		int row = 0;
		for(int column = 0;column < 4;column++)
		{
			float currentValue = myRows[row][column];
			Matrix33<TYPE> currentMatrix;
			GetCofactor(currentMatrix,row,column);
			currentValue *= currentMatrix.GetDeterminant();
			determinant += currentValue * 1-2*((column+row) % 2);
		}
		return determinant;
	}

	template<class TYPE>
	Matrix44<TYPE>& Matrix44<TYPE>::operator=(const TYPE* data)
	{
		for(int i = 0;i < 16;i++)
		{
			myData[i] = data[i];
		}
		return (*this);
	}

	template<class TYPE>
	void Matrix44<TYPE>::SetVolatile(const Matrix44<TYPE>& aRightMatrix) volatile
	{
		for(int i = 0;i < 16;i++)
		{
			myData[i] = aRightMatrix.myData[i];
		}
	}

	template<class TYPE>
	Matrix44<TYPE>& Matrix44<TYPE>::operator=(const Matrix44<TYPE>& aRightMatrix)
	{
		(*this) = aRightMatrix.myData;
		return (*this);
	}

	template<class TYPE>
	Matrix44<TYPE>& Matrix44<TYPE>::operator*=(const Matrix44<TYPE>& aRightMatrix)
	{
		Matrix44 temp;
		for(int row = 0;row < 4;++row)
		{
			for(int column = 0;column < 4;++column)
			{
				
				Vector4<TYPE> vec1 = this->myRows[row];
				Vector4<TYPE> vec2(aRightMatrix(0,column),
									aRightMatrix(1,column),
									aRightMatrix(2,column),
									aRightMatrix(3,column));
				temp.myRows[row][column] = vec1.Dot(vec2);
			}
		}
		(*this) = temp;
		return (*this);
	}

	template<class TYPE>
	Vector4<TYPE> operator*(const Matrix44<TYPE>& aMatrix, const Vector4<TYPE>& aVector)
	{
		Vector4<TYPE> returnVector;
		for(int row = 0;row < 4;++row)
		{
			Vector4<TYPE> temp(aMatrix.myRows[row]);
			returnVector(row) = temp.Dot(aVector);
		}
		return returnVector;
	}

	template<class TYPE>
	Vector3<TYPE> operator*(const Matrix44<TYPE>& aMatrix, const Vector3<TYPE>& aVector)
	{
		Vector4<TYPE> returnVector;
		Vector4<TYPE> tempVector(aVector);
		for(int row = 0;row < 4;++row)
		{
			Vector4<TYPE> temp(aMatrix.myRows[row]);
			returnVector(row) = temp.Dot(tempVector);
		}
		return returnVector.xyz;
	}

	template<class TYPE>
	Vector3<TYPE> operator*(const Vector3<TYPE>& aVector,const Matrix44<TYPE>& aMatrix)
	{
		Vector4<TYPE> returnVector;
		Vector4<TYPE> tempVector(aVector);
		for(int row = 0;row < 4;++row)
		{
			Vector4<TYPE> temp(aMatrix.myRows[row]);
			returnVector(row) = temp.Dot(tempVector);
		}
		return returnVector.xyz;
	}

	template<class TYPE>
	Vector4<TYPE>& operator*=(Vector4<TYPE>& aVector, const Matrix44<TYPE>& aMatrix)
	{
		Vector4<TYPE> tempVector(aVector);
		for(int row = 0;row < 4;++row)
		{
			Vector4<TYPE> temp(aMatrix.myRows[row]);
			tempVector(row) = temp.Dot(aVector);
		}
		aVector = tempVector;
		return aVector;
	}

	template<class TYPE>
	Matrix44<TYPE>& Matrix44<TYPE>::operator*=(const Matrix33<TYPE>& aRightMatrix)
	{
		Matrix33<TYPE> temp33;
		GetMatrix33(temp33);
		temp33 *= aRightMatrix;
		Set33(temp33);
		return (*this);
	}

	typedef Matrix44<float> Matrix44f;
	typedef Matrix44<int> Matrix44i;
	typedef Matrix44<double> Matrix44d;
}

#endif