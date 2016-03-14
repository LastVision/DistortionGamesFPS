#include "stdafx.h"

#include "ClientNetworkManager.h"
#include <Cursor.h>
#include <fstream>
#include <GUIManager.h>
#include "InGameState.h"
#include <InputWrapper.h>
#include "LobbyState.h"
#include <NetMessageRequestStartGame.h>
#include <OnClickMessage.h>
#include <PostMaster.h>


LobbyState::LobbyState()
	: myGUIManager(nullptr)
{
}


LobbyState::~LobbyState()
{
	SAFE_DELETE(myGUIManager);
	myCursor = nullptr;
	PostMaster::GetInstance()->UnSubscribe(eMessageType::ON_CLICK, this);
}

void LobbyState::InitState(StateStackProxy* aStateStackProxy, GUI::Cursor* aCursor)
{
	PostMaster::GetInstance()->Subscribe(eMessageType::ON_CLICK, this);
	myCursor = aCursor;
	myIsActiveState = true;
	myIsLetThrough = false;
	myStateStatus = eStateStatus::eKeepState;
	myStateStack = aStateStackProxy;

	myGUIManager = new GUI::GUIManager(myCursor, "Data/Resource/GUI/GUI_Lobby.xml", nullptr, -1);

	const CU::Vector2<int>& windowSize = Prism::Engine::GetInstance()->GetWindowSizeInt();
	OnResize(windowSize.x, windowSize.y);

	myCursor->SetShouldRender(true);
}

void LobbyState::EndState()
{
	myIsActiveState = false;
	myCursor->SetShouldRender(false);
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
		bool ShouldDisconnectFromServerHere = true;
		return eStateStatus::ePopSubState;
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
}

void LobbyState::ReceiveMessage(const OnClickMessage& aMessage)
{
	if (myIsActiveState == true)
	{
		switch (aMessage.myEvent)
		{
		case eOnClickEvent::START_GAME:
			//myStateStack->PushSubGameState(new InGameState());
			ClientNetworkManager::GetInstance()->AddMessage(NetMessageRequestStartGame());
			break;
		default:
			DL_ASSERT("Unknown event.");
			break;
		}
	}
}