#include "stdafx.h"

#include <AudioInterface.h>
#include "ClientLevel.h"
#include "ClientLevelFactory.h"
#include "Console.h"
#include <CommonHelper.h>
#include "CompleteGameState.h"
#include <GameStateMessage.h>
#include <EffectContainer.h>
#include <FadeMessage.h>
#include "InGameState.h"
#include <InputWrapper.h>
#include <ModelLoader.h>
#include <MurmurHash3.h>
#include "ServerSelectState.h"
#include <NetMessageAllClientsComplete.h>
#include <NetMessageDisconnect.h>
#include <NetMessageLevelComplete.h>
#include <NetMessageLevelLoaded.h>
#include <NetMessageLoadLevel.h>
#include <NetMessageSetLevel.h>
#include <NetMessageRequestStartLevel.h>
#include <PostMaster.h>
#include <ScriptSystem.h>
#include <VTuneApi.h>
#include <TextProxy.h>
#include <SpriteProxy.h>
#include <Cursor.h>
#include "ClientNetworkManager.h"

InGameState::InGameState(int aLevelID, unsigned int aServerHashLevelValue)
	: myGUIManager(nullptr)
	, myLevelToLoad(aLevelID)
	, myState(eInGameState::LOAD_LEVEL)
	//, myShouldLoadLevel(false)
	//, myShouldShowLoadingScreen(true)
	, myLevel(nullptr)
	//, myLevelComplete(false)
	//, myFailedLevel(false)
	//, myLoadingScreen(true)
	//, myLoadingScreenCanStart(false)
	//, myCanStartNextLevel(false)
	, myFailedLevelHash(false)
	//, myPhysicsDone(false)
	, myServerHashLevelValue(aServerHashLevelValue)
	, myHasStartedMusicBetweenLevels(false)
	, myLastLevel(aLevelID)
	, myCanStartTimer(6.f)
{
	myIsActiveState = false;
	myLevelFactory = new ClientLevelFactory("Data/Level/LI_level.xml");
	myHashLevelValue = Hash(CU::ReadFileToString(myLevelFactory->GetLevelPath(aLevelID)).c_str());
	
	
	/*myLevelCompleteSprite = Prism::ModelLoader::GetInstance()->LoadSprite(
		"Data/Resource/Texture/Menu/BetweenLevels/T_background_elevator.dds", { 1920.f, 1080.f });*/
	myLevelCompleteSprite = Prism::ModelLoader::GetInstance()->LoadSprite(
		"Data/Resource/Texture/Menu/T_background_story01.dds", { 1920.f, 1080.f });
	myLevelFailedSprite = Prism::ModelLoader::GetInstance()->LoadSprite(
		"Data/Resource/Texture/Menu/T_background_gameover.dds", { 1920.f, 1080.f });
	myLoadingScreenSprite = Prism::ModelLoader::GetInstance()->LoadSprite(
		"Data/Resource/Texture/Menu/T_background_story01.dds", { 1920.f, 1080.f });
	myRotatingThing = Prism::ModelLoader::GetInstance()->LoadSprite(
		"Data/Resource/Texture/Menu/T_rotating_thing.dds", { 128.f, 128.f }, { 64.f, 64.f });
	myPressToStart = Prism::ModelLoader::GetInstance()->LoadSprite(
		"Data/Resource/Texture/Menu/T_press_to_start.dds", { 512.f, 64.f });
	CU::Vector2<int> windowSize = Prism::Engine::GetInstance()->GetWindowSizeInt();
	OnResize(windowSize.x, windowSize.y);
}

InGameState::~InGameState()
{
	PostMaster::GetInstance()->UnSubscribe(eMessageType::GAME_STATE, this);
	ClientNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::LEVEL_COMPLETE, this);
	ClientNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::ALL_CLIENTS_COMPLETE, this);
	ClientNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::LOAD_LEVEL, this);
	Console::Destroy();
	SAFE_DELETE(myLevelCompleteSprite);
	SAFE_DELETE(myLevelFailedSprite);
	SAFE_DELETE(myLoadingScreenSprite);
	SAFE_DELETE(myRotatingThing);
	SAFE_DELETE(myPressToStart);
	SAFE_DELETE(myLevel);
	SAFE_DELETE(myLevelFactory);
	SAFE_DELETE(myText);
}

