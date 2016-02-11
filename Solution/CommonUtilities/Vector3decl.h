#pragma once


namespace CU
{

	template<typename T>
	class Vector3
	{
	public:
		Vector3();
		Vector3(T aX, T aY, T aZ);
		~Vector3();

		T x;
		T y;
		T z;
	};
}