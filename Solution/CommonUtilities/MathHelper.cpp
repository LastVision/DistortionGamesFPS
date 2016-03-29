#include "stdafx.h"
#include "MathHelper.h"

namespace CU
{
	namespace Math
	{
		int CapValue(int aMinValue, int aMaxValue, int aValueToCap)
		{
			if (aValueToCap > aMaxValue)
			{
				aValueToCap = aMaxValue;
			}
			else if (aValueToCap < aMinValue)
			{
				aValueToCap = aMinValue;
			}

			return aValueToCap;
		}

		float CapValue(const float aMinValue, const float aMaxValue, float aValueToCap)
		{
			if (aValueToCap > aMaxValue)
			{
				aValueToCap = aMaxValue;
			}
			else if (aValueToCap < aMinValue)
			{
				aValueToCap = aMinValue;
			}

			return aValueToCap;
		}

		float GetMaximumValueFromVector(const Vector2<float>& aVector)
		{
			if (aVector.x > aVector.y) return aVector.x;
			return aVector.y;
		}
		float GetMaximumValueFromVector(const Vector3<float>& aVector)
		{
			if (aVector.x > aVector.y && aVector.x > aVector.z) return aVector.x;
			if (aVector.y > aVector.x && aVector.y > aVector.z) return aVector.y;
			return aVector.z;
		}
		float GetMaximumValueFromVector(const Vector4<float>& aVector)
		{
			if (aVector.x > aVector.y && aVector.x > aVector.z && aVector.x > aVector.w) return aVector.x;
			if (aVector.y > aVector.x && aVector.y > aVector.z && aVector.y > aVector.w) return aVector.y;
			if (aVector.z > aVector.y && aVector.z > aVector.x && aVector.z > aVector.w) return aVector.z;
			return aVector.w;
		}

		float Saturate(const float aValue)
		{
			return CapValue(0.f, 1.f, aValue);
		}

		float RadToDegree(float aValue)
		{
			return aValue * 57.2957795f;
		}

		float DegreeToRad(float aValue)
		{
			return aValue * 0.0174532925f;
		}

		Vector2<float> RandomVector(const Vector2<float>& aMin, const Vector2<float>& aMax)
		{
			return Vector2<float>(RandomRange<float>(aMin.x, aMax.x)
				, RandomRange<float>(aMin.y, aMax.y));
		}

		Vector3<float> RandomVector(const Vector3<float>& aMin, const Vector3<float>& aMax)
		{
			return Vector3<float>(RandomRange<float>(aMin.x, aMax.x)
				, RandomRange<float>(aMin.y, aMax.y)
				, RandomRange<float>(aMin.z, aMax.z));
		}

		Vector4<float> RandomVector(const Vector4<float>& aMin, const Vector4<float>& aMax)
		{
			return Vector4<float>(RandomRange<float>(aMin.x, aMax.x)
				, RandomRange<float>(aMin.y, aMax.y)
				, RandomRange<float>(aMin.z, aMax.z)
				, RandomRange<float>(aMin.w, aMax.w));
		}

		unsigned int ClosestPowerOfTwo(unsigned int aValue)
		{
			unsigned int n = aValue;
			n--;
			n |= n >> 1;   
			n |= n >> 2;
			n |= n >> 4;
			n |= n >> 8;
			n |= n >> 16;
			n++;
			return n;
		}

		bool DistanceBetweenLessThanOrEqualToEpsilon(const CU::Vector3<float>& aVector1, const CU::Vector3<float>& aVector2, float anEpsilon)
		{
			float distance2 = CU::Length2(aVector2 - aVector1);
			return distance2 <= anEpsilon * anEpsilon;
		}
	}
}