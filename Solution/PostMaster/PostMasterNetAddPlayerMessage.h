#pragma once
#include "Message.h"
#include <string>
struct PostMasterNetAddPlayerMessage : public Message
{
	PostMasterNetAddPlayerMessage(const std::string& aName, const sockaddr_in& anAddress);

	std::string myName;
	sockaddr_in myAddress;

};

inline PostMasterNetAddPlayerMessage::PostMasterNetAddPlayerMessage(const std::string& aName, const sockaddr_in& anAddress)
	: Message(eMessageType::NETWORK_ADD_PLAYER)
	, myName(aName)
	, myAddress(anAddress)
{
}