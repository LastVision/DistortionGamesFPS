#include "stdafx.h"

#include <AudioInterface.h>
#include "ClientNetworkManager.h"
#include <Cursor.h>
#include <fstream>
#include <GUIManager.h>
#include "InGameState.h"
#include <InputWrapper.h>
#include "LobbyState.h"
#include <ModelLoader.h>
#include <NetMessageDisconnect.h>
#include <NetMessageLoadLevel.h>
#include <NetMessageSetLevel.h>
#include <NetMessageRequestLevel.h>
#include <NetMessageRequestStartLevel.h>
#include <OnClickMessage.h>
#include <OnRadioButtonMessage.h>
#include <PostMaster.h>
#include <SharedNetworkManager.h>
#include <SpriteProxy.h>
#include <TextProxy.h>
#include <FadeMessage.h>

#include <WidgetContainer.h>

LobbyState::LobbyState(bool aShowStartButton)
	: myGUIManager(nullptr)
	, myGUIManagerHost(nullptr)
	, myLevelToStart(-1)
	, myStartGame(false)
	, myServerLevelHash(0)
	, myRefreshPlayerListTimer(0)
	, myRadioLevel(0)
	, myRadioDifficulty(10)
	, myShowStartButton(aShowStartButton)
{
}


LobbyState::~LobbyState()
{
	SAFE_DELETE(myGUIManager);
	SAFE_DELETE(myGUIManagerHost);
	SAFE_DELETE(myText);
	SAFE_DELETE(myPlayerListText);
	SAFE_DELETE(myWaitingForHostText);
	myCursor = nullptr;
	PostMaster::GetInstance()->UnSubscribe(eMessageType::ON_CLICK, this);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::ON_RADIO_BUTTON, this);
	SharedNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::ON_DISCONNECT, this);
	SharedNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::SET_LEVEL, this);
	SharedNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::LOAD_LEVEL, this);
}

void LobbyState::InitState(StateStackProxy* aStateStackProxy, GUI::Cursor* aCursor)
{
	PostMaster::GetInstance()->Subscribe(eMessageType::ON_CLICK, this);
	PostMaster::GetInstance()->Subscribe(eMessageType::ON_RADIO_BUTTON, this);
	SharedNetworkManager::GetInstance()->Subscribe(eNetMessageType::ON_DISCONNECT, this);
	SharedNetworkManager::GetInstance()->Subscribe(eNetMessageType::SET_LEVEL, this);
	SharedNetworkManager::GetInstance()->Subscribe(eNetMessageType::LOAD_LEVEL, this);
	myCursor = aCursor;
	myIsActiveState = true;
	myIsLetThrough = false;
	myStateStatus = eStateStatus::eKeepState;
	myStateStack = aStateStackProxy;

	myGUIManager = new GUI::GUIManager(myCursor, "Data/Resource/GUI/GUI_Lobby.xml", nullptr, -1);
	myGUIManagerHost = new GUI::GUIManager(myCursor, "Data/Resource/GUI/GUI_Lobby_host.xml", nullptr, -1);

	if (myShowStartButton == false)
	{
		static_cast<GUI::WidgetContainer*>(myGUIManagerHost->GetWidgetContainer()->At(0))->At(3)->SetVisibility(false);
	}

	myText = Prism::ModelLoader::GetInstance()->LoadText(Prism::Engine::GetInstance()->GetFont(Prism::eFont::CONSOLE));
	myPlayerListText = Prism::ModelLoader::GetInstance()->LoadText(Prism::Engine::GetInstance()->GetFont(Prism::eFont::CONSOLE));
	myWaitingForHostText = Prism::ModelLoader::GetInstance()->LoadText(Prism::Engine::GetInstance()->GetFont(Prism::eFont::CONSOLE));
	myWaitingForJoinText = Prism::ModelLoader::GetInstance()->LoadText(Prism::Engine::GetInstance()->GetFont(Prism::eFont::CONSOLE));
	Prism::ModelLoader::GetInstance()->WaitUntilFinished();


	myText->SetPosition({ 800.f, 800.f });
	myText->SetText("No current level selected.");
	myText->SetScale({ 1.f, 1.f });

	myPlayerListText->SetPosition({ 20, 800.f });
	myPlayerListText->SetText("Player Online:\n");
	myPlayerListText->SetScale({ 1.f, 1.f });

	myWaitingForHostText->SetPosition({ 800.f - 50.f, 200.f });
	myWaitingForHostText->SetText("Waiting for host...");
	myWaitingForHostText->SetScale({ 1.f, 1.f });

	

	const CU::Vector2<int>& windowSize = Prism::Engine::GetInstance()->GetWindowSizeInt();
	OnResize(windowSize.x, windowSize.y);

	myWaitingForJoinText->SetPosition({ windowSize.x / 2.f - 163.f, 0.f + 93.f });
	myWaitingForJoinText->SetText("Waiting for other player");
	myWaitingForJoinText->SetScale({ 1.f, 1.f });

	myCursor->SetShouldRender(true);
	myLevelToStart = -1;

	//Prism::Audio::AudioInterface::GetInstance()->PostEvent("Stop_MainMenu", 0);
	if (ClientNetworkManager::GetInstance()->GetGID() > 1)
	{
		ClientNetworkManager::GetInstance()->AddMessage(NetMessageRequestLevel());
	}
	myWaitingForConnectTimer = 1.f;

	myRotatingThingy = Prism::ModelLoader::GetInstance()->LoadSprite(
		"Data/Resource/Texture/Menu/T_rotating_thing.dds", { 128.f, 128.f }, { 64.f, 64.f });

	PostMaster::GetInstance()->SendMessage(FadeMessage(1.f / 3.f));
}

