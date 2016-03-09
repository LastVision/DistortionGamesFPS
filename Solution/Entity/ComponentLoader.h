#pragma once

struct AnimationComponentData;
struct FirstPersonRenderComponentData;
struct GraphicsComponentData;
struct HealthComponentData;
struct InputComponentData;
struct NetworkComponentData;
struct ProjectileComponentData;
struct ShootingComponentData;
struct TriggerComponentData;

class XMLReader;

namespace tinyxml2
{
	class XMLElement;
}

class ComponentLoader
{
public:
	void LoadAnimationComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, AnimationComponentData& aOutputData);
	void LoadFirstPersonRenderComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, FirstPersonRenderComponentData& aOutputData);
	void LoadGraphicsComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, GraphicsComponentData& aOutputData);
	void LoadHealthComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, HealthComponentData& aOutputData);
	void LoadNetworkComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, NetworkComponentData& aOutputData);
	void LoadProjectileComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, ProjectileComponentData& aOutputData);
	void LoadTriggerComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, TriggerComponentData& aOutputData);
	void LoadInputComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, InputComponentData& aOutputData);
	void LoadShootingComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, ShootingComponentData& aOutputData);
};