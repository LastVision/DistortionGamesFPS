#include "stdafx.h"
#include <EntityFactory.h>
#include "ClientLevel.h"
#include "ClientLevelFactory.h"
#include "ClientNetworkManager.h"
#include "ClientUnitManager.h"
#include <PhysicsComponent.h>
#include <PhysicsInterface.h>
#include <GraphicsComponent.h>
#include <PointLight.h>
#include <SpotLight.h>
#include <SpotLightTextureProjection.h>
#include <Room.h>
#include <RoomManager.h>
#include <Scene.h>
#include <TriggerComponent.h>
#include <XMLReader.h>
#include <PostMaster.h>
#include <EmitterMessage.h>

ClientLevelFactory::ClientLevelFactory(const std::string& aLevelListPath)
	: SharedLevelFactory(aLevelListPath)
	, myWeaponSettingsPath("")
{

}

ClientLevelFactory::~ClientLevelFactory()
{
}

ClientLevel* ClientLevelFactory::LoadLevel(int aID, GUI::Cursor* aCursor, eStateStatus& aStateStatus)
{
	myIsLoadingLevel = true;
	if (myLevelPaths.find(aID) == myLevelPaths.end())
	{
		DL_ASSERT(CU::Concatenate("[LevelFactory] Trying to load a non-existing level! Check so the ID: %s is a valid id in LI_level.xml"
			, std::to_string(aID).c_str()));
	}

	myCurrentID = aID;

	return LoadCurrentLevel(aCursor, aStateStatus);
}

ClientLevel* ClientLevelFactory::LoadCurrentLevel(GUI::Cursor* aCursor, eStateStatus& aStateStatus)
{
	myIsLoadingLevel = true;
	myCurrentLevel = new ClientLevel(aCursor, aStateStatus, myCurrentID);
	ReadLevel(myLevelPaths[myCurrentID]);
	myCurrentLevel->Init(myWeaponSettingsPath);
	myCurrentLevel->SetMinMax(myMinPoint, myMaxPoint);



	std::string levelName(myLevelPaths[myCurrentID].begin() + myLevelPaths[myCurrentID].rfind('/') + 1, myLevelPaths[myCurrentID].end() - 4);
	myCurrentLevel->SetName(levelName);
#ifdef THREAD_PHYSICS
	Prism::PhysicsInterface::GetInstance()->InitThread();
#endif
	myIsLoadingLevel = false;
	return myCurrentLevel;
}

void ClientLevelFactory::ReadLevel(const std::string& aLevelPath)
{
	XMLReader reader;
	reader.OpenDocument(aLevelPath);
	tinyxml2::XMLElement* levelElement = reader.ForceFindFirstChild("root");
	levelElement = reader.ForceFindFirstChild(levelElement, "scene");

	reader.ForceReadAttribute(reader.ForceFindFirstChild(levelElement, "weaponXML"), "path", myWeaponSettingsPath);

	LoadRooms(reader, levelElement);
	LoadProps(reader, levelElement);
	LoadDoors(reader, levelElement);
	LoadParticles(reader, levelElement);
	//LoadUnits(reader, levelElement);
	LoadText(reader, levelElement);
	ClientUnitManager::GetInstance()->CreateUnits(myCurrentLevel->GetScene());

	LoadTriggers(reader, levelElement);
	LoadLights(reader, levelElement);
	LoadPlayerStartPosition(reader, levelElement);




	reader.CloseDocument();
}

