#pragma once

#include <GrowingArray.h>
#include "Message.h"
#include "MessageEnum.h"
#include <StaticArray.h>
#include "Subscriber.h"
#include <unordered_map>
#include <Vector.h>
#include <VectorOnStack.h>

#undef SendMessage

enum class ePriorityLayer
{
	NO_PRIO,
	NORMAL,
	HIGH
};

struct SubscriberInfo
{
	Subscriber* mySubscriber;
	ePriorityLayer myPriority = ePriorityLayer::NO_PRIO;
	bool myLetThrough = true;
};

class PostMaster
{
public:
	static PostMaster* GetInstance();
	static void Create();
	static void Destroy();

	void Subscribe(const eMessageType aMessageType, Subscriber* aSubscriber, ePriorityLayer aPriority = ePriorityLayer::NO_PRIO, bool aLetThrough = true);
	void UnSubscribe(const eMessageType aMessageType, Subscriber* aSubscriber);
	void UnSubscribe(Subscriber* aSubscriber);
	bool IsSubscribed(const eMessageType aMessageType, Subscriber* aSubscriber);

	template<typename Message>
	void SendMessage(const Message& aMessage);

private:
	PostMaster();
	~PostMaster();
	static PostMaster* myInstance;
	
	void SortSubscribers(CU::GrowingArray<SubscriberInfo>& aSubscribers);
	void QuickSort(CU::GrowingArray<SubscriberInfo>& aBuffer, const int aStart, const int aEnd);

	CU::StaticArray<CU::GrowingArray<SubscriberInfo>, static_cast<int>(eMessageType::COUNT)> mySubscribers;
};

template<typename Message>
void PostMaster::SendMessage(const Message& aMessage)
{
	CU::GrowingArray<SubscriberInfo>& subscribers
		= mySubscribers[static_cast<int>(aMessage.myMessageType)];

	if (subscribers.Size() > 0)
	{
		for (int i = 0; i < subscribers.Size(); ++i)
		{
			bool letThrough = subscribers[i].myLetThrough;
			subscribers[i].mySubscriber->ReceiveMessage(aMessage);

			if (letThrough == false)
			{
				return;
			}
		}
	}
	else if (aMessage.myMessageType != eMessageType::RESIZE)
	{
		//DL_ASSERT("Message sent without subscriber.");
	}
}