#pragma once

namespace CU
{
	template<typename T>
	class Greater
	{
	public:
		bool operator()(T a, T b)
		{
			return a > b;
		}
	};
}