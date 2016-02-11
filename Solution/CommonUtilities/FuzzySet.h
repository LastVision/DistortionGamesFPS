#pragma once

#include "GrowingArray.h"

namespace CU
{
	class FuzzySet
	{
		friend FuzzySet operator+=(FuzzySet& aFirst, const FuzzySet& aOther);

	public:
		FuzzySet(int aNumberOfMembers);
		~FuzzySet();

		void AddValue(int aIndex, float aValue);
		float GetValue(int aIndex) const;
		int GetHighersMember() const;

		void Normalize();
		void Reset();

	private:
		CU::GrowingArray<float> mySet;
	};

	inline FuzzySet operator+=(FuzzySet& aFirst, const FuzzySet& aOther)
	{
		for (int i = 0; i < aFirst.mySet.Size(); ++i)
		{
			aFirst.AddValue(i, aOther.mySet[i]);
		}

		return aFirst;
	}
}