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
	void ClientProjectileManager::Update(float aDeltaTime) override;
	void CreateBullets(Prism::Scene* aScene) override;
	void CreateGrenades(Prism::Scene* aScene) override;
	void RequestExplosion(const CU::Vector3<float>& aPosition, unsigned int aGID);
	void CreateExplosions();
	void KillGrenade(unsigned int aGID);
private:
	ClientProjectileManager();
	~ClientProjectileManager();

	CU::GrowingArray<Entity*> myExplosions;
	struct ExplosionStruct
	{
		ExplosionStruct(){};
		ExplosionStruct(Entity* aEntity)
			: myExplosion(aEntity)
			, myFrameCount(0)
		{}
		Entity* myExplosion = nullptr;
		int myFrameCount;
	};
	CU::GrowingArray<ExplosionStruct> myLiveExplosions;

};

