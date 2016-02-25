#pragma once
#include "../Game_Shared/SharedNetworkManager.h"
class ClientNetwork;
class ClientNetworkManager : public SharedNetworkManager
{
public:

	void Initiate() override;
	static void Create();
	static void Destroy();
	static ClientNetworkManager* GetInstance();

	void StartNetwork() override;
	void ConnectToServer(const char* aServerIP = "127.0.0.1");

private:
	ClientNetworkManager();
	~ClientNetworkManager();
	static ClientNetworkManager* myInstance;

	ClientNetwork* myNetwork;

	void ReceieveThread() override;
	void SendThread() override;
};

