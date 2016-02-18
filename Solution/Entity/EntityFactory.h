#pragma once
#include <EngineEnums.h>

class ComponentLoader;
class Entity;

namespace Prism
{
	class Scene;
	class Terrain;
}

class EntityFactory
{
public:
	static EntityFactory* GetInstance();
	static void Destroy();

	void LoadEntities(const char* aEntityListXML);
	static Entity* CreateEntity(eOwnerType aOwner, eEntityType aType, Prism::eOctreeType aOctreeType, 
		Prism::Scene& aScene, CU::Vector3f aPostion, const CU::Vector3f& aRotation = CU::Vector3f(), 
		const CU::Vector3f& aScale = { 1.f, 1.f, 1.f });

	static Entity* CreateEntity(eOwnerType aOwner, eEntityType aType, std::string aSubType, Prism::eOctreeType aOctreeType,
		Prism::Scene& aScene, CU::Vector3f aPostion, const CU::Vector3f& aRotation = CU::Vector3f(), 
		const CU::Vector3f& aScale = { 1.f, 1.f, 1.f });
private:
	void LoadEntity(const char* aEntityPath);
	EntityFactory();
	~EntityFactory();

	std::unordered_map<eEntityType, EntityData> myLoadedEntityData;
	std::unordered_map<std::string, EntityData> myLoadedSubEntityData;

	ComponentLoader* myComponentLoader;

	static EntityFactory* myInstance;
};

