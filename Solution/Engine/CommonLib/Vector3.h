#ifndef CU_VECTOR3_HEADER
#define CU_VECTOR3_HEADER

#include "assert.h"
#include "Vector.h"
#include <xmmintrin.h>
#include <intrin.h>

#ifndef _align16_
#define _align16_ __declspec(align(16) )
#endif

#pragma warning(disable : 4201)

namespace CommonLib
{
	template<typename TYPE>
	class Vector3
	{
	public:
		Vector3();
		Vector3(Vector2<TYPE> aVector);
		Vector3(TYPE aX, TYPE aY, TYPE aZ);
		Vector3(const TYPE* someData);
		~Vector3();

		void Set(TYPE aX, TYPE aY, TYPE aZ);
		
		Vector3& operator+=(const Vector3& aVector);
		Vector3& operator-=(const Vector3& aVector);
		Vector3& operator*=(const Vector3& aVector);
		Vector3& operator/=(const Vector3& aVector);

		Vector3& operator*=(const TYPE& aType);
		Vector3& operator/=(const TYPE& aType);


		Vector3& operator=(const Vector3& aVector);

		bool operator==(const Vector3& aVector) const;

		const TYPE& operator[](const int aIndex) const;
		TYPE& operator[](const int aIndex);

		TYPE Length() const;
		TYPE Length2() const;
		Vector3& Normalize();

