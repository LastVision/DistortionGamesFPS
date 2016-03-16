#include "stdafx.h"

#include <Cursor.h>
#include "ClientNetworkManager.h"
#include "LobbyState.h"
#include <fstream>
#include <GUIManager.h>
#include <InputWrapper.h>
#include <OnClickMessage.h>
#include <PostMaster.h>
#include "ServerSelectState.h"


ServerSelectState::ServerSelectState()
	: myGUIManager(nullptr)
	, myServer(nullptr)
	, myServers(16)
{
}


ServerSelectState::~ServerSelectState()
{
	SAFE_DELETE(myGUIManager);
	myCursor = nullptr;
	PostMaster::GetInstance()->UnSubscribe(eMessageType::ON_CLICK, this);
}

void ServerSelectState::InitState(StateStackProxy* aStateStackProxy, GUI::Cursor* aCursor)
{
	PostMaster::GetInstance()->Subscribe(eMessageType::ON_CLICK, this);
	myCursor = aCursor;
	myIsActiveState = true;
	myIsLetThrough = true;
	myStateStatus = eStateStatus::eKeepState;
	myStateStack = aStateStackProxy;

	myGUIManager = new GUI::GUIManager(myCursor, "Data/Resource/GUI/GUI_server_select.xml", nullptr, -1);

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

void ServerSelectState::EndState()
{
	myIsActiveState = false;
	myCursor->SetShouldRender(false);
}

void ServerSelectState::OnResize(int aX, int aY)
{
	myGUIManager->OnResize(aX, aY);
}

const eStateStatus ServerSelectState::Update(const float& aDeltaTime)
{
	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_ESCAPE) == true
		|| CU::InputWrapper::GetInstance()->KeyDown(DIK_N) == true)
	{
		return eStateStatus::ePopSubState;
	}

	if (myServer != nullptr)
	{
		ClientNetworkManager::GetInstance()->ConnectToServer(myServer->myIp.c_str());

		PostMaster::GetInstance()->UnSubscribe(eMessageType::ON_CLICK, this);
		myStateStack->PushSubGameState(new LobbyState());

		//return eStateStatus::ePopSubState;
	}

	myGUIManager->Update(aDeltaTime);

	return myStateStatus;
}

void ServerSelectState::Render()
{
	myGUIManager->Render();
}

void ServerSelectState::ResumeState()
{
	myIsActiveState = true;
	PostMaster::GetInstance()->Subscribe(eMessageType::ON_CLICK, this);
}

void ServerSelectState::ReceiveMessage(const OnClickMessage& aMessage)
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