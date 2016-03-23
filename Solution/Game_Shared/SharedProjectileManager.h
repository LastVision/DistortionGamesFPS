#pragma once
class Entity;

namespace Prism
{
	class Scene;
}

class SharedProjectileManager
{
public:
	virtual ~SharedProjectileManager();
	virtual void CreateBullets(Prism::Scene* aScene) = 0;
	virtual void CreateGrenades(Prism::Scene* aScene) = 0;
	static SharedProjectileManager* GetInstance();
	int GetBulletIndex();
	Entity* RequestBullet();
	void Update(float aDeltaTime);
	Entity* RequestGrenade();

protected:
	SharedProjectileManager();
	CU::GrowingArray<Entity*> myBullets;
	CU::GrowingArray<Entity*> myLiveBullets;

	CU::GrowingArray<Entity*> myGrenades;
	CU::GrowingArray<Entity*> myLiveGrenades;
	int myGrenadeIndex;

	static SharedProjectileManager* myInstance;

	int myBulletIndex;

};

