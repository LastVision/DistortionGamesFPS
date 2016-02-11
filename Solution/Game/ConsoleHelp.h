#pragma once
#include <GrowingArray.h>

struct ConsoleLuaHelp
{
	std::string myFunctionName;
	std::string myArguments;
	std::string myHelpText;
};

class ConsoleHelp
{
public:
	ConsoleHelp();
	~ConsoleHelp();

	CU::GrowingArray<std::string> GetAllFunction();
	const ConsoleLuaHelp& GetHelpText(const std::string& aFuncitonName);
private:
	void UpdateDocumentation();

	CU::GrowingArray<ConsoleLuaHelp> myHelpDocumentation;
	ConsoleLuaHelp myEmptyHelpDoc;
};