void InGameState::InitState(StateStackProxy* aStateStackProxy, GUI::Cursor* aCursor)
{
	myIsLetThrough = false;
	myStateStack = aStateStackProxy;
	myStateStatus = eStateStatus::eKeepState;
	myCursor = aCursor;
	myCursor->SetShouldRender(false);

	//PostMaster::GetInstance()->SendMessage(RunScriptMessage("Data/Script/Autorun.script"));
	//myLevel = static_cast<ClientLevel*>(myLevelFactory->LoadLevel(myLevelToLoad));

	myIsActiveState = true;

	myText = Prism::ModelLoader::GetInstance()->LoadText(Prism::Engine::GetInstance()->GetFont(Prism::eFont::CONSOLE));
	myText->SetPosition(CU::Vector2<float>(800.f, 800.f));

	PostMaster::GetInstance()->Subscribe(eMessageType::GAME_STATE, this);
	ClientNetworkManager::GetInstance()->Subscribe(eNetMessageType::LEVEL_COMPLETE, this);
	ClientNetworkManager::GetInstance()->Subscribe(eNetMessageType::ALL_CLIENTS_COMPLETE, this);
	ClientNetworkManager::GetInstance()->Subscribe(eNetMessageType::LOAD_LEVEL, this);

	if (myHashLevelValue != myServerHashLevelValue)
	{
		ClientNetworkManager::GetInstance()->AddMessage(NetMessageDisconnect());
		myStateStatus = eStateStatus::ePopMainState;
		myFailedLevelHash = true;
		DL_MESSAGE_BOX("Level don't match the server level. Please update and try again.", "Failed to play level.", MB_OK | MB_ICONWARNING);
	}
}

void InGameState::EndState()
{
}

