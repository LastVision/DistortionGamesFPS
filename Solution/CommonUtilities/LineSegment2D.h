#pragma once
#include "Vector.h"
namespace CU
{
	namespace Intersection
	{
		class LineSegment2D
		{
		public:
			LineSegment2D(const CU::Vector2<float>& aPos1, const CU::Vector2<float>& aPos2)
				: myStartPos(aPos1), myEndPos(aPos2) {}
			LineSegment2D(){}
			CU::Vector2<float> myStartPos;
			CU::Vector2<float> myEndPos;
		};
	}
}