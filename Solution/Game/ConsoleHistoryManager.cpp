#include "stdafx.h"
#include <CommonHelper.h>
#include "ConsoleHistoryManager.h"
#include <fstream>
#include <MathHelper.h>
#include <Text.h>
#include <ModelLoader.h>


ConsoleHistoryManager::ConsoleHistoryManager()
	: myCurrentIndex(0)
	, myInsertIndex(0)
	, myHasWrapped(false)
	, myEmptyString("")
{
	std::string tempPath = CU::GetMyDocumentFolderPath() + "ICE";
	CreateDirectory(tempPath.c_str(), NULL);
	std::string temp = tempPath + "\\CommandHistory.txt";
	myHistoryFile = temp.c_str();
	myHistory.Init(256);
}

ConsoleHistoryManager::~ConsoleHistoryManager()
{
	for (int i = 0; i < myHistory.Size(); ++i)
	{
		SAFE_DELETE(myHistory[i]);
	}
}

void ConsoleHistoryManager::Save()
{
	std::fstream output;
	output.open(myHistoryFile, std::ios::out);
	for (int i = 1; i < myHistory.Size(); ++i)
	{
		output << int(myHistory[i]->myType) << " " << myHistory[i]->myMessage << std::endl;
	}
	output.flush();
	output.close();
}

void ConsoleHistoryManager::Load()
{
	//AddHistory(" ", eHistoryType::HISTORY);

	std::fstream output;
	output.open(myHistoryFile, std::ios::in);

	std::string line;
	while (std::getline(output, line))
	{
		std::string message = CU::GetSubString(line, ' ', true);

		std::string identifier = CU::GetSubString(line, ' ', false);
		std::stringstream error;
		error << int(eHistoryType::ERROR);
		std::stringstream help;
		help << int(eHistoryType::HELP);
		std::stringstream genCommand;
		genCommand << int(eHistoryType::GENERATED_COMMAND);

		eHistoryType type;
		if (identifier == error.str().c_str())
		{
			type = eHistoryType::ERROR;
		}
		else if (identifier == help.str().c_str())
		{
			type = eHistoryType::HELP;
		}
		else if (identifier == genCommand.str().c_str())
		{
			type = eHistoryType::GENERATED_COMMAND;
		}
		else
		{
			type = eHistoryType::HISTORY;
		}

		AddHistory(message, type);
	}
	output.close();
}

const std::string& ConsoleHistoryManager::GetCurrent(eHistoryType)
{
	if (myHistory.Size() == 0)
	{
		return myEmptyString;
	}
	return myHistory[myCurrentIndex]->myMessage;
}


const std::string& ConsoleHistoryManager::GetNext(eHistoryType aType)
{
	if (myHistory.Size() == 0)
	{
			return myEmptyString;
	}
	myCurrentIndex++;
	if (myCurrentIndex >= myHistory.Size())
	{
		myCurrentIndex = 0;
	}
	if (CheckType(aType, false) == false)
	{
		return myEmptyString;
	}
	return myHistory[myCurrentIndex]->myMessage;
}

const std::string& ConsoleHistoryManager::GetPrevious(eHistoryType aType)
{
	if (myHistory.Size() == 0)
	{
		return myEmptyString;
	}
	myCurrentIndex--;
	if (myCurrentIndex < 0)
	{
		myCurrentIndex = myHistory.Size() - 1;
	}
	if (CheckType(aType, true) == false)
	{
		return myEmptyString;
	}
	return myHistory[myCurrentIndex]->myMessage;
}

bool ConsoleHistoryManager::IsInHistory(const std::string& aString) const
{
	if (aString == " " || aString == "")
	{
		return true;
	}
	//int index = aString.find_first_of("(");
	for (int i = 0; i < myHistory.Size(); ++i)
	{
		if (myHistory[i]->myMessage.length() < aString.length())
		{
			continue;
		}
		for (unsigned int j = 0; j < aString.length(); ++j)
		{
			if (myHistory[i]->myMessage[j] != aString[j])
			{
				break;
			}
		}
		return true;
	}
	return false;
}