const eStateStatus InGameState::Update(const float& aDeltaTime)
{
	if (myFailedLevelHash == true)
	{
		return eStateStatus::ePopMainState;
	}

	myRotatingThing->Rotate(-aDeltaTime * 8.f);

	switch (myState)
	{
	case eInGameState::LOADING_SCREEN:
		myLevel->Update(aDeltaTime, true);
		if (myLevel->GetInitDone() == true)
		{
			myState = eInGameState::LOADING_PHYSICS_INIT;
		}
		break;
	case eInGameState::LOADING_PHYSICS_INIT:
		myLevel->Update(aDeltaTime, true);
		if (Prism::ModelLoader::GetInstance()->IsLoading() == false)
		{
			ClientNetworkManager::GetInstance()->AddMessage(NetMessageLevelLoaded());
			myState = eInGameState::LOADING_GRAPHICS_INIT;
		}
		break;
	case eInGameState::LOADING_GRAPHICS_INIT:
		myLevel->Update(aDeltaTime, true);
		break;
	case eInGameState::LOADING_CAN_START:
		myLevel->Update(aDeltaTime, true);
		if (CU::InputWrapper::GetInstance()->KeyDown(DIK_RETURN) == true
			|| CU::InputWrapper::GetInstance()->KeyDown(DIK_ESCAPE) == true
			|| CU::InputWrapper::GetInstance()->KeyDown(DIK_SPACE) == true
			|| CU::InputWrapper::GetInstance()->MouseDown(0) == true
			|| CU::InputWrapper::GetInstance()->MouseDown(1) == true
			|| CU::InputWrapper::GetInstance()->MouseDown(2) == true
			|| CU::InputWrapper::GetInstance()->AnyKeyDown() == true)
		{
			PostMaster::GetInstance()->SendMessage(FadeMessage(1.f));
			myState = eInGameState::LEVEL;
		}
		break;
	case eInGameState::LEVEL:
		if (CU::InputWrapper::GetInstance()->KeyDown(DIK_ESCAPE))
		{
			myLevel->ToggleEscapeMenu();
		}
		myLevel->Update(aDeltaTime, false);
		break;
	case eInGameState::LOAD_LEVEL:
		SET_RUNTIME(false);
		SAFE_DELETE(myLevel);
		myLevel = static_cast<ClientLevel*>(myLevelFactory->LoadLevel(myLevelToLoad, myCursor, myStateStatus));

		myLastLevel = myLevelToLoad;
		myLevelToLoad = -1;
		myState = eInGameState::LOADING_SCREEN;
		break;
	case eInGameState::LEVEL_COMPLETE:
		myLevelCompleteSprite->Render({ 0.f, 0.f });
		break;
	case eInGameState::LEVEL_COMPLETE_CAN_START:
		myLevelCompleteSprite->Render({ 0.f, 0.f });

		myCanStartTimer -= aDeltaTime;
		if (myCanStartTimer <= 0.f && (CU::InputWrapper::GetInstance()->KeyDown(DIK_RETURN) == true
			|| CU::InputWrapper::GetInstance()->AnyKeyDown() == true))
		{
			ClientNetworkManager::GetInstance()->AddMessage(NetMessageRequestStartLevel());
			myState = eInGameState::LEVEL_COMPLETE;
		}
		break;

	case eInGameState::LEVEL_FAIL:
		myLevelFailedSprite->Render({ 0.f, 0.f });
		break;
	case eInGameState::LEVEL_FAIL_CAN_START:
		myLevelFailedSprite->Render({ 0.f, 0.f });

		myCanStartTimer -= aDeltaTime;
		if (myCanStartTimer <= 0.f && (CU::InputWrapper::GetInstance()->KeyDown(DIK_RETURN) == true
			|| CU::InputWrapper::GetInstance()->AnyKeyDown() == true))
		{
			ClientNetworkManager::GetInstance()->AddMessage(NetMessageRequestStartLevel());
			myState = eInGameState::LEVEL_FAIL;
		}
		break;

	default:
		DL_ASSERT("Unknown state.");
	}

	//if (myLoadingScreen == true && myLoadingScreenCanStart == false && myPhysicsDone == true)
	//{

	//}

	//if (myShouldShowLoadingScreen == true)
	//{
	//	myShouldShowLoadingScreen = false;
	//	myShouldLoadLevel = true;
	//	myLoadingScreen = true;
	//	myLoadingScreenCanStart = false;
	//	myLevelComplete = false;
	//	myFailedLevel = false;
	//	myCanStartNextLevel = false;
	//	myPhysicsDone = false;
	//	return myStateStatus;
	//}

	//
	//if (myLoadingScreen == true)
	//{
	//	myRotatingThing->Rotate(-aDeltaTime * 8.f);
	//}
	//else
	//{
	//	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_ESCAPE))
	//	{
	//		myLevel->ToggleEscapeMenu();
	//	}
	//}

	//if (myLoadingScreenCanStart == true)
	//{
	//	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_RETURN) == true
	//		|| CU::InputWrapper::GetInstance()->KeyDown(DIK_ESCAPE) == true
	//		|| CU::InputWrapper::GetInstance()->KeyDown(DIK_SPACE) == true
	//		|| CU::InputWrapper::GetInstance()->MouseDown(0) == true
	//		|| CU::InputWrapper::GetInstance()->MouseDown(1) == true
	//		|| CU::InputWrapper::GetInstance()->MouseDown(2) == true
	//		|| CU::InputWrapper::GetInstance()->KeyDown(DIK_W) == true
	//		|| CU::InputWrapper::GetInstance()->KeyDown(DIK_A) == true
	//		|| CU::InputWrapper::GetInstance()->KeyDown(DIK_S) == true
	//		|| CU::InputWrapper::GetInstance()->KeyDown(DIK_D) == true
	//		|| CU::InputWrapper::GetInstance()->KeyDown(DIK_E) == true
	//		|| CU::InputWrapper::GetInstance()->KeyDown(DIK_1) == true
	//		|| CU::InputWrapper::GetInstance()->KeyDown(DIK_2) == true
	//		|| CU::InputWrapper::GetInstance()->KeyDown(DIK_3) == true)
	//	{
	//		myLoadingScreen = false;
	//		myLoadingScreenCanStart = false;
	//		PostMaster::GetInstance()->SendMessage(FadeMessage(1.f));
	//	}
	//}
	//
	//if (myShouldLoadLevel == true)
	//{
	//	myShouldLoadLevel = false;
	//	SET_RUNTIME(false);
	//	SAFE_DELETE(myLevel);
	//	myLevel = static_cast<ClientLevel*>(myLevelFactory->LoadLevel(myLevelToLoad, myCursor, myStateStatus));

	//	myLastLevel = myLevelToLoad;
	//	myLevelToLoad = -1;
	//}

	//Prism::EffectContainer::GetInstance()->Update(aDeltaTime);



	//if (myLevelComplete == true)
	//{
	//	myLevelCompleteSprite->Render({ 0.f, 0.f });
	//	if (myHasStartedMusicBetweenLevels == false)
	//	{
	//		//int levelMusic = myLastLevel + 1;
	//		//std::string musicEvent("Play_ElevatorToLevel" + std::to_string(levelMusic));
	//		//myHasStartedMusicBetweenLevels = true;
	//		//Prism::Audio::AudioInterface::GetInstance()->PostEvent(musicEvent.c_str(), 0);
	//	}

	//	if (myCanStartNextLevel == true)
	//	{
	//		DEBUG_PRINT("Press space to continue!");
	//		myText->SetText("Press space to continue");
	//		if (CU::InputWrapper::GetInstance()->KeyDown(DIK_SPACE) == true)
	//		{
	//			ClientNetworkManager::GetInstance()->AddMessage(NetMessageRequestStartLevel());
	//		}
	//	}
	//}
	//else if (myFailedLevel == true)
	//{
	//	myLevelFailedSprite->Render({ 0.f, 0.f });
	//	if (myHasStartedMusicBetweenLevels == false)
	//	{
	//		//int levelMusic = myLastLevel + 1;
	//		//std::string musicEvent("Play_ElevatorToLevel" + std::to_string(levelMusic));
	//		//myHasStartedMusicBetweenLevels = true;
	//		//Prism::Audio::AudioInterface::GetInstance()->PostEvent(musicEvent.c_str(), 0);
	//	}

	//	if (myCanStartNextLevel == true)
	//	{
	//		DEBUG_PRINT("Press space to continue!");
	//		myText->SetText("Press space to continue");
	//		if (CU::InputWrapper::GetInstance()->KeyDown(DIK_SPACE) == true)
	//		{
	//			ClientNetworkManager::GetInstance()->AddMessage(NetMessageRequestStartLevel());
	//		}
	//	}
	//}
	//else
	//{
	//	DL_ASSERT_EXP(myLevel != nullptr, "Invalid level");
	//	myLevel->Update(aDeltaTime, myLoadingScreen || firstFrame);
	//	myPhysicsDone = myLevel->GetInitDone();
	//}

	//LUA::ScriptSystem::GetInstance()->CallFunction("Update", { aDeltaTime });
	//LUA::ScriptSystem::GetInstance()->Update();

	return myStateStatus;
}

