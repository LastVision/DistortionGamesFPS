#include "stdafx.h"

#include "ClientNetworkManager.h"
#include <Cursor.h>
#include <fstream>
#include <GUIManager.h>
#include <InputWrapper.h>
#include "LobbyState.h"
#include <NetMessageReplyServer.h>
#include <NetMessageRequestServer.h>
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
	ClientNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::SERVER_REPLY, this);
}

void ServerSelectState::InitState(StateStackProxy* aStateStackProxy, GUI::Cursor* aCursor)
{
	PostMaster::GetInstance()->Subscribe(eMessageType::ON_CLICK, this);
	ClientNetworkManager::GetInstance()->Subscribe(eNetMessageType::SERVER_REPLY, this);
	myCursor = aCursor;
	myIsActiveState = true;
	myIsLetThrough = true;
	myStateStatus = eStateStatus::eKeepState;
	myStateStack = aStateStackProxy;

	myGUIManager = new GUI::GUIManager(myCursor, "Data/Resource/GUI/GUI_server_select.xml", nullptr, -1);

	const CU::Vector2<int>& windowSize = Prism::Engine::GetInstance()->GetWindowSizeInt();
	OnResize(windowSize.x, windowSize.y);
	/*
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
	}*/
	myCursor->SetShouldRender(true);
	myTriedToConnect = false;

	myRefreshServerTimer = 0.f;
	myWaitForResponseTimer = 0.f;
	// broadcast request server
	ClientNetworkManager::GetInstance()->AddMessage(NetMessageRequestServer(), ClientNetworkManager::GetInstance()->GetBroadcastAddress());
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

	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_SPACE) == true)
	{
		myServer = &myServers[0];
	}

	if (myServer != nullptr)
	{
		if (myTriedToConnect == false)
		{
			ClientNetworkManager::GetInstance()->ConnectToServer(myServer->myIp.c_str());
			myTriedToConnect = true;
			myWaitForResponseTimer = 1.f;
		}
		else if (myTriedToConnect == true)
		{
			myWaitForResponseTimer -= aDeltaTime;
			if (myWaitForResponseTimer <= 0.f)
			{
				//Show Failed to connect message
				Prism::Engine::GetInstance()->PrintText("Failed to connect to the server, the server is either down or ingame. Try again!", 
				{50.f,Prism::Engine::GetInstance()->GetWindowSize().y - 50.f}, Prism::eTextType::RELEASE_TEXT);
			}
		}
		if (ClientNetworkManager::GetInstance()->GetGID() != 0)
		{
			SET_RUNTIME(false);
			PostMaster::GetInstance()->UnSubscribe(eMessageType::ON_CLICK, this);
			myStateStack->PushSubGameState(new LobbyState());
		}
		//return eStateStatus::ePopSubState;
	}

	myRefreshServerTimer -= aDeltaTime;
	if (myRefreshServerTimer <= 0)
	{
		myRefreshServerTimer = 2.f;
		for (int i = 0; i < myServers.Size(); ++i)
		{
			myGUIManager->SetButtonText(i, "");
		}
		myServers.RemoveAll();
		myTriedToConnect = false;
		myServer = nullptr;
		ClientNetworkManager::GetInstance()->AddMessage(NetMessageRequestServer(), ClientNetworkManager::GetInstance()->GetBroadcastAddress());
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
	myServer = nullptr;
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

void ServerSelectState::ReceiveNetworkMessage(const NetMessageReplyServer& aMessage, const sockaddr_in&)
{
	ServerSelectState::Server newServer;
	newServer.myIp = aMessage.myIP;
	newServer.myName = aMessage.myServerName;

	for (ServerSelectState::Server server : myServers)
	{
		if (server == newServer)
		{
			return;
		}
	}

	myServers.Add(newServer);
	for (int i = 0; i < myServers.Size(); ++i)
	{
		std::string text(myServers[i].myName + ": " + myServers[i].myIp);
		myGUIManager->SetButtonText(i, text);
	}
}