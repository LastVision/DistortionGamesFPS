#ifndef MATH_HELPER_HEADER
#define MATH_HELPER_HEADER
#include <cstdlib>
#include "Vector.h"
#define _USE_MATH_DEFINES
#include <math.h>

#define CU_PI static_cast<float>(M_PI)
#define CU_PI_DIV_2 static_cast<float>(M_PI) / 2.f
#define CU_PI_DIV_4 static_cast<float>(M_PI) / 4.f

namespace CU
{
	namespace Math
	{
		int CapValue(int aMinValue, int aMaxValue, int aValueToCap);
		float CapValue(const float aMinValue, const float aMaxValue, float aValueToCap);

		float GetMaximumValueFromVector(const Vector2<float>& aVector);
		float GetMaximumValueFromVector(const Vector3<float>& aVector);
		float GetMaximumValueFromVector(const Vector4<float>& aVector);

		float Saturate(const float aValue);

		float RadToDegree(float aValue);
		float DegreeToRad(float aValue);

		template <typename T>
		inline T RandomRange(const T aMinValue, const T aMaxValue)
		{
			float alpha = static_cast<float>(rand() % 10000) / 10000.f;
			return Lerp<T>(aMinValue, aMaxValue, alpha);
		}

		template <typename T>
		inline T Lerp(const T aStartValue, const T aEndValue, float aAlpha)
		{
			aAlpha = Saturate(aAlpha);
			return static_cast<T>(aStartValue + (aEndValue - aStartValue) * aAlpha);
		}

		template <typename T>
		inline T Remap(T aValue, T aOriginalMin, T aOriginalMax, T aNewMin, T aNewMax)
		{
			DL_ASSERT_EXP(aValue >= aOriginalMin, "Value too small");
			DL_ASSERT_EXP(aValue <= aOriginalMax, "Value too big");
			return (aValue - aOriginalMin) / (aOriginalMax - aOriginalMin) * (aNewMax - aNewMin) + aNewMin;
		}

		Vector2<float> RandomVector(const Vector2<float>& aMin, const Vector2<float>& aMax);
		Vector3<float> RandomVector(const Vector3<float>& aMin, const Vector3<float>& aMax);
		Vector4<float> RandomVector(const Vector4<float>& aMin, const Vector4<float>& aMax);
		
		unsigned int ClosestPowerOfTwo(unsigned int aValue);

		bool DistanceBetweenLessThanOrEqualToEpsilon(const CU::Vector3<float>& aVector1, const CU::Vector3<float>& aVector2, float anEpsilon);
	}
}

#endif