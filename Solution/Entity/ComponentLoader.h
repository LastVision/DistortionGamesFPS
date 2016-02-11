#pragma once

struct ActorComponentData;
struct AnimationComponentData;
struct BuildingComponentData;
struct CollisionComponentData;
struct ControllerComponentData;
struct GraphicsComponentData;
struct HealthComponentData;
struct TriggerComponentData;
struct GrenadeComponentData;
struct PromotionComponentData;
struct SelectionComponentData;
struct SoundComponentData;

class XMLReader;

namespace tinyxml2
{
	class XMLElement;
}

class ComponentLoader
{
public:
	void LoadActorComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, ActorComponentData& aOutputData);
	void LoadAnimationComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, AnimationComponentData& aOutputData);
	void LoadBuidlingComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, BuildingComponentData& aOutputData);
	void LoadCollisionComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, CollisionComponentData& aOutputData);
	void LoadControllerComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, ControllerComponentData& aOutputData);
	void LoadGraphicsComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, GraphicsComponentData& aOutputData);
	void LoadHealthComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, HealthComponentData& aOutputData);
	void LoadTriggerComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, TriggerComponentData& aOutputData);
	void LoadEnrageComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, EnrageComponentData& aOutputData);
	void LoadGrenadeComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, GrenadeComponentData& aOutputData);
	void LoadPromotionComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, PromotionComponentData& aOutputData);
	void LoadSelectionComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, SelectionComponentData& aOutputData);
	void LoadSoundComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, SoundComponentData& aOutputData);
private:
	void FailedToReadChildElementMessage(const std::string& aElement, const std::string& aParent);
};