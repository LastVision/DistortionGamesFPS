#pragma once
#include <SharedProjectileManager.h>
namespace Prism
{
	class Scene;
}

class ClientProjectileManager : public SharedProjectileManager
{
public:

	static void Create();
	static void Destroy();
	static ClientProjectileManager* GetInstance();
	void CreateBullets(Prism::Scene* aScene) override;

private:
	ClientProjectileManager();
	~ClientProjectileManager();

};

