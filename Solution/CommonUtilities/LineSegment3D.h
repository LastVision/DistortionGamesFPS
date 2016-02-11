#pragma once
#include "Vector.h"
namespace CU
{
	namespace Intersection
	{
		class LineSegment3D
		{
		public:
			LineSegment3D(const CU::Vector3<float>& aStartPos, const CU::Vector3<float>& aEndPos)
				: myStartPos(aStartPos)
				, myEndPos(aEndPos)
			{}
			CU::Vector3<float> myStartPos;
			CU::Vector3<float> myEndPos;
		};
	}
}