void InGameState::Render()
{
	VTUNE_EVENT_BEGIN(VTUNE::GAME_RENDER);

	switch (myState)
	{
	case eInGameState::LOADING_SCREEN:
		myLoadingScreenSprite->Render({ 0.f, 0.f });
		myRotatingThing->Render({ 84.f, 84.f }, { 1.f, 1.f });
		break;
	case eInGameState::LOADING_PHYSICS_INIT:
		myLoadingScreenSprite->Render({ 0.f, 0.f });
		myRotatingThing->Render({ 84.f, 84.f }, { 1.f, 1.f });
		break;
	case eInGameState::LOADING_GRAPHICS_INIT:
		myLoadingScreenSprite->Render({ 0.f, 0.f });
		myRotatingThing->Render({ 84.f, 84.f }, { 1.f, 1.f });
		break;
	case eInGameState::LOADING_CAN_START:
		myLoadingScreenSprite->Render({ 0.f, 0.f });
		myPressToStart->Render({ 200.f, 20.f });
		break;
	case eInGameState::LEVEL:
		myLevel->Render();
		break;
	case eInGameState::LOAD_LEVEL:
		myLoadingScreenSprite->Render({ 0.f, 0.f });
		break;
	case eInGameState::LEVEL_FAIL:
		myLevelFailedSprite->Render({ 0.f, 0.f });
		break;
	case eInGameState::LEVEL_FAIL_CAN_START:
		myLevelFailedSprite->Render({ 0.f, 0.f });
		if (myCanStartTimer <= 0.f)
		{
			myPressToStart->Render({ 200.f, 20.f });
		}
		break;
	case eInGameState::LEVEL_COMPLETE:
		myLevelCompleteSprite->Render({ 0.f, 0.f });
		break;
	case eInGameState::LEVEL_COMPLETE_CAN_START:
		myLevelCompleteSprite->Render({ 0.f, 0.f });
		if (myCanStartTimer <= 0.f)
		{
			myPressToStart->Render({ 200.f, 20.f });
		}
		break;
	default:
		break;
	}



	//if (myLoadingScreen == true)
	//{
	//	myLoadingScreenSprite->Render({ 0.f, 0.f });
	//	if (myLoadingScreenCanStart == true)
	//	{
	//		myPressToStart->Render({ 200.f, 20.f });
	//	}
	//	else
	//	{
	//		myRotatingThing->Render({ 84.f, 84.f }, { 1.f, 1.f });
	//	}
	//}
	//else if (myLevelComplete == false && myFailedLevel == false)
	//{
	//	myLevel->Render();
	//}
	//else
	//{
	//	myText->Render();
	//}

	VTUNE_EVENT_END();
}

