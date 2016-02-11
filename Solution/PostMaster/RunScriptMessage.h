#pragma once
#include "Message.h"
#include <string>

struct RunScriptMessage : public Message
{
	RunScriptMessage(const std::string& aFile);

	const std::string myFilePath;
};

inline RunScriptMessage::RunScriptMessage(const std::string& aFile)
	: Message(eMessageType::RUN_SCRIPT)
	, myFilePath(aFile)
{
}