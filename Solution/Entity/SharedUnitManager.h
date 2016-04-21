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
	void ActivateUnit(Entity* aUnit, const CU::Vector3<float>& aPosition);
	Entity* RequestUnit(const std::string& aUnitType);
	Entity* RequestUnit(unsigned int aGID);

	const CU::GrowingArray<Entity*>& GetUnits();

	Entity* GetUnitToHit(const CU::Matrix44<float>& aPlayerOrientation) const;


protected:
	void AddToMap();
	Entity* SearchForUnit(const std::string& aUnitType);

	static SharedUnitManager* myInstance;
	SharedUnitManager();

	CU::GrowingArray<Entity*> myUnits;
	std::unordered_map<unsigned int, Entity*> myUnitsMap;


	CU::GrowingArray<Entity*> myActiveUnits;

	unsigned int myUnitIndex;
};