bool ConsoleHistoryManager::CheckType(eHistoryType aType, bool aShouldGoBackwards)
{
	if (myHistory[myCurrentIndex]->myType == aType)
	{
		return true;
	}
	else
	{
		int aStartIndex = myCurrentIndex;
		while (myHistory[myCurrentIndex]->myType != aType)
		{
			if (aShouldGoBackwards == true)
			{
				myCurrentIndex--;
				if (myCurrentIndex < 0)
				{
					myCurrentIndex = myHistory.Size() - 1;
				}
				if (myCurrentIndex == aStartIndex)
				{
					return false;
				}
			}
			else
			{
				++myCurrentIndex;
				if (myCurrentIndex > myHistory.Size() - 1)
				{
					myCurrentIndex = 0;
				}
				if (myCurrentIndex == aStartIndex)
				{
					return false;
				}
			}
		}
	}

	return true;








	//bool currentlyWrong = true;
	//bool hasLooped = false;
	//while (currentlyWrong == true)
	//{
	//	if (myHistory[myCurrentIndex]->myType != aType)
	//	{
	//		if (aShouldGoBackwards == false)
	//		{
	//			myCurrentIndex++;
	//			if (myCurrentIndex >= myHistory.Size() && hasLooped == false)
	//			{
	//				myCurrentIndex = 0;
	//				hasLooped = true;
	//			}
	//			else if (myCurrentIndex >= myHistory.Size() && hasLooped == true)
	//			{
	//				break;
	//			}
	//		}
	//		else if (aShouldGoBackwards == true)
	//		{
	//			myCurrentIndex--;
	//			if (myCurrentIndex < 0 && hasLooped == false)
	//			{
	//				myCurrentIndex = myHistory.Size() - 1;
	//				hasLooped = true;
	//			}
	//			else if (myCurrentIndex < 0 && hasLooped == true)
	//			{
	//				break;
	//			}
	//		}
	//	}
	//	else
	//	{
	//		break;
	//	}
	//}
}

void ConsoleHistoryManager::SplitCommandToMulitpleLines(const std::string& aCommand, eHistoryType anEnum)
{
	std::string splitedCommandA = aCommand;
	std::string splitedCommandB;
	int currentEndLineIndex = splitedCommandA.find_first_of("\n");
	splitedCommandA = aCommand.substr(0, currentEndLineIndex);
	splitedCommandB = aCommand.substr(currentEndLineIndex + 1, aCommand.length() - 1);
	splitedCommandA = RemoveTabFromString(splitedCommandA);
	splitedCommandB = RemoveTabFromString(splitedCommandB);
	AddHistory(splitedCommandA, anEnum);
	AddHistory(splitedCommandB, anEnum);
}

std::string ConsoleHistoryManager::RemoveTabFromString(const std::string& aCommand)
{
	std::string noTabCommand = aCommand;
	int currentTabIndex = noTabCommand.find_first_of("\t");
	if (currentTabIndex != -1)
	{
		noTabCommand = aCommand.substr(currentTabIndex + 1, aCommand.length()-1);
		RemoveTabFromString(noTabCommand);
	}
	return noTabCommand;
}

void ConsoleHistoryManager::AddHistory(const std::string& aCommand, eHistoryType anEnum)
{
	bool shouldPause = !Prism::ModelLoader::GetInstance()->IsPaused();

	if (shouldPause == true)
	{
		Prism::ModelLoader::GetInstance()->Pause();
	}

	DL_ASSERT_EXP(aCommand.length() > 0, "Should not be able to save empty commands in history");
	if (aCommand.find_first_of("\n") != -1)
	{
		SplitCommandToMulitpleLines(aCommand, anEnum);
		return;
	}
	History* tempHistory;
	
	//bool prevRuntime = ////Prism::MemoryTracker::GetInstance()->GetRunTime();
	////Prism::MemoryTracker::GetInstance()->SetRunTime(false);
	tempHistory = new History();
	tempHistory->myRenderText = new Prism::Text(*Prism::Engine::GetInstance()->GetFont(Prism::eFont::CONSOLE));
	////Prism::MemoryTracker::GetInstance()->SetRunTime(prevRuntime);

	tempHistory->myMessage = aCommand;
	tempHistory->myRenderText->SetText(aCommand);
	tempHistory->myType = anEnum;
	switch (anEnum)
	{
	case eHistoryType::ERROR:
		tempHistory->myRenderText->SetColor({ 1.f, 0.f, 0.f, 1.f });
		break;
	case eHistoryType::HISTORY:
		tempHistory->myRenderText->SetColor({ 1.f, 1.f, 1.f, 1.f });
		break;
	case eHistoryType::HELP:
		tempHistory->myRenderText->SetColor({ 0.5f, 1.f, 0.5f, 1.f });
		break;
	case eHistoryType::GENERATED_COMMAND:
		tempHistory->myRenderText->SetColor({ 0.6f, 0.6f, 0.6f, 1.f });
		break;
	case eHistoryType::WARNING:
		tempHistory->myRenderText->SetColor({ 1.0f, 1.0f, 0.5f, 1.f });
		break;
	default:
		break;
	}

	if (myHistory.Size() >= myHistory.GetCapacity())
	{
	//	SAFE_DELETE(myHistory[myInsertIndex]->myRenderText);
		myHistory.DeleteNonCyclicAtIndex(0);
	}
	myHistory.Add(tempHistory);

	myInsertIndex++;

	if (shouldPause == true)
	{
		Prism::ModelLoader::GetInstance()->UnPause();
	}
}


History::History()
	: myType(eHistoryType::ERROR)
	, myRenderText(nullptr)
{
}

History::~History()
{
	SAFE_DELETE(myRenderText);
}