void ClientLevelFactory::LoadRooms(XMLReader& aReader, tinyxml2::XMLElement* aElement)
{
	for (tinyxml2::XMLElement* entityElement = aReader.ForceFindFirstChild(aElement, "room"); entityElement != nullptr;
		entityElement = aReader.FindNextElement(entityElement, "room"))
	{
		CU::Vector3f position;
		CU::Vector3f rotation;
		CU::Vector3f scale;
		ReadOrientation(aReader, entityElement, position, rotation, scale);
		DL_ASSERT_EXP(rotation.x == 0 && rotation.y == 0 && rotation.z == 0, "Room rotation non-zero.");

		std::string name;
		aReader.ForceReadAttribute(entityElement, "name", name);
		std::string type;
		aReader.ForceReadAttribute(entityElement, "type", type);



		DL_ASSERT_EXP(type == "room" || type == "connector", CU::Concatenate("Room type can't be: %s", type.c_str()));

		Prism::eRoomType typeEnum(Prism::eRoomType::ROOM);

		if (type == "connector")
		{
			typeEnum = Prism::eRoomType::CONNECTOR;
		}

		Prism::Room *newRoom = new Prism::Room(position, scale, name, typeEnum);

		if (typeEnum == Prism::eRoomType::ROOM)
		{
#ifdef USE_ATMOSPHERE_PARTICLES
			PostMaster::GetInstance()->SendMessage(EmitterMessage("Atmosphere", position, scale, newRoom));
#endif
		}

		static_cast<ClientLevel*>(myCurrentLevel)->GetScene()->AddRoom(newRoom);
	}

	static_cast<ClientLevel*>(myCurrentLevel)->GetScene()->GetRoomManager()->CalcPortals();
}

void ClientLevelFactory::LoadProps(XMLReader& aReader, tinyxml2::XMLElement* aElement)
{
	for (tinyxml2::XMLElement* entityElement = aReader.FindFirstChild(aElement, "prop"); entityElement != nullptr;
		entityElement = aReader.FindNextElement(entityElement, "prop"))
	{
		std::string propType;
		aReader.ForceReadAttribute(entityElement, "propType", propType);
		propType = CU::ToLower(propType);

		CU::Vector3f propPosition;
		CU::Vector3f propRotation;
		CU::Vector3f propScale;

		unsigned int gid(UINT32_MAX);

		ReadGID(aReader, entityElement, gid);
		ReadOrientation(aReader, entityElement, propPosition, propRotation, propScale);

		propRotation.x = CU::Math::DegreeToRad(propRotation.x);
		propRotation.y = CU::Math::DegreeToRad(propRotation.y);
		propRotation.z = CU::Math::DegreeToRad(propRotation.z);

		Entity* newEntity = EntityFactory::CreateEntity(gid, eEntityType::PROP, propType, static_cast<ClientLevel*>(myCurrentLevel)->GetScene()
			, true, propPosition, propRotation, propScale);
		newEntity->AddToScene();
		newEntity->Reset();

		myCurrentLevel->AddEntity(newEntity);

		if (newEntity->GetSubType() == "sm_square_ceiling_lamp_a_short")
		{
			propPosition.y -= 0.37f;
			Prism::SpotLightTextureProjection* light = new Prism::SpotLightTextureProjection(gid * 10000, false);
			light->SetPosition(CU::Vector4<float>(propPosition, 1.f));
			//light->SetColor(color);
			light->SetRange(1.f);
			light->SetAngle(CU::Math::DegreeToRad(175.f / 2.f));
			light->Init();

			//light->PerformTransformation(CU::Matrix44f::CreateRotateAroundZ(rotation.z));
			light->PerformTransformation(CU::Matrix44f::CreateRotateAroundX(-3.14f * 0.5f));
			//light->PerformTransformation(CU::Matrix44f::CreateRotateAroundY(rotation.y));

			light->Update();
			static_cast<ClientLevel*>(myCurrentLevel)->AddLight(light);
		}

		if (newEntity->GetSubType() == "sm_wall_lamp_a")
		{
			CU::Vector3<float> offsetBack(newEntity->GetOrientation().GetRight() * -1.1f);
			CU::Vector3<float> offsetUp(newEntity->GetOrientation().GetUp() * 1.5f);

			Prism::SpotLightTextureProjection* light = new Prism::SpotLightTextureProjection(gid * 10000, true);
			//light->SetPosition(CU::Vector4<float>(21.f, 1.5f, -6.1f, 1.f));
			propPosition += offsetBack;
			propPosition += offsetUp;
			light->SetPosition(CU::Vector4<float>(propPosition, 1.f));
			//light->SetColor(color);
			light->SetRange(1.f);
			light->SetAngle(CU::Math::DegreeToRad(175.f / 2.f));
			light->Init();


			//light->PerformTransformation(CU::Matrix44f::CreateRotateAroundZ(propRotation.z));
			//light->PerformTransformation(CU::Matrix44f::CreateRotateAroundX(propRotation.x));
			light->PerformTransformation(CU::Matrix44f::CreateRotateAroundY(propRotation.y + (3.14f * 0.5f)));

			light->Update();

			static_cast<ClientLevel*>(myCurrentLevel)->AddLight(light);
		}
	}
}

