#pragma once

struct AnimationComponentData;
struct GraphicsComponentData;
struct ProjectileComponentData;
struct HealthComponentData;
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
	void LoadProjectileComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, ProjectileComponentData& aOutputData);
	void LoadHealthComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, HealthComponentData& aOutputData);
	void LoadTriggerComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, TriggerComponentData& aOutputData);
};