#ifndef CU_VECTOR4_HEADER
#define CU_VECTOR4_HEADER

#include "Vector3.h"
#include <xmmintrin.h>
#include <intrin.h>

#ifndef _align16_
#define _align16_ __declspec(align(16) )
#endif

#pragma warning(disable : 4201)

namespace CommonLib
{
	template<class TYPE>
	class Vector4
	{
	public:
		Vector4();
		Vector4(const Vector2<TYPE>& aVector);
		Vector4(const Vector3<TYPE>& aVector);
		Vector4(const Vector3<TYPE>& aVector,float aValue);
		Vector4(TYPE aX, TYPE aY, TYPE aZ, TYPE aW = 1.0f);
		Vector4(const TYPE* aData);
		~Vector4();

		void Set(TYPE aX, TYPE aY, TYPE aZ, TYPE aW = 1.0f);
		
		Vector4& operator+=(const Vector4& aVector);
		Vector4& operator-=(const Vector4& aVector);
		Vector4& operator*=(const Vector4& aVector);
		Vector4& operator/=(const Vector4& aVector);

		Vector4& operator*=(const TYPE& aType);
		Vector4& operator/=(const TYPE& aType);


		Vector4& operator=(const Vector4& aVector);
		Vector4& operator=(const TYPE* aData);

		bool operator==(const Vector4& aVector) const;

		TYPE operator()(const int aIndex) const;
		TYPE& operator()(const int aIndex);

		const TYPE& operator[](const int aIndex) const;
		TYPE& operator[](const int aIndex);

		TYPE Length() const;
		TYPE Length2() const;
		Vector4& Normalize();

		TYPE Dot(const Vector4& aRightSideVector) const;
		union 
		{
			__m128 myAlignedData;
			struct 
			{
				TYPE myData[4];
			};
			struct
			{
				Vector3<TYPE> xyz;
				TYPE w;
			};
			struct
			{
				Vector2<TYPE> xy;
				Vector2<TYPE> zw;
			};
			struct
			{
				union
				{
					TYPE myX;
					TYPE x;
					TYPE r;
				};
				union
				{
					TYPE myY;
					TYPE y;
					TYPE g;
				};
				union
				{
					TYPE myZ;
					TYPE z;
					TYPE b;
				};
				union
				{
					TYPE myW;
					TYPE w;
					TYPE a;
				};
			};
		};
	private:
	};

	template<class TYPE>
	Vector4<TYPE>::Vector4()
	{
	}

	template<class TYPE>
	Vector4<TYPE>::Vector4(const Vector2<TYPE>& aVector)
	:myX(aVector.myX)
	,myY(aVector.myY)
	,myZ(1.0f)
	,myW(1.0f)
	{
	}

	template<class TYPE>
	Vector4<TYPE>::Vector4(const Vector3<TYPE>& aVector)
	:myX(aVector.myX)
	,myY(aVector.myY)
	,myZ(aVector.myZ)
	,myW(1.0f)
	{
	}

	template<class TYPE>
	Vector4<TYPE>::Vector4(const Vector3<TYPE>& aVector,float aValue)
	:myX(aVector.myX)
	,myY(aVector.myY)
	,myZ(aVector.myZ)
	,myW(aValue)
	{
	}

	template<class TYPE>
	Vector4<TYPE>::Vector4(TYPE aX, TYPE aY, TYPE aZ, TYPE aW)
	:myX(aX)
	,myY(aY)
	,myZ(aZ)
	,myW(aW)
	{
	}

	template<class TYPE>
	Vector4<TYPE>::Vector4(const TYPE* aData)
	{
		(*this) = aData;
	}

	template<class TYPE>
	Vector4<TYPE>::~Vector4()
	{
	}

	template<class TYPE>
	void Vector4<TYPE>::Set(TYPE aX, TYPE aY, TYPE aZ, TYPE aW)
	{
		myX = aX;
		myY = aY;
		myZ = aZ;
		myW = aW;
	}

	template<class TYPE>
	Vector4<TYPE>& Vector4<TYPE>::operator+=(const Vector4<TYPE>& aVector)
	{
		myData[0] += aVector.myData[0];
		myData[1] += aVector.myData[1];
		myData[2] += aVector.myData[2];
		myData[3] += aVector.myData[3];
		return (*this);
	}