void ClientLevelFactory::LoadDoors(XMLReader& aReader, tinyxml2::XMLElement* aElement)
{
	for (tinyxml2::XMLElement* entityElement = aReader.FindFirstChild(aElement, "door"); entityElement != nullptr;
		entityElement = aReader.FindNextElement(entityElement, "door"))
	{
		std::string propType;
		aReader.ForceReadAttribute(entityElement, "doorType", propType);
		propType = CU::ToLower(propType);

		CU::Vector3f propPosition;
		CU::Vector3f propRotation;
		CU::Vector3f propScale;

		unsigned int gid(UINT32_MAX);

		ReadGID(aReader, entityElement, gid);
		ReadOrientation(aReader, entityElement, propPosition, propRotation, propScale);

		propRotation.x = CU::Math::DegreeToRad(propRotation.x);
		propRotation.y = CU::Math::DegreeToRad(propRotation.y);
		propRotation.z = CU::Math::DegreeToRad(propRotation.z);

		Entity* newEntity = EntityFactory::CreateEntity(gid, eEntityType::PROP, propType, static_cast<ClientLevel*>(myCurrentLevel)->GetScene()
			, true, propPosition, propRotation, propScale);
		newEntity->AddToScene();
		newEntity->Reset();

		myCurrentLevel->AddEntity(newEntity);
	}
}

void ClientLevelFactory::LoadUnits(XMLReader& aReader, tinyxml2::XMLElement* aElement)
{
	for (tinyxml2::XMLElement* entityElement = aReader.FindFirstChild(aElement, "unit"); entityElement != nullptr;
		entityElement = aReader.FindNextElement(entityElement, "unit"))
	{
		std::string unitType;
		aReader.ForceReadAttribute(entityElement, "unitType", unitType);
		unitType = CU::ToLower(unitType);

		CU::Vector3f unitPosition;
		CU::Vector3f unitRotation;
		CU::Vector3f unitScale;

		unsigned int gid(UINT32_MAX);

		ReadGID(aReader, entityElement, gid);

		ReadOrientation(aReader, entityElement, unitPosition, unitRotation, unitScale);

		unitRotation.x = CU::Math::DegreeToRad(unitRotation.x);
		unitRotation.y = CU::Math::DegreeToRad(unitRotation.y);
		unitRotation.z = CU::Math::DegreeToRad(unitRotation.z);

		Entity* newEntity = EntityFactory::CreateEntity(gid, eEntityType::UNIT, unitType, myCurrentLevel->GetScene(), true
			, unitPosition, unitRotation, unitScale);
		newEntity->Reset();

		myCurrentLevel->AddEnemy(newEntity);
		newEntity->AddToScene();

		//Entity* newEnemy = EntityFactory::CreateEntity(aMessage.myGID, eEntityType::UNIT, "gundroid", myScene, true, aMessage.myPosition);
	}
}

