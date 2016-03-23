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
	static SharedProjectileManager* GetInstance();
	int GetBulletIndex();
	Entity* RequestBullet();
	void Update(float aDeltaTime);

protected:
	SharedProjectileManager();
	CU::GrowingArray<Entity*> myBullets;
	CU::GrowingArray<Entity*> myLiveBullets;

	static SharedProjectileManager* myInstance;

	int myBulletIndex;

};

