#include "stdafx.h"

#ifndef RELEASE_BUILD

#include "ConsoleState.h"
#include "Console.h"
#include "ConsoleHelp.h"
#include "ConsoleHistoryManager.h"
#include <InputWrapper.h>
#include <ModelLoader.h>
#include <ScriptSystem.h>
#include <SpriteProxy.h>
#include <Text.h>

ConsoleState::ConsoleState(bool& aShouldReOpenConsole)
	: myShouldReOpenConsole(aShouldReOpenConsole)
	, myLuaSuggestions(8)
	, myCurrentSuggestion(0)
	, mySuggestionString("")
	, myHistoryMode(false)
	, myBackground(nullptr)
	, myMarker(nullptr)
	, mySuggestionBox(nullptr)
	, mySuggestionText(nullptr)
	, myText(nullptr)
	, myRenderTime(0)
	, myMarkerBlinker(0)
{
}

ConsoleState::~ConsoleState()
{
}

void ConsoleState::InitState(StateStackProxy* aStateStackProxy, GUI::Cursor* aCursor)
{
	myStateStack = aStateStackProxy;
	myCursor = aCursor;
	myStateStatus = eStateStatus::eKeepState;
	myIsLetThrough = true;

	CU::Vector2<float> windowSize = Prism::Engine::GetInstance()->GetWindowSize();
	windowSize *= 0.75f;
	
	myBackground = Prism::ModelLoader::GetInstance()->LoadSprite(
		"Data/Resource/Texture/Console/T_console_window.dds", windowSize);
	myMarker = Prism::ModelLoader::GetInstance()->LoadSprite(
		"Data/Resource/Texture/UI/T_healthbar_background.dds", { 2.f, 20.f }, { 0.f, 0.f });
	mySuggestionBox = Prism::ModelLoader::GetInstance()->LoadSprite(
		"Data/Resource/Texture/Console/T_console_window_suggestions.dds", { windowSize.x - 50.f, 30.f });
	/*myBackground = new Prism::Sprite("Data/Resource/Texture/Console/T_console_window.dds", windowSize);
	myMarker = new Prism::Sprite("Data/Resource/Texture/UI/T_healthbar_background.dds", { 2.f, 20.f }, { 0.f, 0.f });
	mySuggestionBox = new Prism::Sprite("Data/Resource/Texture/Console/T_console_window_suggestions.dds", { windowSize.x - 50.f, 30.f });*/
	myMarkerBlinker = true;

	myText = new Prism::Text(*Prism::Engine::GetInstance()->GetFont(Prism::eFont::CONSOLE));
	mySuggestionText = new Prism::Text(*Prism::Engine::GetInstance()->GetFont(Prism::eFont::CONSOLE));

	myLowerLeftCorner = Prism::Engine::GetInstance()->GetWindowSize();
	myLowerLeftCorner *= 0.25f;

	Console::GetInstance()->GetConsoleHistory()->ResetHistoryCounter();
}

void ConsoleState::EndState()
{
	SAFE_DELETE(myBackground);
	SAFE_DELETE(myMarker);
	SAFE_DELETE(myText);
	SAFE_DELETE(mySuggestionBox);
	SAFE_DELETE(mySuggestionText);
}

