#pragma once

namespace Prism
{
	namespace Navigation
	{
		template<typename T>
		class LesserTriangle
		{
		public:
			bool operator()(T a, T b)
			{
				return a->myTotalCost < b->myTotalCost;
			}
		};
	}
}