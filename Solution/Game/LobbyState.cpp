#include "stdafx.h"

#include "ClientNetworkManager.h"
#include <Cursor.h>
#include <fstream>
#include <GUIManager.h>
#include "InGameState.h"
#include <InputWrapper.h>
#include "LobbyState.h"
#include <NetMessageDisconnect.h>
#include <NetMessageStartGame.h>
#include <NetMessageRequestLevel.h>
#include <NetMessageRequestStartGame.h>
#include <OnClickMessage.h>
#include <OnRadioButtonMessage.h>
#include <PostMaster.h>
#include <SharedNetworkManager.h>

LobbyState::LobbyState()
	: myGUIManager(nullptr)
	, myLevelToStart(-1)
{
}


LobbyState::~LobbyState()
{
	SAFE_DELETE(myGUIManager);
	myCursor = nullptr;
	PostMaster::GetInstance()->UnSubscribe(eMessageType::ON_CLICK, this);
	SharedNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::START_GAME, this);
}

void LobbyState::InitState(StateStackProxy* aStateStackProxy, GUI::Cursor* aCursor)
{
	PostMaster::GetInstance()->Subscribe(eMessageType::ON_CLICK, this);
	SharedNetworkManager::GetInstance()->Subscribe(eNetMessageType::START_GAME, this);
	myCursor = aCursor;
	myIsActiveState = true;
	myIsLetThrough = false;
	myStateStatus = eStateStatus::eKeepState;
	myStateStack = aStateStackProxy;

	myGUIManager = new GUI::GUIManager(myCursor, "Data/Resource/GUI/GUI_Lobby.xml", nullptr, -1);

	const CU::Vector2<int>& windowSize = Prism::Engine::GetInstance()->GetWindowSizeInt();
	OnResize(windowSize.x, windowSize.y);

	myCursor->SetShouldRender(true);
	myLevelToStart = -1;
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
}

const eStateStatus LobbyState::Update(const float& aDeltaTime)
{
	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_ESCAPE) == true
		|| CU::InputWrapper::GetInstance()->KeyDown(DIK_N) == true)
	{
		ClientNetworkManager::GetInstance()->AddMessage(NetMessageDisconnect(ClientNetworkManager::GetInstance()->GetGID()));
		return eStateStatus::ePopMainState;
	}

	ClientNetworkManager::GetInstance()->DebugPrint();

	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_SPACE) == true)
	{
		ClientNetworkManager::GetInstance()->AddMessage(NetMessageRequestStartGame());
	}

	if (myLevelToStart != -1)
	{
		PostMaster::GetInstance()->UnSubscribe(eMessageType::ON_CLICK, this);
		SET_RUNTIME(false);
		myStateStack->PushSubGameState(new InGameState(myLevelToStart));
	}

	myGUIManager->Update(aDeltaTime);

	return myStateStatus;
}

void LobbyState::Render()
{
	myGUIManager->Render();
}

void LobbyState::ResumeState()
{
	myIsActiveState = true;
	myLevelToStart = -1;
}

void LobbyState::ReceiveMessage(const OnClickMessage& aMessage)
{
	if (myIsActiveState == true)
	{
		switch (aMessage.myEvent)
		{
		case eOnClickEvent::START_GAME:
			ClientNetworkManager::GetInstance()->AddMessage(NetMessageRequestStartGame());
			break;
		default:
			DL_ASSERT("Unknown event.");
			break;
		}
	}
}

void LobbyState::ReceiveMessage(const OnRadioButtonMessage& aMessage)
{
	DL_ASSERT_EXP(aMessage.myEvent == eOnRadioButtonEvent::LEVEL_SELECT, "Only level select in lobby state.");

	ClientNetworkManager::GetInstance()->AddMessage(NetMessageRequestLevel(aMessage.myID));
}

void LobbyState::ReceiveNetworkMessage(const NetMessageStartGame& aMessage, const sockaddr_in&)
{
	myLevelToStart = aMessage.myLevelID;
}