	template<class TYPE>
	Vector4<TYPE>& Vector4<TYPE>::operator-=(const Vector4<TYPE>& aVector)
	{
		myData[0] -= aVector.myData[0];
		myData[1] -= aVector.myData[1];
		myData[2] -= aVector.myData[2];
		myData[3] -= aVector.myData[3];
		return (*this);
	}

	template<class TYPE>
	Vector4<TYPE>& Vector4<TYPE>::operator*=(const Vector4<TYPE>& aVector)
	{
		myAlignedData =  _mm_mul_ps(myAlignedData,aVector.myAlignedData);
		return (*this);
	}

	template<class TYPE>
	Vector4<TYPE>& Vector4<TYPE>::operator/=(const Vector4<TYPE>& aVector)
	{
		myAlignedData = _mm_div_ps(myAlignedData,aVector.myAlignedData);
		return (*this);
	}

	template<class TYPE>
	Vector4<TYPE>& Vector4<TYPE>::operator*=(const TYPE& aType)
	{
		__m128 vector2 = _mm_set_ps(aType,aType,aType,aType);  
		myAlignedData = _mm_mul_ps(myAlignedData,vector2);
		return (*this);
	}

	template<class TYPE>
	Vector4<TYPE>& Vector4<TYPE>::operator/=(const TYPE& aType)
	{
		__m128 vector2 = _mm_set_ps(aType,aType,aType,aType);  
		myAlignedData =  _mm_div_ps(myAlignedData,vector2);
		return (*this);
	}

	template<class TYPE>
	Vector4<TYPE>& Vector4<TYPE>::operator=(const Vector4<TYPE>& aVector)
	{
		myX = aVector.myX;
		myY = aVector.myY;
		myZ = aVector.myZ;
		myW = aVector.myW;
		return (*this);
	}

	template<class TYPE>
	Vector4<TYPE>& Vector4<TYPE>::operator=(const TYPE* aData)
	{
		myData[0] = aData[0];
		myData[1] = aData[1];
		myData[2] = aData[2];
		myData[3] = aData[3];
		return (*this);
	}


	template<class TYPE>
	bool Vector4<TYPE>::operator==(const Vector4<TYPE>& aVector) const
	{
		return myX == aVector.myX &&
		myY == aVector.myY &&
		myZ == aVector.myZ &&
		myW == aVector.myW;
	}

	template<class TYPE>
	const TYPE& Vector4<TYPE>::operator[](const int aIndex) const
	{
		assert(aIndex >= 0 && aIndex < 4);
		return myData[aIndex];
	}

	template<class TYPE>
	TYPE& Vector4<TYPE>::operator[](const int aIndex)
	{
		assert(aIndex >= 0 && aIndex < 4);
		return myData[aIndex];
	}

	template<class TYPE>
	TYPE Vector4<TYPE>::operator()(const int aIndex) const
	{
		assert(aIndex >= 0 && aIndex < 4);
		return myData[aIndex];
	}

	template<class TYPE>
	TYPE& Vector4<TYPE>::operator()(const int aIndex)
	{
		assert(aIndex >= 0 && aIndex < 4);
		return myData[aIndex];
	}

	template<class TYPE>
	TYPE Vector4<TYPE>::Length() const
	{
		return sqrt(Length2());
	}

	template<class TYPE>
	TYPE Vector4<TYPE>::Length2() const
	{
		return (x * x + y*y + z*z + w*w);
	}

	template<class TYPE>
	Vector4<TYPE>& Vector4<TYPE>::Normalize()
	{
		(*this) /= Length();
		return (*this);
	}

	template<class TYPE>
	TYPE Vector4<TYPE>::Dot(const Vector4<TYPE>& aRightSideVector) const
	{
		_align16_ float temp[4];
		_mm_store_ps(&temp[0], _mm_mul_ps(myAlignedData,aRightSideVector.myAlignedData));
		return temp[0] + temp[1] + temp[2] + temp[3];
	}

	typedef Vector4<float> Vector4f;
	typedef Vector4<int> Vector4i;
}

namespace CL = CommonLib;

#endif