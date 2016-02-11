#include "stdafx.h"

#include <GameStateMessage.h>
#include <InputWrapper.h>
#include "NeutralDirector.h"
#include "PlayerDirector.h"
#include <PostMaster.h>
#include <Text.h>
#include <TextMessage.h>
#include "Tutorial.h"
#include <TutorialMessage.h>
#include <XMLReader.h>

Tutorial::Tutorial(const std::string& aXMLPath, const PlayerDirector* aPlayer, const CU::Matrix44<float>& aCameraOrientation
	, const NeutralDirector* aNeutral)
	: myPlayer(aPlayer)
	, myCameraOrientation(aCameraOrientation)
	, myNeutral(aNeutral)
	, myMaxTime(3.0f) // read from XML later
	, myMissions(64)
	, myCurrentMission(0)
	, myMissionComplete(false)
	, myActive(true)
	, myArtifactMission(nullptr)
	, myVictoryPointMission(nullptr)
	, myResourcePointMission(nullptr)
	, myEnemyBaseMission(nullptr)
{
	myPreviousNeutralCount = myNeutral->GetUnitCount();
	myPreviousCameraPosition = myCameraOrientation.GetPos();

	XMLReader reader;
	reader.OpenDocument(aXMLPath);

	tinyxml2::XMLElement* element = reader.ForceFindFirstChild("root");
	element = reader.ForceFindFirstChild(element, "tutorial");
	reader.ForceReadAttribute(element, "time", myMaxTime);
	for (tinyxml2::XMLElement* mission = reader.FindFirstChild(element, "mission"); mission != nullptr
		; mission = reader.FindNextElement(mission, "mission"))
	{
		float time = myMaxTime;
		std::string typeString;
		reader.ForceReadAttribute(mission, "type", typeString);
		reader.ReadAttribute(mission, "time", time);

		std::string textString = reader.ForceFindFirstChild(mission, "text")->GetText();

		Prism::Text* text = new Prism::Text(*Prism::Engine::GetInstance()->GetFont(Prism::eFont::DIALOGUE));
		text->SetText(textString);
		myMissions.Add(Mission(text, GetAction(typeString), time));
	}

	element = reader.FindNextElement(element, "globalTutorial");
	if (element != nullptr)
	{
		for (tinyxml2::XMLElement* mission = reader.FindFirstChild(element, "mission"); mission != nullptr;
			mission = reader.FindNextElement(mission, "mission"))
		{
			float time = myMaxTime;
			std::string typeString;
			reader.ForceReadAttribute(mission, "type", typeString);
			reader.ReadAttribute(mission, "time", time);

			std::string textString = reader.ForceFindFirstChild(mission, "text")->GetText();
			Prism::Text* text = new Prism::Text(*Prism::Engine::GetInstance()->GetFont(Prism::eFont::DIALOGUE));
			text->SetText(textString);

			eAction type = GetSpecialAction(typeString);

			switch (type)
			{
			case Tutorial::eAction::ARTIFACT:
				myArtifactMission = new Mission(text, type, time);
				break;
			case Tutorial::eAction::RESOURCE_POINT:
				myResourcePointMission = new Mission(text, type, time);
				break;
			case Tutorial::eAction::VICTORY_POINT:
				myVictoryPointMission = new Mission(text, type, time);
				break;
			case Tutorial::eAction::ENEMY_BASE:
				myEnemyBaseMission = new Mission(text, type, time);
				break;
			default:
				SAFE_DELETE(text);
				break;
			}
		}
	}
	reader.CloseDocument();

	myCurrentTime = myMaxTime;
	if (myMissions.Size() > 0)
	{
		PostMaster::GetInstance()->SendMessage(TextMessage(myMissions[myCurrentMission].myText));
		myCurrentTime = myMissions[myCurrentMission].myTime;
	}

	PostMaster::GetInstance()->Subscribe(eMessageType::TUTORIAL, this);
	myPlayer->GetBuilding().SetSelectable(false);
}

Tutorial::~Tutorial()
{
	PostMaster::GetInstance()->UnSubscribe(eMessageType::TUTORIAL, this);
	for (int i = 0; i < myMissions.Size(); ++i)
	{
		SAFE_DELETE(myMissions[i].myText);
	}
	if (myArtifactMission != nullptr)
	{
		SAFE_DELETE(myArtifactMission->myText);
	}
	if (myEnemyBaseMission != nullptr)
	{
		SAFE_DELETE(myEnemyBaseMission->myText);
	}
	if (myResourcePointMission != nullptr)
	{
		SAFE_DELETE(myResourcePointMission->myText);
	}
	if (myVictoryPointMission != nullptr)
	{
		SAFE_DELETE(myVictoryPointMission->myText);
	}
	SAFE_DELETE(myArtifactMission);
	SAFE_DELETE(myEnemyBaseMission);
	SAFE_DELETE(myResourcePointMission);
	SAFE_DELETE(myVictoryPointMission);
}

