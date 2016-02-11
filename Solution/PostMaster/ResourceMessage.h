#pragma once
#include "Message.h"

class Entity;


struct ResourceMessage : public Message
{
	ResourceMessage(eOwnerType aOwner, int aResourceModifier);

	const eOwnerType myOwner;
	const int myResourceModifier;
};

inline ResourceMessage::ResourceMessage(eOwnerType aOwner, int aResourceModifier)
	: Message(eMessageType::RESOURCE)
	, myOwner(aOwner)
	, myResourceModifier(aResourceModifier)
{
}