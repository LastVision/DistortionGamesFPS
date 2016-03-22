#pragma once
#include <SharedProjectileManager.h>

namespace Prism
{
	class Scene;
}


class ServerProjectileManager : public SharedProjectileManager
{
public:
	static void Create();
	static void Destroy();
	static ServerProjectileManager* GetInstance();

	void CreateBullets(Prism::Scene* aScene = nullptr) override;
private:
	ServerProjectileManager();
	~ServerProjectileManager();

};

