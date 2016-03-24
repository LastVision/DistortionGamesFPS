#pragma once
#include <SharedUnitManager.h>
#include <NetworkSubscriber.h>

class ClientUnitManager : public SharedUnitManager, public NetworkSubscriber
{
public:


	void CreateUnits(Prism::Scene* aScene) override;
	static ClientUnitManager* GetInstance();
	void ReceiveNetworkMessage(const NetMessageActivateUnit& aMessage, const sockaddr_in& aSenderAddress) override;
	void ReceiveNetworkMessage(const NetMessageEntityState& aMessage, const sockaddr_in& aSenderAddress) override;
	static void Create();
	static void Destroy();
private:
	ClientUnitManager();
	~ClientUnitManager();

};

