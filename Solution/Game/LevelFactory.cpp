#include "stdafx.h"

#include <EntityFactory.h>
#include "Level.h"
#include "LevelFactory.h"
#include <XMLReader.h>


LevelFactory::LevelFactory()
{
	EntityFactory::GetInstance()->LoadEntities("Data/Resource/Entity/LI_Entity.xml");
}


LevelFactory::~LevelFactory()
{
}

Level* LevelFactory::LoadCurrentLevel()
{
	myCurrentLevel = new Level();
	ReadLevel("Data/Level/level_01/level_01.xml");
	return myCurrentLevel;
}

void LevelFactory::ReadLevel(const std::string& aLevelPath)
{
	XMLReader reader;
	reader.OpenDocument(aLevelPath);
	tinyxml2::XMLElement* levelElement = reader.ForceFindFirstChild("root");
	levelElement = reader.ForceFindFirstChild(levelElement, "scene");

	LoadProps(reader, levelElement);

	reader.CloseDocument();
}

void LevelFactory::LoadProps(XMLReader& aReader, tinyxml2::XMLElement* aElement)
{
	for (tinyxml2::XMLElement* entityElement = aReader.FindFirstChild(aElement, "prop"); entityElement != nullptr;
		entityElement = aReader.FindNextElement(entityElement, "prop"))
	{
		std::string propType;
		aReader.ForceReadAttribute(entityElement, "propType", propType);
		propType = CU::ToLower(propType);

		tinyxml2::XMLElement* propElement = aReader.ForceFindFirstChild(entityElement, "position");
		CU::Vector3<float> propPosition;
		aReader.ForceReadAttribute(propElement, "X", "Y", "Z", propPosition);

		propElement = aReader.ForceFindFirstChild(entityElement, "rotation");
		CU::Vector3<float> propRotation;
		aReader.ForceReadAttribute(propElement, "X", "Y", "Z", propRotation);

		propElement = aReader.ForceFindFirstChild(entityElement, "scale");
		CU::Vector3<float> propScale;
		aReader.ForceReadAttribute(propElement, "X", "Y", "Z", propScale);

		propRotation.x = CU::Math::DegreeToRad(propRotation.x);
		propRotation.y = CU::Math::DegreeToRad(propRotation.y);
		propRotation.z = CU::Math::DegreeToRad(propRotation.z);
		
		Entity* newEntity = EntityFactory::CreateEntity(eOwnerType::NEUTRAL, eEntityType::PROP, propType, Prism::eOctreeType::STATIC, 
			*myCurrentLevel->GetScene(), propPosition, propRotation, propScale);
		newEntity->AddToScene();
		newEntity->Reset();

		myCurrentLevel->AddEntity(newEntity);
	}
}