#pragma once
#include "GrowingArray.h"

namespace Prism
{
	struct Lod
	{
		short myLevel;
		bool myUseLod;
	};

	struct LodGroup
	{
		LodGroup()
			: myLods(8)
			, myThreshHolds(8)
			, myMin(0.f)
			, myMax(1000.f)
		{}

		Lod& CreateLod(){ myLods.Add(Lod()); return myLods.GetLast(); }

		CU::GrowingArray<Lod> myLods;
		CU::GrowingArray<double> myThreshHolds;
		float myMin;
		float myMax;
	};
}