void LobbyState::EndState()
{
	myIsActiveState = false;
	myCursor->SetShouldRender(false);
	myLevelToStart = -1;
}

void LobbyState::OnResize(int aX, int aY)
{
	myGUIManager->OnResize(aX, aY);
	myWaitingForJoinText->SetPosition({ aX / 2.f - 163.f, 0.f + 93.f });
}

const eStateStatus LobbyState::Update(const float& aDeltaTime)
{
	if (ClientNetworkManager::GetInstance()->GetGID() == 0)
	{
		myWaitingForConnectTimer -= aDeltaTime;
		if (myWaitingForConnectTimer <= 0.f)
		{
			ClientNetworkManager::GetInstance()->SetHasLeftLobby(true);
			ClientNetworkManager::GetInstance()->AddMessage(NetMessageDisconnect(0));
			return eStateStatus::ePopMainState;
		}
		return myStateStatus;
	}
	if (ClientNetworkManager::GetInstance()->GetGID() == 1)
	{
		if (CU::InputWrapper::GetInstance()->KeyDown(DIK_ESCAPE) == true)
		{
			ClientNetworkManager::GetInstance()->SetHasLeftLobby(true);
			ClientNetworkManager::GetInstance()->AddMessage(NetMessageDisconnect(ClientNetworkManager::GetInstance()->GetGID()));
			return eStateStatus::ePopMainState;
		}
	}

	if (ClientNetworkManager::GetInstance()->GetClients().Size() > 0)
	{
		myShowStartButton = true;
		static_cast<GUI::WidgetContainer*>(myGUIManagerHost->GetWidgetContainer()->At(0))->At(3)->SetVisibility(true);
	}

	myRefreshPlayerListTimer -= aDeltaTime;
	if (myRefreshPlayerListTimer <= 0)
	{
		myRefreshPlayerListTimer = 1.f;
		std::string playerListText = "Players Online:\n";
		playerListText += ClientNetworkManager::GetInstance()->GetName() + "\n";
		const CU::GrowingArray<OtherClients>& playerList = ClientNetworkManager::GetInstance()->GetClients();
		for each (const OtherClients& player in playerList)
		{
			playerListText += player.myName + "\n";
		}
		myPlayerListText->SetText(playerListText);
	}

	ClientNetworkManager::GetInstance()->DebugPrint();
//#ifndef RELEASE_BUILD
	if (ClientNetworkManager::GetInstance()->GetGID() == 1)
	{
		if (CU::InputWrapper::GetInstance()->KeyDown(DIK_SPACE) == true)
		{
			ClientNetworkManager::GetInstance()->AddMessage(NetMessageRequestStartLevel());
		}
	}
//#endif

	if (myStartGame == true)
	{
		DL_ASSERT_EXP(myLevelToStart != -1, "Can't start level -1.");
		PostMaster::GetInstance()->UnSubscribe(eMessageType::ON_CLICK, this);
		PostMaster::GetInstance()->UnSubscribe(eMessageType::ON_RADIO_BUTTON, this);
		SharedNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::ON_DISCONNECT, this);
		SharedNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::SET_LEVEL, this);
		SharedNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::LOAD_LEVEL, this);

		SET_RUNTIME(false);
		myStateStack->PushSubGameState(new InGameState(myLevelToStart, myServerLevelHash));
	}

	if (ClientNetworkManager::GetInstance()->GetGID() == 1)
	{
		myGUIManagerHost->Update(aDeltaTime);
	}
	else
	{
		myRotatingThingy->Rotate(-aDeltaTime * 8.f);
		myGUIManager->Update(aDeltaTime);
	}

	return myStateStatus;
}

