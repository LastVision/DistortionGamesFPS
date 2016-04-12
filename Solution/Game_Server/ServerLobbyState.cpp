#include "stdafx.h"

#include <CommonHelper.h>
#include <PostMaster.h>
#include "ServerInGameState.h"
#include "ServerLevelFactory.h"
#include "ServerLobbyState.h"
#include "ServerNetworkManager.h"
#include "ServerStateStackProxy.h"
#include <NetMessageDisconnect.h>
#include <NetMessageLoadLevel.h>
#include <NetMessageRequestConnect.h>
#include <NetMessageSetLevel.h>
#include <NetMessageReplyServer.h>
#include <NetMessageRequestStartLevel.h>
#include <MurmurHash3.h>
#include <XMLReader.h>

ServerLobbyState::ServerLobbyState(eGameType aGameType)
	: myGameType(aGameType)
	, myLevelFactory(nullptr)
{
	myIsActiveState = false;

	XMLReader reader;
	reader.OpenDocument("Data/Setting/SET_difficulty.xml");
	tinyxml2::XMLElement* root = reader.ForceFindFirstChild("root");

	reader.ForceReadAttribute(reader.ForceFindFirstChild(root, "Damage"), "easy", GC::DamageMultiplier[0]);
	reader.ForceReadAttribute(reader.ForceFindFirstChild(root, "Damage"), "normal", GC::DamageMultiplier[1]);
	reader.ForceReadAttribute(reader.ForceFindFirstChild(root, "Damage"), "hard", GC::DamageMultiplier[2]);

	reader.ForceReadAttribute(reader.ForceFindFirstChild(root, "Spawninterval"), "easy", GC::SpawnIntervalMultiplier[0]);
	reader.ForceReadAttribute(reader.ForceFindFirstChild(root, "Spawninterval"), "normal", GC::SpawnIntervalMultiplier[1]);
	reader.ForceReadAttribute(reader.ForceFindFirstChild(root, "Spawninterval"), "hard", GC::SpawnIntervalMultiplier[2]);

	reader.CloseDocument();
}

ServerLobbyState::~ServerLobbyState()
{
	ServerNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::ON_DISCONNECT, this);
	ServerNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::SET_LEVEL, this);
	ServerNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::REQUEST_LEVEL, this);
	ServerNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::REQUEST_START_LEVEL, this);
	ServerNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::SERVER_REQUEST, this);
	ServerNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::ON_CONNECT, this);
	SAFE_DELETE(myLevelFactory);
}

void ServerLobbyState::InitState(ServerStateStackProxy* aStateStackProxy)
{
	myStateStack = aStateStackProxy;
	myStateStatus = eStateStatus::KEEP_STATE;

	ServerNetworkManager::GetInstance()->Subscribe(eNetMessageType::ON_DISCONNECT, this);
	ServerNetworkManager::GetInstance()->Subscribe(eNetMessageType::SET_LEVEL, this);
	ServerNetworkManager::GetInstance()->Subscribe(eNetMessageType::REQUEST_LEVEL, this);
	ServerNetworkManager::GetInstance()->Subscribe(eNetMessageType::REQUEST_START_LEVEL, this);
	ServerNetworkManager::GetInstance()->Subscribe(eNetMessageType::SERVER_REQUEST, this);
	ServerNetworkManager::GetInstance()->Subscribe(eNetMessageType::ON_CONNECT, this);
	myCurrentLevelID = 0;
	myIsActiveState = true;
	
	ServerNetworkManager::GetInstance()->AllowNewConnections(true);
	Utility::Printf("State: Lobby", eConsoleColor::AQUA_TEXT, true);
	myAboutToChangeState = false;
}

void ServerLobbyState::EndState()
{
}

const eStateStatus ServerLobbyState::Update(const float aDeltaTime)
{
	aDeltaTime;
	if (ServerNetworkManager::GetInstance()->GetClients().Size() <= 0)
	{
		myStateStatus = eStateStatus::POP_MAIN_STATE;
	}
	return myStateStatus;
}

