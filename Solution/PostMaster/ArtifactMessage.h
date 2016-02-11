#pragma once
#include "Message.h"
#include <GameEnum.h>

struct ArtifactMessage : public Message
{
	ArtifactMessage(eOwnerType aOwner, int aArtifactModifier);

	const eOwnerType myOwner;
	const int myArtifactModifier;
};

inline ArtifactMessage::ArtifactMessage(eOwnerType aOwner, int aArtifactModifier)
	: Message(eMessageType::RESOURCE)
	, myOwner(aOwner)
	, myArtifactModifier(aArtifactModifier)
{
}