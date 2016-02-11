#ifndef CU_VECTOR_HEADER
#define CU_VECTOR_HEADER

#include <math.h>
#pragma warning(disable : 4201)

namespace CommonLib
{
template<typename TYPE>
class Vector2
{
public:
	Vector2();
	Vector2(TYPE aX,TYPE aY);
	~Vector2();

	void Set(TYPE aX,TYPE aY);
	
	Vector2& operator+=(const Vector2& aVector);
	Vector2& operator-=(const Vector2& aVector);
	Vector2& operator*=(const Vector2& aVector);
	Vector2& operator/=(const Vector2& aVector);

	Vector2& operator*=(const TYPE& aType);
	Vector2& operator/=(const TYPE& aType);

	Vector2& operator=(const Vector2& aVector);

	bool operator==(const Vector2& aVector) const;

	const TYPE& operator[](const int aIndex) const;
	TYPE& operator[](const int aIndex);

	TYPE Length() const;
	TYPE Length2() const;
	Vector2<TYPE>& Normalize();

	TYPE Dot(const Vector2<TYPE>& aRightSideVector) const;
	TYPE DotPerp(const Vector2<TYPE>& aVector) const;

	Vector2<TYPE>& RotateDegrees(TYPE aRotation);
	Vector2<TYPE>& Rotate(TYPE aRotation);

	union 
	{
		TYPE myData[2];
		struct
		{
			union
			{
				TYPE myX;
				TYPE x;
				TYPE u;
			};
			union
			{
				TYPE myY;
				TYPE y;
				TYPE v;
			};
		};
	};
private:
};

template <class TYPE>
TYPE operator+(const TYPE& aLeftSideVector,const TYPE& aRightSideVector)
{
	TYPE returnValue(aLeftSideVector);
	returnValue+=aRightSideVector;
	return(returnValue);
}
template <class TYPE>
TYPE operator-(const TYPE& aLeftSideVector,const TYPE& aRightSideVector)
{
	TYPE returnValue(aLeftSideVector);
	returnValue-=aRightSideVector;
	return(returnValue);
}

template <class TYPE>
TYPE operator*(const TYPE& aLeftSideVector,const TYPE& aRightSideVector)
{
	TYPE returnValue(aLeftSideVector);
	returnValue*=aRightSideVector;
	return(returnValue);
}
template <class TYPE>
TYPE operator/(const TYPE& aLeftSideVector,const TYPE& aRightSideVector)
{
	TYPE returnValue(aLeftSideVector);
	returnValue/=aRightSideVector;
	return(returnValue);
}
template <class TYPE,class ValueType>
TYPE operator*(const TYPE& aLeftSideVector,const ValueType& aRightSideType)
{
	TYPE returnValue(aLeftSideVector);
	returnValue*=aRightSideType;
	return(returnValue);
}
template <class TYPE,class ValueType>
TYPE operator/(const TYPE& aLeftSideVector,const ValueType& aRightSideType)
{
	TYPE returnValue(aLeftSideVector);
	returnValue/=aRightSideType;
	return(returnValue);
}

template <class ValueType, class TYPE>
ValueType Dot(const TYPE& aLeftSideVector,const TYPE& aRightSideVector)
{
	return aLeftSideVector.Dot(aRightSideVector);
}

template<typename TYPE>
Vector2<TYPE>::Vector2()
:myX(0)
,myY(0)
{
}

template<typename TYPE>
Vector2<TYPE>::Vector2(TYPE aX,TYPE aY)
:myX(aX)
,myY(aY)
{
}

template<typename TYPE>
Vector2<TYPE>::~Vector2()
{
}

template<typename TYPE>
void Vector2<TYPE>::Set(TYPE aX,TYPE aY)
{
	myX = aX;
	myY = aY;
}

template<typename TYPE>
TYPE Vector2<TYPE>::Dot(const Vector2<TYPE>& aRightSideVector) const
{
	return x * aRightSideVector.x + y * aRightSideVector.y;
}

template<typename TYPE>
Vector2<TYPE>& Vector2<TYPE>::operator+=(const Vector2& aVector)
{
	myX += aVector.myX;
	myY += aVector.myY;
	return (*this);
}

template<typename TYPE>
Vector2<TYPE>& Vector2<TYPE>::operator-=(const Vector2& aVector)
{
	myX -= aVector.myX;
	myY -= aVector.myY;
	return (*this);
}

template<typename TYPE>
Vector2<TYPE>& Vector2<TYPE>::operator*=(const Vector2& aVector)
{
	myX *= aVector.myX;
	myY *= aVector.myY;
	return (*this);
}

template<typename TYPE>
Vector2<TYPE>& Vector2<TYPE>::operator/=(const Vector2& aVector)
{
	myX /= aVector.myX;
	myY /= aVector.myY;
	return (*this);
}

template<typename TYPE>
Vector2<TYPE>& Vector2<TYPE>::operator*=(const TYPE& aType)
{
	myX *= aType;
	myY *= aType;
	return (*this);
}

template<typename TYPE>
Vector2<TYPE>& Vector2<TYPE>::operator/=(const TYPE& aType)
{
	myX /= aType;
	myY /= aType;
	return (*this);
}

template<typename TYPE>
Vector2<TYPE>& Vector2<TYPE>::operator=(const Vector2& aVector)
{
	myX = aVector.myX;
	myY = aVector.myY;
	return (*this);
}

template<typename TYPE>
bool Vector2<TYPE>::operator==(const Vector2& aVector) const
{
	return (myX == aVector.myX && myY == aVector.myY);
}

template<typename TYPE>
TYPE Vector2<TYPE>::Length() const
{
	return sqrt(Length2());
}

template<typename TYPE>
TYPE Vector2<TYPE>::Length2() const
{
	return myX*myX + myY*myY;
}

template<typename TYPE>
Vector2<TYPE>& Vector2<TYPE>::Normalize()
{
	(*this)/=Length();
	return (*this);
}

template<typename TYPE>
const TYPE& Vector2<TYPE>::operator[](const int aIndex) const
{
	return myData[aIndex];
}

template<typename TYPE>
TYPE& Vector2<TYPE>::operator[](const int aIndex)
{
	return myData[aIndex];
}

template<typename TYPE>
Vector2<TYPE>& Vector2<TYPE>::RotateDegrees(TYPE aRotation)
{
	TYPE rotation = aRotation * 3.14f / 180.0f;
	return Rotate(rotation);
}

template <class TYPE>
TYPE Vector2<TYPE>::DotPerp(const Vector2<TYPE>& aVector) const
{
	return myData[0]*aVector[1]-myData[1]*aVector[0];
}

template<typename TYPE>
Vector2<TYPE>& Vector2<TYPE>::Rotate(TYPE aRotation)
{
	TYPE sn = sin(aRotation);
	TYPE cs = cos(aRotation);
	TYPE oldX = x;
	TYPE oldY = y;
	x = oldX * cs - oldY * sn;
	y = oldX * sn + oldY * cs;
	return (*this);
}

	typedef Vector2<float> Vector2f;
	typedef Vector2<double> Vector2d;
	typedef Vector2<int> Vector2i;

};

namespace CL = CommonLib;

#endif