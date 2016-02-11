#include "stdafx.h"

#include "PostMaster.h"

PostMaster* PostMaster::myInstance = nullptr;
PostMaster::PostMaster()
{
	for (int i = 0; i < static_cast<int>(eMessageType::COUNT); ++i)
	{
		mySubscribers[i].Init(64);
	}
}


PostMaster::~PostMaster()
{
	for (int i = 0; i < static_cast<int>(eMessageType::COUNT); ++i)
	{
		if (mySubscribers[i].Size() > 0)
		{
			DL_ASSERT("Subscriber not unsubscribed at Postmaster-Destroy.");
		}

		mySubscribers[i].RemoveAll();
	}
}

PostMaster* PostMaster::GetInstance()
{
	DL_ASSERT_EXP(myInstance != nullptr, "Need to create Postmaster before getting it.");
	return myInstance;
}

void PostMaster::Create()
{
	DL_ASSERT_EXP(myInstance == nullptr, "Postmaster already created.");

	myInstance = new PostMaster();
}

void PostMaster::Destroy()
{
	DL_ASSERT_EXP(myInstance != nullptr, "Need to create Postmaster before destroying it.");

	
	delete myInstance;
	myInstance = nullptr;
}

void PostMaster::Subscribe(const eMessageType aMessageType, Subscriber* aSubscriber, ePriorityLayer aPriority, bool aLetThrough)
{
#ifdef _DEBUG
	CU::GrowingArray<SubscriberInfo>& subscribers
		= mySubscribers[static_cast<int>(aMessageType)];
	for (int i = 0; i < subscribers.Size(); ++i)
	{
		DL_ASSERT_EXP(subscribers[i].mySubscriber != aSubscriber, "Tried to add the same subscriber to the same message twice.");
	}
#endif

	SubscriberInfo newSubscriber;
	newSubscriber.mySubscriber = aSubscriber;
	newSubscriber.myPriority = aPriority;
	newSubscriber.myLetThrough = aLetThrough;

	if (aPriority == ePriorityLayer::NO_PRIO)
	{
		mySubscribers[static_cast<int>(aMessageType)].Add(newSubscriber);
	}
	else
	{
		mySubscribers[static_cast<int>(aMessageType)].Add(newSubscriber);
		SortSubscribers(mySubscribers[static_cast<int>(aMessageType)]);
	}
}

void PostMaster::UnSubscribe(const eMessageType aMessageType, Subscriber* aSubscriber)
{
	CU::GrowingArray<SubscriberInfo>& subscribers
		= mySubscribers[static_cast<int>(aMessageType)];

	for (int i = 0; i < subscribers.Size(); ++i)
	{
		if (subscribers[i].mySubscriber == aSubscriber)
		{
			subscribers.RemoveCyclicAtIndex(i);
			break;
		}
	}

	SortSubscribers(subscribers);
}

void PostMaster::UnSubscribe(Subscriber* aSubscriber)
{
	for (int i = 0; i < static_cast<int>(eMessageType::COUNT); ++i)
	{
		CU::GrowingArray<SubscriberInfo, int>& subscribers
			= mySubscribers[i];

		for (int j = 0; j < subscribers.Size(); ++j)
		{
			if (subscribers[j].mySubscriber == aSubscriber)
			{
				subscribers.RemoveCyclicAtIndex(j);
				break;
			}
		}
	}
}

bool PostMaster::IsSubscribed(const eMessageType aMessageType, Subscriber* aSubscriber)
{
	CU::GrowingArray<SubscriberInfo>& subscribers
		= mySubscribers[static_cast<int>(aMessageType)];

	for (int i = 0; i < subscribers.Size(); ++i)
	{
		if (subscribers[i].mySubscriber == aSubscriber)
		{
			return true;
		}
	}

	return false;
}

void PostMaster::SortSubscribers(CU::GrowingArray<SubscriberInfo> &aBuffer)
{
	int max = 0;
	if (aBuffer.Size() < 3)
	{
		if (aBuffer.Size() > 1 && (aBuffer[1].myPriority > aBuffer[0].myPriority))
		{
			std::swap(aBuffer[1], aBuffer[0]);
		}
		return;
	}

	for (int i = 0; i < aBuffer.Size(); ++i)
	{
		if (aBuffer[max].myPriority < aBuffer[i].myPriority)
			max = i;
	}

	std::swap(aBuffer[max], aBuffer[aBuffer.Size() - 1]);
	QuickSort(aBuffer, 0, aBuffer.Size() - 2);
}

void PostMaster::QuickSort(CU::GrowingArray<SubscriberInfo> &aBuffer, const int aStart, const int aEnd)
{
	int lower = aStart + 1;
	int upper = aEnd;

	std::swap(aBuffer[aStart], aBuffer[(aStart + aEnd) / 2]);

	int bound = static_cast<int>(aBuffer[aStart].myPriority);

	while (lower <= upper)
	{
		while (bound > static_cast<int>(aBuffer[lower].myPriority))
			++lower;

		while (bound < static_cast<int>(aBuffer[upper].myPriority))
			--upper;

		if (lower < upper)
			std::swap(aBuffer[lower++], aBuffer[upper--]);
		else
			++lower;
	}

	std::swap(aBuffer[aStart], aBuffer[upper]);

	if (aStart < upper - 1)
		QuickSort(aBuffer, aStart, upper - 1);
	if (upper + 1 < aEnd)
		QuickSort(aBuffer, upper + 1, aEnd);
}