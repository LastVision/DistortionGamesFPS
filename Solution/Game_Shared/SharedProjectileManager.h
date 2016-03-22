#pragma once
class Entity;

namespace Prism
{
	class Scene;
}

class SharedProjectileManager
{
public:
	virtual void CreateBullets(Prism::Scene* aScene) = 0;

protected:
	SharedProjectileManager();
	~SharedProjectileManager();
	CU::GrowingArray<Entity*> myBullets;


};