void ClientLevelFactory::LoadTriggers(XMLReader& aReader, tinyxml2::XMLElement* aElement)
{
	for (tinyxml2::XMLElement* entityElement = aReader.FindFirstChild(aElement, "trigger"); entityElement != nullptr;
		entityElement = aReader.FindNextElement(entityElement, "trigger"))
	{
		std::string triggerType;
		aReader.ForceReadAttribute(entityElement, "triggerType", triggerType);
		triggerType = CU::ToLower(triggerType);

		CU::Vector3f triggerPosition;
		CU::Vector3f triggerRotation;
		CU::Vector3f triggerScale;

		unsigned int gid(UINT32_MAX);

		ReadGID(aReader, entityElement, gid);

		ReadOrientation(aReader, entityElement, triggerPosition, triggerRotation, triggerScale);

		triggerRotation.x = CU::Math::DegreeToRad(triggerRotation.x);
		triggerRotation.y = CU::Math::DegreeToRad(triggerRotation.y);
		triggerRotation.z = CU::Math::DegreeToRad(triggerRotation.z);

		Entity* newEntity = EntityFactory::CreateEntity(gid, eEntityType::TRIGGER, triggerType, static_cast<ClientLevel*>(myCurrentLevel)->GetScene()
			, true, triggerPosition, triggerRotation, triggerScale);

		bool isCoOp = ClientNetworkManager::GetInstance()->GetClients().Size() >= 1;

		if (isCoOp == false && newEntity->GetComponent<TriggerComponent>()->GetTriggerType() == eTriggerType::HEALTH_PACK)
		{
			PostMaster::GetInstance()->SendMessage(EmitterMessage("Healthpack", newEntity->GetOrientation().GetPos(), newEntity));
		}

		if (newEntity->GetComponent<TriggerComponent>()->IsClientSide() == true)
		{
			newEntity->Reset();
			if (newEntity->GetComponent<TriggerComponent>()->GetIsActiveFromStart() == true)
			{
				newEntity->AddToScene();
			}
			else
			{
				newEntity->GetComponent<PhysicsComponent>()->RemoveFromScene();
			}
			myCurrentLevel->AddEntity(newEntity);


		}
		else
		{
			if (newEntity->GetComponent<TriggerComponent>()->GetTriggerType() == eTriggerType::HEALTH_PACK && isCoOp == true)
			{
				SAFE_DELETE(newEntity);
				continue;
			}

			if (newEntity->GetComponent<GraphicsComponent>() != nullptr)
			{
				newEntity->AddToScene();
				newEntity->GetComponent<PhysicsComponent>()->RemoveFromScene();
				newEntity->RemoveComponent(eComponentType::TRIGGER);
				newEntity->RemoveComponent(eComponentType::PHYSICS);
				myCurrentLevel->AddEntity(newEntity);
			}
			else
			{
				SAFE_DELETE(newEntity);
			}
		}
	}
}

