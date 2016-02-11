#include "stdafx.h"
#include "ConsoleHelp.h"
#include <ScriptSystem.h>

ConsoleHelp::ConsoleHelp()
	: myHelpDocumentation(128)
{
	UpdateDocumentation();
	myEmptyHelpDoc = { "", "", "" };
}

ConsoleHelp::~ConsoleHelp()
{
}

void ConsoleHelp::UpdateDocumentation()
{
	const std::vector<LUA::Documentation>& luaDocumentation = LUA::ScriptSystem::GetInstance()->GetDocumentation();
	for (unsigned int i = 0; i < luaDocumentation.size(); ++i)
	{
		ConsoleLuaHelp newFunction;
		newFunction.myFunctionName = luaDocumentation[i].myFunction;
		newFunction.myArguments = luaDocumentation[i].myArguments;
		newFunction.myHelpText = luaDocumentation[i].myHelpText;
		myHelpDocumentation.Add(newFunction);
	}
}

CU::GrowingArray<std::string> ConsoleHelp::GetAllFunction()
{
	CU::GrowingArray<std::string> arrayWithFunctionNames(myHelpDocumentation.Size());
	std::string functionName;
	for (int i = 0; i < myHelpDocumentation.Size(); ++i)
	{
		functionName = myHelpDocumentation[i].myFunctionName + "(" + myHelpDocumentation[i].myArguments + ")";
		arrayWithFunctionNames.Add(functionName);
	}
	return arrayWithFunctionNames;
}

const ConsoleLuaHelp& ConsoleHelp::GetHelpText(const std::string& aFuncitonName)
{
	for (int i = 0; i < myHelpDocumentation.Size(); ++i)
	{
		if (myHelpDocumentation[i].myFunctionName == aFuncitonName)
		{
			return myHelpDocumentation[i];
		}
	}
	
	return myEmptyHelpDoc;
}