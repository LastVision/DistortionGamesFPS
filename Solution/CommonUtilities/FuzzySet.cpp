#include "stdafx.h"
#include "FuzzySet.h"


namespace CU
{
	FuzzySet::FuzzySet(int aNumberOfMembers)
		: mySet(aNumberOfMembers)
	{
		for (int i = 0; i < aNumberOfMembers; ++i)
		{
			mySet.Add(0.f);
		}
	}

	FuzzySet::~FuzzySet()
	{
	}

	void FuzzySet::AddValue(int aIndex, float aValue)
	{
		DL_ASSERT_EXP(aIndex >= 0, "FuzzySet index below 0");
		DL_ASSERT_EXP(aIndex < mySet.Size(), "FuzzySet index too high");

		mySet[aIndex] += fmaxf(aValue, 0.f);
	}

	float FuzzySet::GetValue(int aIndex) const
	{
		DL_ASSERT_EXP(aIndex >= 0, "FuzzySet index below 0");
		DL_ASSERT_EXP(aIndex < mySet.Size(), "FuzzySet index too high");

		return mySet[aIndex];
	}

	int FuzzySet::GetHighersMember() const
	{
		DL_ASSERT_EXP(mySet.Size() >= 0, "Cant get HighestMember from empty FuzzySet");

		float bestValue = FLT_MIN;
		int bestIndex = -1;

		for (int i = 0; i < mySet.Size(); ++i)
		{
			if (mySet[i] > bestValue)
			{
				bestValue = mySet[i];
				bestIndex = i;
			}
		}

		return bestIndex;
	}

	void FuzzySet::Normalize()
	{
		float total = 0.f;
		for (int i = 0; i < mySet.Size(); ++i)
		{
			total += mySet[i];
		}

		for (int i = 0; i < mySet.Size(); ++i)
		{
			mySet[i] /= total;
		}
	}

	void FuzzySet::Reset()
	{
		for (int i = 0; i < mySet.Size(); ++i)
		{
			mySet[i] = 0.f;
		}
	}
}