void ClientLevelFactory::LoadLights(XMLReader& aReader, tinyxml2::XMLElement* aElement)
{
	for (tinyxml2::XMLElement* lightElement = aReader.FindFirstChild(aElement, "pointlight"); lightElement != nullptr;
		lightElement = aReader.FindNextElement(lightElement, "pointlight"))
	{
		CU::Vector3<float> position;
		CU::Vector4<float> color;
		float range;

		aReader.ForceReadAttribute(aReader.ForceFindFirstChild(lightElement, "position"), "X", position.x);
		aReader.ForceReadAttribute(aReader.ForceFindFirstChild(lightElement, "position"), "Y", position.y);
		aReader.ForceReadAttribute(aReader.ForceFindFirstChild(lightElement, "position"), "Z", position.z);

		aReader.ForceReadAttribute(aReader.ForceFindFirstChild(lightElement, "color"), "R", color.x);
		aReader.ForceReadAttribute(aReader.ForceFindFirstChild(lightElement, "color"), "G", color.y);
		aReader.ForceReadAttribute(aReader.ForceFindFirstChild(lightElement, "color"), "B", color.z);
		aReader.ForceReadAttribute(aReader.ForceFindFirstChild(lightElement, "color"), "A", color.w);

		aReader.ForceReadAttribute(aReader.ForceFindFirstChild(lightElement, "range"), "value", range);

		unsigned int gid(UINT32_MAX);

		ReadGID(aReader, lightElement, gid);

		Prism::PointLight* light = new Prism::PointLight(gid, false);
		light->SetPosition(position);
		light->SetColor(color);
		light->SetRange(range);
		light->Update();
		static_cast<ClientLevel*>(myCurrentLevel)->AddLight(light);
	}

	for (tinyxml2::XMLElement* lightElement = aReader.FindFirstChild(aElement, "pointlightambient"); lightElement != nullptr;
		lightElement = aReader.FindNextElement(lightElement, "pointlightambient"))
	{
		CU::Vector3<float> position;
		CU::Vector4<float> color;
		float range;

		aReader.ForceReadAttribute(aReader.ForceFindFirstChild(lightElement, "position"), "X", position.x);
		aReader.ForceReadAttribute(aReader.ForceFindFirstChild(lightElement, "position"), "Y", position.y);
		aReader.ForceReadAttribute(aReader.ForceFindFirstChild(lightElement, "position"), "Z", position.z);

		aReader.ForceReadAttribute(aReader.ForceFindFirstChild(lightElement, "color"), "R", color.x);
		aReader.ForceReadAttribute(aReader.ForceFindFirstChild(lightElement, "color"), "G", color.y);
		aReader.ForceReadAttribute(aReader.ForceFindFirstChild(lightElement, "color"), "B", color.z);
		aReader.ForceReadAttribute(aReader.ForceFindFirstChild(lightElement, "color"), "A", color.w);

		aReader.ForceReadAttribute(aReader.ForceFindFirstChild(lightElement, "range"), "value", range);

		unsigned int gid(UINT32_MAX);

		ReadGID(aReader, lightElement, gid);

		Prism::PointLight* light = new Prism::PointLight(gid, true);
		light->SetPosition(position);
		light->SetColor(color);
		light->SetRange(range);
		light->Update();
		static_cast<ClientLevel*>(myCurrentLevel)->AddLight(light);
	}

	bool hasAddedTextureThingy = false;

	for (tinyxml2::XMLElement* lightElement = aReader.FindFirstChild(aElement, "spotlight"); lightElement != nullptr;
		lightElement = aReader.FindNextElement(lightElement, "spotlight"))
	{
		CU::Vector3<float> position;
		CU::Vector3<float> rotation;
		CU::Vector4<float> color;
		float range;
		float spotangle;


		aReader.ForceReadAttribute(aReader.ForceFindFirstChild(lightElement, "position"), "X", "Y", "Z", position);
		aReader.ForceReadAttribute(aReader.ForceFindFirstChild(lightElement, "rotation"), "X", "Y", "Z", rotation);

		aReader.ForceReadAttribute(aReader.ForceFindFirstChild(lightElement, "color"), "R", color.x);
		aReader.ForceReadAttribute(aReader.ForceFindFirstChild(lightElement, "color"), "G", color.y);
		aReader.ForceReadAttribute(aReader.ForceFindFirstChild(lightElement, "color"), "B", color.z);
		aReader.ForceReadAttribute(aReader.ForceFindFirstChild(lightElement, "color"), "A", color.w);

		aReader.ForceReadAttribute(aReader.ForceFindFirstChild(lightElement, "range"), "value", range);
		aReader.ForceReadAttribute(aReader.ForceFindFirstChild(lightElement, "spotangle"), "value", spotangle);

		unsigned int gid(UINT32_MAX);

		ReadGID(aReader, lightElement, gid);

		Prism::SpotLight* light = new Prism::SpotLight(gid);
		light->SetPosition(CU::Vector4<float>(position, 1.f));
		light->SetColor(color);
		light->SetRange(range);
		light->SetAngle(CU::Math::DegreeToRad(spotangle / 2.f));

		rotation.x = CU::Math::DegreeToRad(rotation.x);
		rotation.y = CU::Math::DegreeToRad(rotation.y);
		rotation.z = CU::Math::DegreeToRad(rotation.z);

		light->PerformTransformation(CU::Matrix44f::CreateRotateAroundZ(rotation.z));
		light->PerformTransformation(CU::Matrix44f::CreateRotateAroundX(rotation.x));
		light->PerformTransformation(CU::Matrix44f::CreateRotateAroundY(rotation.y));

		light->Update();
		static_cast<ClientLevel*>(myCurrentLevel)->AddLight(light);
	}
}