void ServerLobbyState::ResumeState()
{
	Utility::Printf("State: Lobby", eConsoleColor::AQUA_TEXT, true);
	myIsActiveState = true;
	ServerNetworkManager::GetInstance()->Subscribe(eNetMessageType::ON_DISCONNECT, this);
	ServerNetworkManager::GetInstance()->Subscribe(eNetMessageType::SET_LEVEL, this);
	ServerNetworkManager::GetInstance()->Subscribe(eNetMessageType::REQUEST_LEVEL, this);
	ServerNetworkManager::GetInstance()->Subscribe(eNetMessageType::REQUEST_START_LEVEL, this);
	ServerNetworkManager::GetInstance()->Subscribe(eNetMessageType::SERVER_REQUEST, this);
	ServerNetworkManager::GetInstance()->Subscribe(eNetMessageType::ON_CONNECT, this);
	ServerNetworkManager::GetInstance()->AllowNewConnections(true);
}

void ServerLobbyState::ReceiveNetworkMessage(const NetMessageSetLevel& aMessage, const sockaddr_in&)
{
	myCurrentLevelID = aMessage.myLevelID % 10;
	GC::Difficulty = (aMessage.myLevelID / 10) - 1;
	ServerNetworkManager::GetInstance()->AddMessage(NetMessageSetLevel(myCurrentLevelID));
}

void ServerLobbyState::ReceiveNetworkMessage(const NetMessageRequestLevel&, const sockaddr_in& aSenderAddress)
{
	ServerNetworkManager::GetInstance()->AddMessage(NetMessageSetLevel(myCurrentLevelID), aSenderAddress);
}

void ServerLobbyState::ReceiveNetworkMessage(const NetMessageRequestStartLevel&, const sockaddr_in&)
{
	ServerNetworkManager::GetInstance()->AllowNewConnections(false);

	SET_RUNTIME(false);
	myLevelFactory = new ServerLevelFactory("Data/Level/LI_level.xml");
	unsigned int levelHashValue = Hash(CU::ReadFileToString(myLevelFactory->GetLevelPath(myCurrentLevelID)).c_str());
	ServerNetworkManager::GetInstance()->AddMessage(NetMessageLoadLevel(myCurrentLevelID, levelHashValue));

	ServerNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::ON_DISCONNECT, this);
	ServerNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::SET_LEVEL, this);
	ServerNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::REQUEST_LEVEL, this);
	ServerNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::REQUEST_START_LEVEL, this);
	ServerNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::SERVER_REQUEST, this);
	ServerNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::ON_CONNECT, this);

	myStateStack->PushSubState(new ServerInGameState(myCurrentLevelID, levelHashValue));
}

void ServerLobbyState::ReceiveNetworkMessage(const NetMessageRequestConnect& aMessage, const sockaddr_in& aSenderAddress)
{
	aMessage; 
	aSenderAddress;
	//if (myAboutToChangeState == false)
	//{
	//	//Broadcast join
		ServerNetworkManager::GetInstance()->CreateConnection(aMessage.myName, aSenderAddress);
	//}
}

void ServerLobbyState::ReceiveNetworkMessage(const NetMessageRequestServer&, const sockaddr_in& aSenderAddress)
{
	if (myGameType == eGameType::MULTIPLAYER && ServerNetworkManager::GetInstance()->IsAllowedNewConnection() == true)
	{
		char username[256 + 1];
		DWORD username_len = 256 + 1;
		GetUserNameA(username, &username_len);
		ServerNetworkManager::GetInstance()->AddMessage(NetMessageReplyServer(username, ServerNetworkManager::GetInstance()->GetIP()), aSenderAddress);
	}
}

void ServerLobbyState::ReceiveNetworkMessage(const NetMessageDisconnect& aMessage, const sockaddr_in&)
{
	if (aMessage.mySenderID == 1 || ServerNetworkManager::GetInstance()->GetClients().Size() <= 0)
	{
		ServerNetworkManager::GetInstance()->DisconnectAll();
		myStateStatus = eStateStatus::POP_MAIN_STATE;
	}
}