#pragma once
#include <string>

class ConsoleBackspace
{
public:
	ConsoleBackspace(std::string& aString);

	void Update(bool aButtonIsPressed);

private:
	void operator=(ConsoleBackspace&) = delete;
	std::string& myString;
	const float myFirstWait;
	const float myRepeatWait;
	float myCurrentTime;
	bool myHasReachedFirstWait;
};

