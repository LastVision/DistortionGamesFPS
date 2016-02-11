#include "stdafx.h"
#include "Console.h"
#ifndef RELEASE_BUILD
#include "ConsoleBackspace.h"
#include "ConsoleHelp.h"
#include "ConsoleHistoryManager.h"
#include <InputWrapper.h>
#include <RunScriptMessage.h>
#include <ScriptSystem.h>
#include <PostMaster.h>
#include <ModelLoader.h>
#endif
Console* Console::myInstance = nullptr;

Console* Console::GetInstance()
{
	if (myInstance == nullptr)
	{
		myInstance = new Console();
	}

	return myInstance;
}

void Console::Destroy()
{
	delete myInstance;
	myInstance = nullptr;
}

Console::Console()
{
#ifndef RELEASE_BUILD
	myHistory = new ConsoleHistoryManager();
	myHistory->Load();
	myBackspace = new ConsoleBackspace(myInput);
	myHelp = new ConsoleHelp();
	PostMaster::GetInstance()->Subscribe(eMessageType::RUN_SCRIPT, this);
#endif
}

Console::~Console()
{
#ifndef RELEASE_BUILD
	PostMaster::GetInstance()->UnSubscribe(eMessageType::RUN_SCRIPT, this);
	SAFE_DELETE(myHistory);
	SAFE_DELETE(myBackspace);
	SAFE_DELETE(myHelp);
#endif
}

#ifndef RELEASE_BUILD

void Console::Update()
{
	ReadInput();
	myBackspace->Update(CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_BACKSPACE));
	
	//if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_UPARROW) || 
	//	CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_DOWNARROW))
	//{
	//	myInput = myHistory->GetCurrent();
	//}

	//if (CU::InputWrapper::GetInstance()->KeyUp(DIK_RETURN))
	//{
	//	LUA::ScriptSystem::GetInstance()->RunLuaFromString(myInput);
	//	myInput = "";
	//}

	//Use to find keycodes
	/*for (int i  = 0; i < 255; ++i)
	{
		if (CU::InputWrapper::GetInstance()->KeyIsPressed(i))
		{
			DEBUG_PRINT(i);
		}
	}*/

	//DEBUG_PRINT(myInput);
}

void Console::ReceiveMessage(const RunScriptMessage& aMessage)
{
	if (aMessage.myMessageType == eMessageType::RUN_SCRIPT)
	{
		std::fstream output;
		output.open(aMessage.myFilePath, std::ios::in);

		std::string line;
		std::string errorLine;
		while (std::getline(output, line))
		{
			if (LUA::ScriptSystem::GetInstance()->ValidateLuaString(line, errorLine))
			{
				LUA::ScriptSystem::GetInstance()->RunLuaFromString(line);
				Console::GetInstance()->GetConsoleHistory()->AddHistory(line, eHistoryType::GENERATED_COMMAND);
			}
			else
			{
				Console::GetInstance()->GetConsoleHistory()->AddHistory(errorLine, eHistoryType::ERROR);
			}
	
		}
		Console::GetInstance()->GetConsoleHistory()->Save();
		output.close();
	}
}

void Console::AddHistory(const std::string& aMessage, eHistoryType aType)
{
	bool shouldPause = !Prism::ModelLoader::GetInstance()->IsPaused();
	if (shouldPause == true)
	{
		Prism::ModelLoader::GetInstance()->Pause();
	}
	myHistory->AddHistory(aMessage, aType);
	if (shouldPause == true)
	{
		Prism::ModelLoader::GetInstance()->UnPause();
	}
}

