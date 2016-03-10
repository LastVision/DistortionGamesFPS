#pragma once

#include "Vector3.h"

namespace CU
{
	template<typename T>

	class Vector4
	{
	public:
		Vector4();
		Vector4(T aX, T aY, T aZ, T aW);
		Vector4(Vector3<T> aVector, T aW);
		~Vector4();
		Vector3<T> GetVector3()
		{
			return Vector3<T>(x, y, z);
		}

		T x;
		T y;
		T z;
		T w;
	};
}