void InGameState::ResumeState()
{
	myIsActiveState = true;
	PostMaster::GetInstance()->SendMessage(FadeMessage(1.f / 3.f));
	myLevelToLoad = -1;
}

void InGameState::ReceiveMessage(const GameStateMessage& aMessage)
{
	if (myState != eInGameState::LEVEL)
	{
		switch (aMessage.myGameState)
		{
		case eGameState::LOAD_LEVEL:
			myLevelToLoad = aMessage.myID;
			myState = eInGameState::LOAD_LEVEL;
			break;
		}
	}
}

void InGameState::ReceiveNetworkMessage(const NetMessageAllClientsComplete& aMessage, const sockaddr_in&)
{
	DL_ASSERT_EXP(aMessage.myType == NetMessageAllClientsComplete::eType::LEVEL_COMPLETE
		|| aMessage.myType == NetMessageAllClientsComplete::eType::LEVEL_LOAD, "Unknown All clients complete message type");

	switch (aMessage.myType)
	{
	case NetMessageAllClientsComplete::eType::LEVEL_COMPLETE:
		if (myState == eInGameState::LEVEL_FAIL)
		{
			myState = eInGameState::LEVEL_FAIL_CAN_START;
		}
		if (myState == eInGameState::LEVEL_COMPLETE)
		{
			myState = eInGameState::LEVEL_COMPLETE_CAN_START;
		}
		myCanStartTimer = 6.f;
		//DL_ASSERT_EXP(myLevelToLoad >= 0, "Wrong level");
		//myState = eInGameState::LOAD_LEVEL;
		break;
	case NetMessageAllClientsComplete::eType::LEVEL_LOAD:
		//myLevelComplete = false;
		//myFailedLevel = false;
		//myLoadingScreenCanStart = true;
		myState = eInGameState::LOADING_CAN_START;
		break;
	}
}

void InGameState::ReceiveNetworkMessage(const NetMessageLevelComplete& aMsg, const sockaddr_in&)
{
	SAFE_DELETE(myLevel);
	if (myLastLevel == 3 && aMsg.myAllPlayersDied == false)
	{
		myStateStack->PushSubGameState(new CompleteGameState());
	}
	else
	{
		ClientNetworkManager::GetInstance()->AddMessage(NetMessageLevelComplete(aMsg.myAllPlayersDied));
	}
	ClientNetworkManager::GetInstance()->AllowSendWithoutSubscriber(true);
	myHasStartedMusicBetweenLevels = false;

	if (aMsg.myAllPlayersDied == true)
	{
		//myFailedLevel = true;
		myState = eInGameState::LEVEL_FAIL;
		//DL_ASSERT_EXP(myLevelComplete == false, "Level error");
	}
	else
	{
		myState = eInGameState::LEVEL_COMPLETE;
		//myLevelComplete = true;
		//DL_ASSERT_EXP(myFailedLevel == false, "Level error");
	}
}

void InGameState::ReceiveNetworkMessage(const NetMessageLoadLevel& aMessage, const sockaddr_in&)
{
	if (myLevel != nullptr)
	{
		bool shouldAssertLater = true;
		return;
	}
	//DL_ASSERT_EXP(myLevel == nullptr, "Level has to be nullptr here");
	//int levelMusic = myLastLevel + 1;
	//std::string musicEvent("Stop_ElevatorToLevel" + std::to_string(levelMusic));
	//myHasStartedMusicBetweenLevels = true;
	//Prism::Audio::AudioInterface::GetInstance()->PostEvent(musicEvent.c_str(), 0);

	if (myState != eInGameState::LEVEL)
	{
		myLevelToLoad = aMessage.myLevelID;
		//myShouldShowLoadingScreen = true;
		myState = eInGameState::LOAD_LEVEL;
	}
}

void InGameState::OnResize(int aWidth, int aHeight)
{
	myLevelCompleteSprite->SetSize({ float(aWidth), float(aHeight) }, { 0.f, 0.f });
	myLevelFailedSprite->SetSize({ float(aWidth), float(aHeight) }, { 0.f, 0.f });
	myLoadingScreenSprite->SetSize({ float(aWidth), float(aHeight) }, { 0.f, 0.f });
	if (myLevel != nullptr)
	{
		myLevel->OnResize(float(aWidth), float(aHeight));
	}
}