void Console::ReadInput()
{
	CU::InputWrapper* input = CU::InputWrapper::GetInstance();

	bool shift = input->KeyIsPressed(DIK_LSHIFT) || input->KeyIsPressed(DIK_RSHIFT);

	myInput += input->KeyDown(DIK_A) ? shift ? "A" : "a" : "";
	myInput += input->KeyDown(DIK_B) ? shift ? "B" : "b" : "";
	myInput += input->KeyDown(DIK_C) ? shift ? "C" : "c" : "";
	myInput += input->KeyDown(DIK_D) ? shift ? "D" : "d" : "";
	myInput += input->KeyDown(DIK_E) ? shift ? "E" : "e" : "";
	myInput += input->KeyDown(DIK_F) ? shift ? "F" : "f" : "";
	myInput += input->KeyDown(DIK_G) ? shift ? "G" : "g" : "";
	myInput += input->KeyDown(DIK_H) ? shift ? "H" : "h" : "";
	myInput += input->KeyDown(DIK_I) ? shift ? "I" : "i" : "";
	myInput += input->KeyDown(DIK_J) ? shift ? "J" : "j" : "";
	myInput += input->KeyDown(DIK_K) ? shift ? "K" : "k" : "";
	myInput += input->KeyDown(DIK_L) ? shift ? "L" : "l" : "";
	myInput += input->KeyDown(DIK_M) ? shift ? "M" : "m" : "";
	myInput += input->KeyDown(DIK_N) ? shift ? "N" : "n" : "";
	myInput += input->KeyDown(DIK_O) ? shift ? "O" : "o" : "";
	myInput += input->KeyDown(DIK_P) ? shift ? "P" : "p" : "";
	myInput += input->KeyDown(DIK_Q) ? shift ? "Q" : "q" : "";
	myInput += input->KeyDown(DIK_R) ? shift ? "R" : "r" : "";
	myInput += input->KeyDown(DIK_S) ? shift ? "S" : "s" : "";
	myInput += input->KeyDown(DIK_T) ? shift ? "T" : "t" : "";
	myInput += input->KeyDown(DIK_U) ? shift ? "U" : "u" : "";
	myInput += input->KeyDown(DIK_V) ? shift ? "V" : "v" : "";
	myInput += input->KeyDown(DIK_W) ? shift ? "W" : "w" : "";
	myInput += input->KeyDown(DIK_X) ? shift ? "X" : "x" : "";
	myInput += input->KeyDown(DIK_Y) ? shift ? "Y" : "y" : "";
	myInput += input->KeyDown(DIK_Z) ? shift ? "Z" : "z" : "";

	myInput += input->KeyDown(DIK_0) ? shift ? "=" : "0" : "";
	myInput += input->KeyDown(DIK_1) ? shift ? "!" : "1" : "";
	myInput += input->KeyDown(DIK_2) ? shift ? "\"" : "2" : "";
	myInput += input->KeyDown(DIK_3) ? shift ? "#" : "3" : "";
	myInput += input->KeyDown(DIK_4) ? shift ? "¤" : "4" : "";
	myInput += input->KeyDown(DIK_5) ? shift ? "%" : "5" : "";
	myInput += input->KeyDown(DIK_6) ? shift ? "&" : "6" : "";
	myInput += input->KeyDown(DIK_7) ? shift ? "/" : "7" : "";
	myInput += input->KeyDown(DIK_8) ? shift ? "(" : "8" : "";
	myInput += input->KeyDown(DIK_9) ? shift ? ")" : "9" : "";

	myInput += input->KeyDown(DIK_NUMPAD0) ? shift ? "0" : "0" : "";
	myInput += input->KeyDown(DIK_NUMPAD1) ? shift ? "1" : "1" : "";
	myInput += input->KeyDown(DIK_NUMPAD2) ? shift ? "2" : "2" : "";
	myInput += input->KeyDown(DIK_NUMPAD3) ? shift ? "3" : "3" : "";
	myInput += input->KeyDown(DIK_NUMPAD4) ? shift ? "4" : "4" : "";
	myInput += input->KeyDown(DIK_NUMPAD5) ? shift ? "5" : "5" : "";
	myInput += input->KeyDown(DIK_NUMPAD6) ? shift ? "6" : "6" : "";
	myInput += input->KeyDown(DIK_NUMPAD7) ? shift ? "7" : "7" : "";
	myInput += input->KeyDown(DIK_NUMPAD8) ? shift ? "8" : "8" : "";
	myInput += input->KeyDown(DIK_NUMPAD9) ? shift ? "9-" : "9" : "";

	myInput += input->KeyDown(DIK_NUMPADPLUS) ? shift ? "+" : "+" : "";
	myInput += input->KeyDown(DIK_NUMPADMINUS) ? shift ? "-" : "-" : "";
	myInput += input->KeyDown(DIK_NUMPADSTAR) ? shift ? "*" : "*" : "";
	myInput += input->KeyDown(DIK_NUMPADSLASH) ? shift ? "/" : "/" : "";
	myInput += input->KeyDown(DIK_NUMPADCOMMA) ? shift ? "," : "," : "";
	myInput += input->KeyDown(DIK_NUMPADPERIOD) ? shift ? "." : "." : "";

	myInput += input->KeyDown(DIK_COMMA) ? shift ? ";" : "," : "";
	myInput += input->KeyDown(DIK_PERIOD) ? shift ? ":" : "." : "";
	myInput += input->KeyDown(53) ? shift ? "_" : "-" : "";

	if (input->KeyDown(DIK_SPACE))
	{
		myInput += " ";
	}

	if (input->KeyDown(DIK_TAB) && myInput.size() > 0)
	{
		//myInput = mySuggestion;
	}

	
}

#endif