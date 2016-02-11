#pragma once

namespace CU
{
	template<typename T>
	class Lesser
	{
	public:
		bool operator()(T a, T b)
		{
			return a < b;
		}
	};
}