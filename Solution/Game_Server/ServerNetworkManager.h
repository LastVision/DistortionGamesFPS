#pragma once
#include "../Game_Shared/SharedNetworkManager.h"
class ServerNetwork;
class ServerNetworkManager : public SharedNetworkManager
{
public:
	void Initiate() override;
	static void Create();
	static void Destroy();
	static ServerNetworkManager* GetInstance();

	void StartNetwork() override;

private:
	ServerNetworkManager();
	~ServerNetworkManager();
	static ServerNetworkManager* myInstance;
	ServerNetwork* myNetwork;

	void ReceieveThread() override;
	void SendThread() override;

};

