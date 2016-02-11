#pragma once

class Entity;
class PollingStation
{
public:
	static PollingStation* GetInstance();
	static void Destroy();

	void RegisterEntity(Entity* aEntity);

	Entity* FindClosestEntity(const CU::Vector3<float>& aPosition, int aEntityOwner, float aMaxDistance2 = FLT_MAX);
	CU::GrowingArray<Entity*> FindClosestEntities(const CU::Vector3<float>& aPosition, int aEntityOwner, float aMaxDistance2 = FLT_MAX);
	Entity* FindEntityAtPosition(const CU::Vector3<float>& aPosition, int aEntityOwner);
	void FindAllEntitiesCloseToEntity(const Entity* anEntity, float aRadius, CU::GrowingArray<Entity*>& someEntitiesOut);
	void FindAllUnitsCloseToEntity(const Entity* anEntity, float aRadius, CU::GrowingArray<Entity*>& someEntitiesOut);
	void FindAllPropsCloseToEntity(const Entity* anEntity, float aRadius, CU::GrowingArray<Entity*>& someEntitiesOut);
	
	const CU::GrowingArray<Entity*>& GetUnits(eOwnerType anOwner) const;
	const CU::GrowingArray<Entity*>& GetResourcePoints() const;
	const CU::GrowingArray<Entity*>& GetVictoryPoints() const;
	const CU::GrowingArray<Entity*>& GetVictoryAndResourcePoints() const;

	//Returns the number of ResourcePoints held by anOwner
	int GetResourcePointsCount(eOwnerType anOwner) const;

	//Returns the number of VictoryPoints held by anOwner
	int GetVictoryPointsCount(eOwnerType anOwner) const;

	CU::Vector2<float> GetClosestNotOwnedResourcePoint(eOwnerType aOwner, const CU::Vector2<float>& aPoint) const;
	CU::Vector2<float> GetClosestNotOwnedVictoryPoint(eOwnerType aOwner, const CU::Vector2<float>& aPoint) const;

	void AddArtifact(Entity* aArtifact);
	void RemoveArtifact(Entity* aArtifact);
	const CU::GrowingArray<Entity*>& GetArtifacts() const;

	const Entity* GetBase(eOwnerType anOwner) const;

	void CleanUp();

	void AddTotem(Entity* aTotem);
	Entity* GetTotem();


private:
	PollingStation();
	~PollingStation();

	void FindClosestEntity(const CU::Vector3<float>& aPosition, const CU::GrowingArray<Entity*>& someEntities
		,float aMaxDistance, float& aBestDistance, Entity** aOutEntity);
	void FindAllEntitiesCloseToEntity(const CU::GrowingArray<Entity*>& someEntitiesIn, const Entity* anEntity, float aRadius
		, CU::GrowingArray<Entity*>& someEntitiesOut);

	CU::GrowingArray<Entity*> myPlayerUnits;
	CU::GrowingArray<Entity*> myAIUnits;
	CU::GrowingArray<Entity*> myNeutralUnits;
	CU::GrowingArray<Entity*> myResourcePoints;
	CU::GrowingArray<Entity*> myVictoryPoints;
	CU::GrowingArray<Entity*> myVictoryAndResourcePoints;
	CU::GrowingArray<Entity*> myArtifacts;
	CU::GrowingArray<Entity*> myProps;

	Entity* myPlayerBuilding;
	Entity* myAIBuilding;
	Entity* myPlayerTotem;

	static PollingStation* myInstance;
};