const eStateStatus ConsoleState::Update(const float& aDeltaTime)
{
	Console::GetInstance()->Update();

	if (CU::InputWrapper::GetInstance()->KeyUp(DIK_GRAVE) == true)
	{
		myStateStatus = ePopSubState;
		return eStateStatus::eKeepState;
	}

	if (CU::InputWrapper::GetInstance()->KeyUp(DIK_ESCAPE) == true)
	{
		myStateStatus = ePopSubState;
		return eStateStatus::eKeepState;
	}

	std::string input = Console::GetInstance()->GetInput();
	if (input == " " || input == "")
	{
		if (CU::InputWrapper::GetInstance()->KeyUp(DIK_UPARROW) == true)
		{
			myHistoryMode = true;
			//mySuggestionString = Console::GetInstance()->GetConsoleHistory()->GetPrevious();
		}
		if (CU::InputWrapper::GetInstance()->KeyUp(DIK_DOWNARROW) == true)
		{
			myHistoryMode = true;
			//mySuggestionString = Console::GetInstance()->GetConsoleHistory()->GetNext();
		}
	}

	if (myHistoryMode == true)
	{
		HandleHistoryMode();
	}
	else
	{
		HandleSuggestionMode();	
	}

	if (CU::InputWrapper::GetInstance()->KeyUp(DIK_RETURN) == true)
	{
		const std::string& consoleInput = Console::GetInstance()->GetInput();
		if (consoleInput != "")
		{
			myShouldReOpenConsole = true;
			Console::GetInstance()->GetConsoleHistory()->AddHistory(consoleInput);
			std::string errorString;
			std::string temp = CU::ToLower(consoleInput);
			if (temp.find("help") == 0 || temp.find("halp") == 0)
			{
				if (temp == "help" || temp == "halp")
				{
					const CU::GrowingArray<std::string>& allFunctions = Console::GetInstance()->GetConsoleHelp()->GetAllFunction();
					for (int i = 0; i < allFunctions.Size(); ++i)
					{
						Console::GetInstance()->GetConsoleHistory()->AddHistory(allFunctions[i], eHistoryType::HELP);
					}
				}
				else
				{
					std::string helpFunctionName = CU::GetSubString(consoleInput, " ", true, 2);
					const ConsoleLuaHelp& helpDoc = Console::GetInstance()->GetConsoleHelp()->GetHelpText(helpFunctionName);
					if (helpDoc.myFunctionName != "")
					{
						Console::GetInstance()->GetConsoleHistory()->AddHistory(helpDoc.myFunctionName + "(" + helpDoc.myArguments + ")", eHistoryType::HELP);
						Console::GetInstance()->GetConsoleHistory()->AddHistory(helpDoc.myHelpText, eHistoryType::HELP);
					}
					else
					{
						Console::GetInstance()->GetConsoleHistory()->AddHistory("There is no " + helpFunctionName +
							" command. Did you mean " + LUA::ScriptSystem::GetInstance()->FindClosestFunction(helpFunctionName)
							+ "?", eHistoryType::ERROR);
					}
				}
				Console::GetInstance()->ClearInput();
			}
			else if (LUA::ScriptSystem::GetInstance()->ValidateLuaString(consoleInput, errorString))
			{
				LUA::ScriptSystem::GetInstance()->RunLuaFromString(consoleInput);
				Console::GetInstance()->ClearInput();
			}
			else
			{
				Console::GetInstance()->GetConsoleHistory()->AddHistory(errorString, eHistoryType::ERROR);
			}


			Console::GetInstance()->GetConsoleHistory()->Save();
			myStateStatus = ePopSubState;
		}
	}


	if (Console::GetInstance()->GetInput().length() <= 0 && myHistoryMode == false)
	{
		mySuggestionString = "";
	}


	myText->SetText(Console::GetInstance()->GetInput());
	myMarkerPosition.x = (myLowerLeftCorner.x * 1.1f) + myText->GetWidth() + 3;
	myMarkerPosition.y = myLowerLeftCorner.y * 1.1f - 3;

	myRenderTime += aDeltaTime;
	if (myRenderTime > 0.5f)
	{
		myMarkerBlinker = !myMarkerBlinker;
		myRenderTime = 0.f;
	}

	LUA::ScriptSystem::GetInstance()->Update();
	return myStateStatus;
}

void ConsoleState::Render()
{

	myBackground->Render(myLowerLeftCorner);

	myText->SetPosition(myLowerLeftCorner * 1.1f);
	myText->SetScale({ 0.9f, 0.9f });
	if (mySuggestionString != "")
	{
		myText->SetColor({ 1.f, 1.f, 1.f, 0.2f });
		myText->SetText(mySuggestionString);
		myText->Render();
	}
	myText->SetText(Console::GetInstance()->GetInput());
	myText->SetColor({ 1.f, 1.f, 1.f, 1.f });
	myText->Render();



	if (myMarkerBlinker == true)
	{
		myMarker->Render(myMarkerPosition);
	}

	std::string consoleInput = Console::GetInstance()->GetInput();
	if (myHistoryMode == false && consoleInput.length() > 0)
	{
		CU::Vector2<float> windowSize = Prism::Engine::GetInstance()->GetWindowSize();
		windowSize *= 0.75f;
		mySuggestionBox->SetSize({ windowSize.x - 50.f, 30.f + (myLuaSuggestions.Size() * 30.f) }, { 0.f, myLuaSuggestions.Size() * 30.f });
		mySuggestionBox->Render({ myLowerLeftCorner.x + 25.f, myLowerLeftCorner.y - 20.f });

		for (int i = 0; i < myLuaSuggestions.Size(); ++i)
		{
			mySuggestionText->SetText(myLuaSuggestions[i]);
			mySuggestionText->SetPosition({ myLowerLeftCorner.x + 25.f, myLowerLeftCorner.y - (20.f * (i + 1)) });
			mySuggestionText->Render();
		}

	}

	const CU::GrowingArray<History*>& history = Console::GetInstance()->GetConsoleHistory()->GetHistoryArray();
	CU::Vector2<float> position = myLowerLeftCorner * 1.1f;
	position.y += 30.f;

	if (history.Size() != 0)
	{
		for (int i = history.Size() - 1; i >= history.Size()-23; --i)
		{
			if (i < 0)
			{
				break;
			}
			position.y += 30.f;
			history[i]->myRenderText->SetPosition(position);
			history[i]->myRenderText->Render();
		}
	}
	myLuaSuggestions.RemoveAll();
}

