#include "stdafx.h"
#include "SpawnpointComponent.h"
#include "Entity.h"
#include "EntityFactory.h"
#include <MathHelper.h>
SpawnpointComponent::SpawnpointComponent(Entity& anEntity, const SpawnpointComponentData& aSpawnpointComponentData)
	: Component(anEntity)
	, myData(aSpawnpointComponentData)
{

	int randomType = CU::Math::RandomRange(0, aSpawnpointComponentData.myUnitTypes.Size());



	for (int i = 0; i < myUnits.GetCapacity(); ++i)
	{
		Entity* unit = EntityFactory::CreateEntity((50000 + i), eEntityType::BULLET, aSpawnpointComponentData.myUnitTypes[randomType]
			, nullptr, false, CU::Vector3<float>(0, -10.f, 0));
		unit->Kill();
		myUnits.Add(unit);
	}


}


SpawnpointComponent::~SpawnpointComponent()
{
}

void SpawnpointComponent::Update(float aDelta)
{


}