void Tutorial::Update(float aDeltaTime)
{
	myCurrentTime -= aDeltaTime;
	CheckIfCompletedGlobalTutorial();
	if (myActive == false)
	{
		myPlayer->GetBuilding().SetSelectable(true);
		return;
	}

	if (myMissionComplete == false)
	{
		switch (myMissions[myCurrentMission].myAction)
		{
		case eAction::ATTACK:
			if (myNeutral->GetActiveUnitsSize() < myPreviousNeutralCount)
			{
				myMissionComplete = true;
			}
			break;
		case eAction::CLICK:
			PostMaster::GetInstance()->SendMessage(GameStateMessage(eGameState::CLICKABLE_STATE, myMaxTime,
				myMissions[myCurrentMission].myText));
			myMissionComplete = true;
			break;
		case eAction::MOVE_CAMERA:
			if (myCameraOrientation.GetPos() != myPreviousCameraPosition)
			{
				myMissionComplete = true;
			}
			break;
		case eAction::SELECT:
			if (myPlayer->GetSelectedUnits().Size() > 0)
			{
				myMissionComplete = true;
			}
			break;
		}
	}

	if (myCurrentTime < 0)
	{
		if (myMissionComplete == true)
		{
			myCurrentTime = myMaxTime;
			++myCurrentMission;
			if (myCurrentMission < myMissions.Size())
			{
				myCurrentTime = myMissions[myCurrentMission].myTime;
			}
			myMissionComplete = false;

			myPreviousNeutralCount = myNeutral->GetActiveUnitsSize();
			myPreviousCameraPosition = myCameraOrientation.GetPos();

			if (myCurrentMission == myMissions.Size())
			{
				PostMaster::GetInstance()->SendMessage(TextMessage(nullptr, false));
				myActive = false;
			}
			else
			{
				PostMaster::GetInstance()->SendMessage(TextMessage(myMissions[myCurrentMission].myText));
			}
		}
	}
}

void Tutorial::ReceiveMessage(const TutorialMessage& aMessage)
{
	if (aMessage.myAction == eTutorialAction::CLICK)
	{
		myMissionComplete = true;
		myCurrentTime = 0;
	}
	else if (aMessage.myAction == eTutorialAction::ARTIFACT)
	{
		if (myArtifactMission != nullptr)
		{
			myArtifactMission->myIsDone = true;
		}
	}
	else if (aMessage.myAction == eTutorialAction::RESOURCE_POINT)
	{
		if (myResourcePointMission != nullptr)
		{
			myResourcePointMission->myIsDone = true;
		}
	}
	else if (aMessage.myAction == eTutorialAction::VICTORY_POINT)
	{
		if (myVictoryPointMission != nullptr)
		{
			myVictoryPointMission->myIsDone = true;
		}
	}
	else if (aMessage.myAction == eTutorialAction::ENEMY_BASE)
	{
		if (myEnemyBaseMission != nullptr)
		{
			myEnemyBaseMission->myIsDone = true;
		}
	}
}

void Tutorial::CheckIfCompletedGlobalTutorial()
{
	if (myArtifactMission == nullptr && myEnemyBaseMission == nullptr &&
		myResourcePointMission == nullptr && myVictoryPointMission == nullptr)
	{
		return;
	}
	if (myCurrentTime < 0 && myActive == false)
	{
		PostMaster::GetInstance()->SendMessage(TextMessage(nullptr, false));
	}

	if (myArtifactMission != nullptr && myArtifactMission->myIsDone == true && myArtifactMission->myIsActive == true)
	{
		PostMaster::GetInstance()->SendMessage(TextMessage(myArtifactMission->myText));
		myArtifactMission->myIsActive = false;
		myCurrentTime = myArtifactMission->myTime;
	}
	if (myEnemyBaseMission != nullptr && myEnemyBaseMission->myIsDone == true && myEnemyBaseMission->myIsActive == true)
	{
		PostMaster::GetInstance()->SendMessage(TextMessage(myEnemyBaseMission->myText));
		myEnemyBaseMission->myIsActive = false;
		myCurrentTime = myEnemyBaseMission->myTime;
	}
	if (myResourcePointMission != nullptr && myResourcePointMission->myIsDone == true && myResourcePointMission->myIsActive == true)
	{
		PostMaster::GetInstance()->SendMessage(TextMessage(myResourcePointMission->myText));
		myResourcePointMission->myIsActive = false;
		myCurrentTime = myResourcePointMission->myTime;
	}
	if (myVictoryPointMission != nullptr && myVictoryPointMission->myIsDone == true && myVictoryPointMission->myIsActive == true)
	{
		PostMaster::GetInstance()->SendMessage(TextMessage(myVictoryPointMission->myText));
		myVictoryPointMission->myIsActive = false;
		myCurrentTime = myVictoryPointMission->myTime;
	}
}

Tutorial::eAction Tutorial::GetAction(const std::string& anAction) const
{
	if (anAction == "click")
	{
		return eAction::CLICK;
	}
	else if (anAction == "select")
	{
		return eAction::SELECT;
	}
	else if (anAction == "move_camera")
	{
		return eAction::MOVE_CAMERA;
	}
	else if (anAction == "attack")
	{
		return eAction::ATTACK;
	}

	DL_ASSERT("Action not found.");
	return eAction::CLICK;
}

Tutorial::eAction Tutorial::GetSpecialAction(const std::string& anAction) const
{
	if (anAction == "artifact")
	{
		return eAction::ARTIFACT;
	}
	else if (anAction == "victorypoint")
	{
		return eAction::VICTORY_POINT;
	}
	else if (anAction == "resourcepoint")
	{
		return eAction::RESOURCE_POINT;
	}
	else if (anAction == "enemybase")
	{
		return eAction::ENEMY_BASE;
	}

	DL_ASSERT("Action not found.");
	return eAction::CLICK;
}