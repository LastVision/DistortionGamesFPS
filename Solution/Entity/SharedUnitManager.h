#pragma once


class Entity;

namespace Prism
{
	class Scene;
}


class SharedUnitManager
{
public:
	virtual ~SharedUnitManager();

	static SharedUnitManager* GetInstance();
	virtual void CreateUnits(Prism::Scene* aScene) = 0;
	void Update(float aDeltaTime);
	void ActivateUnit(Entity* aUnit);
	Entity* RequestUnit(const std::string& aUnitType);
	Entity* RequestUnit(unsigned int aGID);


protected:

	Entity* SearchForUnit(const std::string& aUnitType);

	static SharedUnitManager* myInstance;
	SharedUnitManager();

	CU::GrowingArray<Entity*> myUnits;
	CU::GrowingArray<Entity*> myActiveUnits;
	unsigned int myUnitIndex;
};