void LobbyState::Render()
{
	if (ClientNetworkManager::GetInstance()->GetGID() == 1)
	{
		myGUIManagerHost->Render();
		myPlayerListText->Render();
	}
	else
	{
		myGUIManager->Render();
		myPlayerListText->Render();
		myWaitingForHostText->Render();
		myText->Render();
		myRotatingThingy->Render({ 800.f + myText->GetWidth() + 35.f, 200.f + 7.f });
	}

	if (myShowStartButton == false)
	{
		myWaitingForJoinText->Render();
	}

	DEBUG_PRINT(myLevelToStart+1);
}

void LobbyState::ResumeState()
{
	myStartGame = false;
	myIsActiveState = true;
	myLevelToStart = -1;
	PostMaster::GetInstance()->SendMessage(FadeMessage(1.f / 3.f));
}

void LobbyState::ReceiveMessage(const OnClickMessage& aMessage)
{
	if (myIsActiveState == true)
	{
		switch (aMessage.myEvent)
		{
		case eOnClickEvent::START_GAME:
			ClientNetworkManager::GetInstance()->AddMessage(NetMessageRequestStartLevel());
			break;
		case eOnClickEvent::GAME_QUIT:
			//Prism::Audio::AudioInterface::GetInstance()->PostEvent("Stop_AllElevators", 0);
			ClientNetworkManager::GetInstance()->SetHasLeftLobby(true);
			ClientNetworkManager::GetInstance()->AddMessage(NetMessageDisconnect(ClientNetworkManager::GetInstance()->GetGID()));
			myStateStatus = eStateStatus::ePopMainState;
			break;
		default:
			DL_ASSERT("Unknown event.");
			break;
		}
	}
}

void LobbyState::ReceiveMessage(const OnRadioButtonMessage& aMessage)
{
	DL_ASSERT_EXP(aMessage.myEvent == eOnRadioButtonEvent::LEVEL_SELECT
		|| aMessage.myEvent == eOnRadioButtonEvent::DIFFICULTY_SELECT, "Only level select in lobby state.");

	if (aMessage.myEvent == eOnRadioButtonEvent::LEVEL_SELECT)
	{
		//int levelMusic = aMessage.myID;
		//levelMusic = min(levelMusic, 2);
		//std::string musicEvent("Play_ElevatorToLevel" + std::to_string(levelMusic));
		//Prism::Audio::AudioInterface::GetInstance()->PostEvent(musicEvent.c_str(), 0);
		
		myRadioLevel = aMessage.myID;
	}
	else if (aMessage.myEvent == eOnRadioButtonEvent::DIFFICULTY_SELECT)
	{
		myRadioDifficulty = aMessage.myID;
	}

	if (ClientNetworkManager::GetInstance()->GetGID() == 1)
	{
		ClientNetworkManager::GetInstance()->AddMessage(NetMessageSetLevel(myRadioLevel + myRadioDifficulty));
	}
}

void LobbyState::ReceiveNetworkMessage(const NetMessageDisconnect& aMessage, const sockaddr_in&)
{
	if (aMessage.myClientID == ClientNetworkManager::GetInstance()->GetGID() || ClientNetworkManager::GetInstance()->GetGID() == 0)
	{
		myStateStatus = eStateStatus::ePopMainState;
	}
}

void LobbyState::ReceiveNetworkMessage(const NetMessageSetLevel& aMessage, const sockaddr_in&)
{
	//Prism::Audio::AudioInterface::GetInstance()->PostEvent("Stop_AllElevators", 0);

	//int levelMusic = aMessage.myLevelID;
	//levelMusic = min(levelMusic, 2);
	//std::string musicEvent("Play_ElevatorToLevel" + std::to_string(levelMusic));
	//Prism::Audio::AudioInterface::GetInstance()->PostEvent(musicEvent.c_str(), 0);

	myLevelToStart = aMessage.myLevelID;
	myText->SetText(CU::Concatenate("Current level: %i", min(myLevelToStart+1, 3)));
}

void LobbyState::ReceiveNetworkMessage(const NetMessageLoadLevel& aMessage, const sockaddr_in&)
{
	myLevelToStart = aMessage.myLevelID;
	myServerLevelHash = aMessage.myLevelHash;

	myStartGame = true;
}
