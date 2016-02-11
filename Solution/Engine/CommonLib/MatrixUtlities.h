#ifndef COMMON_LIB_MATRIX_UTIL_HEADER
#define COMMON_LIB_MATRIX_UTIL_HEADER

#include "Angle3.h"

namespace MatrixUtils
{
	template<class Type>
	void GetMatrixFromAngles(CL::Angle3<Type> someAngles, CL::Matrix33<Type>& outMatrix)
	{
		outMatrix.SetRotationAngles(someAngles.b,someAngles.p,someAngles.h);
	};


	template<class Type>
	CL::Angle3<Type> GetAnglesFromMatrix(const CL::Matrix33<Type>& aMatrix)
	{
		CL::Angle3<Type> returnAngles;

		returnAngles.p = asin(-aMatrix._32);
		returnAngles.b = atan2(aMatrix._12,aMatrix._22);
		returnAngles.h = atan2(aMatrix._31,aMatrix._33);
		return returnAngles;
	};
}

#endif