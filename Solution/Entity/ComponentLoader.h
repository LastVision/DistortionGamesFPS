#pragma once

struct AnimationComponentData;
struct GraphicsComponentData;
struct ProjectileComponentData;
struct HealthComponentData;

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
};