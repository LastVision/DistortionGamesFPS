#include "stdafx.h"

#include "Console.h"
#include <Entity.h>
#include <PostMaster.h>
#include "ScriptInterface.h"
#include <ScriptSystem.h>

namespace Script_Interface
{
	int Print(lua_State* aState) //std::string aString
	{
		std::string stringArg = lua_tostring(aState, 1);

		DEBUG_PRINT_LUA(stringArg);

		return 0;
	}

	int PrintConsole(lua_State* aState) //std::string aString
	{
		std::string stringArg = lua_tostring(aState, 1);

		//Console::GetInstance()->AddHistory(stringArg, eHistoryType::WARNING);
		return 0;
	}

	int ScriptRun(lua_State* aState)//string aScriptFile
	{
		std::string filePath = lua_tostring(aState, 1);
		//PostMaster::GetInstance()->SendMessage(RunScriptMessage(filePath));
		return 0;
	}
}

void ScriptInterface::RegisterFunctions()
{
	LUA::ScriptSystem* system = LUA::ScriptSystem::GetInstance();
	system->RegisterFunction("Print", Script_Interface::Print, "aString", "Prints stuff to the screen");
	system->RegisterFunction("PrintConsole", Script_Interface::PrintConsole, "aString", "Prints a single message to the console.");
	system->RegisterFunction("ScriptRun", Script_Interface::ScriptRun, "aScriptFile", "Run the script file.");
}