void ConsoleState::ResumeState()
{

}

void ConsoleState::OnResize(int aWidth, int aHeight)
{
	aWidth;
	aHeight;
}


void ConsoleState::HandleHistoryMode()
{
	if (CU::InputWrapper::GetInstance()->KeyUp(DIK_UPARROW) == true)
	{
		myHistoryMode = true;
		mySuggestionString = Console::GetInstance()->GetConsoleHistory()->GetPrevious();
	}
	else if (CU::InputWrapper::GetInstance()->KeyUp(DIK_DOWNARROW) == true)
	{
		myHistoryMode = true;
		mySuggestionString = Console::GetInstance()->GetConsoleHistory()->GetNext();
	}
	else if (CU::InputWrapper::GetInstance()->KeyUp(DIK_RETURN) == true)
	{
		Console::GetInstance()->SetInput(mySuggestionString);
		mySuggestionString = "";
	}
	else if (CU::InputWrapper::GetInstance()->KeyUp(DIK_BACKSPACE) == true)
	{
		if (Console::GetInstance()->GetInput().length() == 0)
		{
			myHistoryMode = false;
			mySuggestionString = "";
		}
	}
	if (Console::GetInstance()->GetInput().length() > 0)
	{
		mySuggestionString = "";
	}
	if (mySuggestionString.length() == 0)
	{
		myHistoryMode = false;
		mySuggestionString = "";
		myCurrentSuggestion = 0;
	}
}

void ConsoleState::HandleSuggestionMode()
{
	const CU::GrowingArray<std::string> allFunctions = Console::GetInstance()->GetConsoleHelp()->GetAllFunction();
	std::string input = Console::GetInstance()->GetInput();

	if (input.length() > 0)
	{
		std::string inputFunctionName(input);
		inputFunctionName = CU::ToLower(inputFunctionName);
		int index = inputFunctionName.find_first_of("(");
		if (index != std::string::npos)
		{
			inputFunctionName = std::string(inputFunctionName.begin(), inputFunctionName.begin() + index);
		}
		for (int i = 0; i < allFunctions.Size(); ++i)
		{
			std::string	functionToTest = CU::ToLower(allFunctions[i]);
			if (functionToTest.find(inputFunctionName) != std::string::npos)
			{
				myLuaSuggestions.Add(allFunctions[i]);
			}
		}

		if (CU::InputWrapper::GetInstance()->KeyUp(DIK_UPARROW) == true)
		{
			--myCurrentSuggestion;
		}

		if (CU::InputWrapper::GetInstance()->KeyUp(DIK_DOWNARROW) == true)
		{
			++myCurrentSuggestion;
		}
		myCurrentSuggestion = CU::ClipInt(myCurrentSuggestion, 0, myLuaSuggestions.Size() - 1);
		if (myLuaSuggestions.Size() > 0 && index == std::string::npos)
		{
			mySuggestionString = myLuaSuggestions[myCurrentSuggestion];
		}
		else
		{
			mySuggestionString = "";
		}
	}


	if (CU::InputWrapper::GetInstance()->KeyUp(DIK_TAB) == true)
	{
		if (mySuggestionString != "")
		{
			int index = mySuggestionString.find_first_of("(");
			Console::GetInstance()->SetInput(std::string(mySuggestionString.begin(), mySuggestionString.begin() + index + 1));
			mySuggestionString = "";
			myCurrentSuggestion = 0;
		}
	}
}
#endif