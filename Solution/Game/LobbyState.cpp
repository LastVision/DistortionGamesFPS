#include "stdafx.h"
#include "LobbyState.h"
#include <Cursor.h>
#include <fstream>
#include <GUIManager.h>
#include <InputWrapper.h>
#include "ClientNetworkManager.h"
#include <PostMaster.h>
#include <OnClickMessage.h>


LobbyState::LobbyState()
	: myGUIManager(nullptr)
	, myServer(nullptr)
	, myServers(16)
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
	myIsLetThrough = true;
	myStateStatus = eStateStatus::eKeepState;
	myStateStack = aStateStackProxy;

	myGUIManager = new GUI::GUIManager(myCursor, "Data/Resource/GUI/GUI_Lobby.xml", nullptr, -1);

	const CU::Vector2<int>& windowSize = Prism::Engine::GetInstance()->GetWindowSizeInt();
	OnResize(windowSize.x, windowSize.y);

	std::ifstream stream;
	stream.open("Data/Setting/ip.txt");

	int i = 0;
	Server server;
	while (stream >> server.myIp && i < 9)
	{
		++i;
		stream >> server.myName;

		myServers.Add(server);
	}

	for (int i = 0; i < myServers.Size(); ++i)
	{
		std::string text(myServers[i].myName + ": " + myServers[i].myIp);
		myGUIManager->SetButtonText(i, text);
	}
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
		return eStateStatus::ePopSubState;
	}

	if (myServer != nullptr)
	{
		ClientNetworkManager::GetInstance()->ConnectToServer(myServer->myIp.c_str());

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
		case eOnClickEvent::CONNECT:
			myServer = &myServers[aMessage.myID];
			break;
		default:
			DL_ASSERT("Unknown event.");
			break;
		}
	}
}