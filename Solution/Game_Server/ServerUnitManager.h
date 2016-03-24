#pragma once
#include <SharedUnitManager.h>
class ServerUnitManager : public SharedUnitManager
{
public:


	void CreateUnits(Prism::Scene* aScene) override;

	static ServerUnitManager* GetInstance();
	static void Create();
	static void Destroy();


private:
	ServerUnitManager();
	~ServerUnitManager();
};

