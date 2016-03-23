#pragma once
#include <SharedUnitManager.h>

class ClientUnitManager : public SharedUnitManager
{
public:


	void CreateUnits(Prism::Scene* aScene) override;
	static ClientUnitManager* GetInstance();
	static void Create();
	static void Destroy();
private:
	ClientUnitManager();
	~ClientUnitManager();

};