void ClientLevelFactory::LoadParticles(XMLReader& aReader, tinyxml2::XMLElement* aElement)
{
	for (tinyxml2::XMLElement* entityElement = aReader.FindFirstChild(aElement, "particle"); entityElement != nullptr;
		entityElement = aReader.FindNextElement(entityElement, "particle"))
	{
		std::string particleType;
		aReader.ForceReadAttribute(entityElement, "type", particleType);
		particleType = CU::ToLower(particleType);

		tinyxml2::XMLElement* propElement = aReader.ForceFindFirstChild(entityElement, "position");
		CU::Vector3<float> propPosition;
		aReader.ForceReadAttribute(propElement, "X", propPosition.x);
		aReader.ForceReadAttribute(propElement, "Y", propPosition.y);
		aReader.ForceReadAttribute(propElement, "Z", propPosition.z);

		//PostMaster::GetInstance()->SendMessage(EmitterMessage(particleType, propPosition, true));
	}
}

void ClientLevelFactory::LoadText(XMLReader& aReader, tinyxml2::XMLElement* aElement)
{
	for (tinyxml2::XMLElement* textElement = aReader.FindFirstChild(aElement, "worldtext"); textElement != nullptr;
		textElement = aReader.FindNextElement(textElement, "worldtext"))
	{
		std::string text;
		CU::Vector3<float> position;
		CU::Vector3<float> rotation;
		CU::Vector4<float> color;

		aReader.ForceReadAttribute(aReader.ForceFindFirstChild(textElement, "text"), "value", text);
		aReader.ForceReadAttribute(aReader.ForceFindFirstChild(textElement, "position"), "X", "Y", "Z", position);
		aReader.ForceReadAttribute(aReader.ForceFindFirstChild(textElement, "rotation"), "X", "Y", "Z", rotation);

		aReader.ForceReadAttribute(aReader.ForceFindFirstChild(textElement, "color"), "R", color.x);
		aReader.ForceReadAttribute(aReader.ForceFindFirstChild(textElement, "color"), "G", color.y);
		aReader.ForceReadAttribute(aReader.ForceFindFirstChild(textElement, "color"), "B", color.z);
		aReader.ForceReadAttribute(aReader.ForceFindFirstChild(textElement, "color"), "A", color.w);

		rotation.x = CU::Math::DegreeToRad(rotation.x);
		rotation.y = CU::Math::DegreeToRad(rotation.y);
		rotation.z = CU::Math::DegreeToRad(rotation.z);

		myCurrentLevel->AddWorldText(text, position, rotation.y, color);
	}
}

void ClientLevelFactory::LoadPlayerStartPosition(XMLReader& aReader, tinyxml2::XMLElement* aElement)
{
	for (tinyxml2::XMLElement* entityElement = aReader.ForceFindFirstChild(aElement, "startPosition"); entityElement != nullptr;
		entityElement = aReader.FindNextElement(entityElement, "startPosition"))
	{
		CU::Vector3<float> position;
		unsigned int gid = 0;

		aReader.ForceReadAttribute(aReader.ForceFindFirstChild(entityElement, "gid"), "value", gid);
		aReader.ForceReadAttribute(aReader.ForceFindFirstChild(entityElement, "position"), "X", "Y", "Z", position);

		myCurrentLevel->AddPlayerStartPosition(gid, position);
	}
}