		Vector3 Cross(const Vector3& aVector) const;
		TYPE Dot(const Vector3& aRightSideVector) const;
		union 
		{
			TYPE myData[3];
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
			};
		};
	private:
	};

	template<typename TYPE>
	Vector3<TYPE> Cross(const Vector3<TYPE>& aLeftVector, const Vector3<TYPE>& aRightVector)
	{
		return aLeftVector.Cross(aRightVector);
	}

	template<typename TYPE>
	Vector3<TYPE>::Vector3()
	:myX(0.0f)
	,myY(0.0f)
	,myZ(0.0f)
	{
	}

	template<typename TYPE>
	Vector3<TYPE>::Vector3(TYPE aX,TYPE aY, TYPE aZ)
	:myX(aX)
	,myY(aY)
	,myZ(aZ)
	{
	}

	template<typename TYPE>
	Vector3<TYPE>::Vector3(Vector2<TYPE> aVector)
	:myX(aVector.x)
	,myY(aVector.y)
	,myZ(1.0f)
	{
	}

	template<typename TYPE>
	Vector3<TYPE>::Vector3(const TYPE* someData)
	:myX(someData[0]),
	myY(someData[1]),
	myZ(someData[2])
	{

	}

	template<typename TYPE>
	Vector3<TYPE>::~Vector3()
	{
	}

	template<typename TYPE>
	void Vector3<TYPE>::Set(TYPE aX,TYPE aY, TYPE aZ)
	{
		myX = aX;
		myY = aY;
		myZ = aZ;
	}

	template<typename TYPE>
	Vector3<TYPE>& Vector3<TYPE>::operator+=(const Vector3& aVector)
	{
		myX += aVector.myX;
		myY += aVector.myY;
		myZ += aVector.myZ;
		return (*this);
	}

	template<typename TYPE>
	Vector3<TYPE> operator-(const Vector3<TYPE> aLeftVector, const Vector3<TYPE> aRightVector)
	{
		Vector3<TYPE> result(aLeftVector);
		return result -= aRightVector;
	}

	template<typename TYPE>
	Vector3<TYPE>& Vector3<TYPE>::operator-=(const Vector3& aVector)
	{
		myX -= aVector.myX;
		myY -= aVector.myY;
		myZ -= aVector.myZ;
		return (*this);
	}

	template<typename TYPE>
	Vector3<TYPE>& Vector3<TYPE>::operator*=(const Vector3& aVector)
	{
		__m128 vector1 = _mm_set_ps(myData[0],myData[1],myData[2],0);  
		__m128 vector2 = _mm_set_ps(aVector.myData[0],aVector.myData[1],aVector.myData[2],0);  
		_align16_ float temp[4];
		_mm_store_ps(&temp[0], _mm_mul_ps(vector1,vector2));
		myData[0] = temp[3];
		myData[1] = temp[2];
		myData[2] = temp[1];
		return (*this);
	}

	template<typename TYPE>
	Vector3<TYPE>& Vector3<TYPE>::operator/=(const Vector3& aVector)
	{
		__m128 vector1 = _mm_set_ps(myData[0],myData[1],myData[2],0);  
		__m128 vector2 = _mm_set_ps(aVector.myData[0],aVector.myData[1],aVector.myData[2],0);  
		_align16_ float temp[4];
		_mm_store_ps(&temp[0], _mm_div_ps(vector1,vector2));
		myData[0] = temp[3];
		myData[1] = temp[2];
		myData[2] = temp[1];
		return (*this);
	}

	template<typename TYPE>
	Vector3<TYPE>& Vector3<TYPE>::operator*=(const TYPE& aType)
	{
		__m128 vector1 = _mm_set_ps(myData[0],myData[1],myData[2],0);  
		__m128 vector2 = _mm_set_ps(aType,aType,aType,aType);  
		_align16_ float temp[4];
		_mm_store_ps(&temp[0], _mm_mul_ps(vector1,vector2));
		myData[0] = temp[3];
		myData[1] = temp[2];
		myData[2] = temp[1];
		return (*this);
	}

	template<typename TYPE>
	Vector3<TYPE>& Vector3<TYPE>::operator/=(const TYPE& aType)
	{
		__m128 vector1 = _mm_set_ps(myData[0],myData[1],myData[2],0);  
		__m128 vector2 = _mm_set_ps(aType,aType,aType,aType);  
		_align16_ float temp[4];
		_mm_store_ps(&temp[0], _mm_div_ps(vector1,vector2));
		myData[0] = temp[3];
		myData[1] = temp[2];
		myData[2] = temp[1];
		return (*this);
	}

	template<typename TYPE>
	Vector3<TYPE>& Vector3<TYPE>::operator=(const Vector3<TYPE>& aVector)
	{
		myX = aVector.myX;
		myY = aVector.myY;
		myZ = aVector.myZ;
		return (*this);
	}

	template<typename TYPE>
	bool Vector3<TYPE>::operator==(const Vector3& aVector) const
	{
		return myX == aVector.myX && myY == aVector.myY && myZ == aVector.myZ;
	}

	template<typename TYPE>
	const TYPE& Vector3<TYPE>::operator[](const int aIndex) const
	{
		assert(aIndex >= 0 && aIndex < 3);
		return myData[aIndex];
	}

	template<typename TYPE>
	TYPE& Vector3<TYPE>::operator[](const int aIndex)
	{
		assert(aIndex >= 0 && aIndex < 3);
		return myData[aIndex];
	}

	template<typename TYPE>
	TYPE Vector3<TYPE>::Length() const
	{
		return sqrt(Length2());
	}

	template<typename TYPE>
	TYPE Vector3<TYPE>::Length2() const
	{
		return x*x+y*y+z*z;
	}

	template<typename TYPE>
	Vector3<TYPE>& Vector3<TYPE>::Normalize()
	{
		(*this) /= Length();
		return (*this);
	}

	template<typename TYPE>
	TYPE Vector3<TYPE>::Dot(const Vector3& aRightSideVector) const
	{
		__m128 vector1 = _mm_set_ps(myData[0],myData[1],myData[2],0);  
		__m128 vector2 = _mm_set_ps(aRightSideVector.myData[0],aRightSideVector.myData[1],aRightSideVector.myData[2],0);  
		_align16_ float temp[4];
		_mm_store_ps(&temp[0], _mm_mul_ps(vector1,vector2));
		return temp[3] + temp[2] + temp[1];
		//return x*aRightSideVector.x + y*aRightSideVector.y + z*aRightSideVector.z;
	}

	template<typename TYPE>
	Vector3<TYPE> Vector3<TYPE>::Cross(const Vector3<TYPE>& aVector) const
	{
		Vector3<TYPE> tempVector;
		tempVector.x = y*aVector.z - z*aVector.y;
		tempVector.y = z*aVector.x - x*aVector.z;
		tempVector.z = x*aVector.y - y*aVector.x;
		return tempVector;
	}

	typedef Vector3<float> Vector3f;
	typedef Vector3<int> Vector3i;

	template<typename TYPE>
	TYPE Dot(const Vector3<TYPE>& aVector1,const Vector3<TYPE>& aVector2)
	{
		return aVector1.x*aVector2.x+aVector1.y*aVector2.y+aVector1.z*aVector2.z;
	}
}

namespace CL = CommonLib;

#endif