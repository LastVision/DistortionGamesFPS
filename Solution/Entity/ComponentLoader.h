#pragma once

struct AnimationComponentData;
struct GraphicsComponentData;
struct HealthComponentData;
struct NetworkComponentData;
struct ProjectileComponentData;
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
	void LoadGraphicsComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, GraphicsComponentData& aOutputData);
	void LoadHealthComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, HealthComponentData& aOutputData);
	void LoadNetworkComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, NetworkComponentData& aOutputData);
	void LoadProjectileComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, ProjectileComponentData& aOutputData);
	void LoadTriggerComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, TriggerComponentData& aOutputData);
};