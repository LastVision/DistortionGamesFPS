#pragma once
#include "Director.h"
#include <StaticArray.h>
#include "GameDefines.h"

namespace Prism
{
	class Camera;
	class Terrain;
	class Scene;
	class SpriteProxy;
	class SpriteAnimator;
}

namespace GUI
{
	class GUIManager;
	class Cursor;
}

namespace CU
{
	namespace Intersection
	{
		class LineSegment3D;
	}
}

class Entity;
class TextEventManager;

enum class eSelectedAction
{
	NONE,
	MOVE,
	ATTACK_TAGRET,
	STOP,
	ATTACK_MOVE,
	HOLD_POSITION,
	PLACE_TOTEM,
	ENRAGE
};

class BuildingComponent;
class AIDirector;

class PlayerDirector : public Director
{
public:
	PlayerDirector(const Prism::Terrain& aTerrain, Prism::Scene& aScene, GUI::Cursor* aCursor);
	~PlayerDirector();

	void InitGUI(const AIDirector* anAI, const Prism::Camera& aCamera, int aLeveID);

	void Update(float aDeltaTime, const Prism::Camera& aCamera);
	void Render(const Prism::Camera& aCamera);

	void OnResize(int aWidth, int aHeight);

	void SelectUnit(Entity* anEntity);

	void ReceiveMessage(const ToggleGUIMessage& aMessage) override;
	void ReceiveMessage(const OnClickMessage& aMessage) override;
	void ReceiveMessage(const TimeMultiplierMessage& aMessage) override;
	void ReceiveMessage(const MinimapMoveMessage& aMessage) override;
	void ReceiveMessage(const ToggleBuildTimeMessage& aMessage) override;
	void ReceiveMessage(const EventPositionMessage& aMessage) override;
	void ReceiveMessage(const SelectUnitMessage& aMessage) override;

	const CU::GrowingArray<Entity*>& GetSelectedUnits() const;
	CU::Vector3<float> GetCameraMoveVector() const;

	const int& GetGunpowder() const;
	const int& GetArtifact() const;
	const bool& GetRenderDragSelection() const;

	const CU::StaticArray<CU::GrowingArray<Entity*>, AMOUNT_OF_CONTROL_GROUPS>& GetControlGroups() const;

	eSelectedAction GetSelectedAction() const;

	void AttackMoveSelectedUnits(const CU::Vector2<float>& aPosition);

	const float& GetTotemCooldown() const;
	const float& GetTotemMaxCooldown() const;

	const bool& HasUnlockedTank() const;
	const bool& HasUnlockedRanger() const;
	const int& GetUpgradeLevel(int aUnitID) const;
	const float& GetUpgradeCooldown(int aUnitID) const;
	const float& GetUpgradeMaxCooldown(int aUnitID) const;
	const int& GetUnitCost(int aUnitID) const;
	const int& GetUnitSupplyCost(int aUnitID) const;
	const int& GetUpgradeCost(int aUnitID, int aUpgradeLevel) const;

	bool CanUpgrade(int aUnitType) const;
	bool CanAffordSupply(int aSupplyCost) const;
	bool CanAffordGunpowder(int aCost) const;
	bool CanAffordArtifact(int aCost) const;
	bool CanCastTotem() const;

private:
	void UpdateInputs();
	void UpdateControlGroups();
	void UpdateConfirmationAnimation(float aDeltaTime, const Prism::Camera& aCamera);
	void CameraFocusOnControlGroup(int aIndex);
	CU::Vector3<float> CalcCursorWorldPosition(const CU::Vector2<float>& aMousePosition, const Prism::Camera& aCamera);
	void UpdateMouseInteraction(const Prism::Camera& aCamera);
	void SelectOrHoverEntity(Entity* aEntity, bool &aSelected, bool &aHovered
		, const CU::Intersection::LineSegment3D& aMouseRay);
	void SelectAllUnits();
	void PlaceRallyPoint(CU::Vector2<float> aWorldPosition);

	void PlaceTotem(const CU::Vector3f& aPositionInWorld);

	void SelectControlGroup(int anIndex);

	CU::StaticArray<CU::GrowingArray<Entity*>, AMOUNT_OF_CONTROL_GROUPS> myControlGroups;
	int mySelectedControlGroup;

	CU::GrowingArray<Entity*> mySelectedUnits;
	GUI::GUIManager* myGUIManager;
	GUI::Cursor* myCursor;

	bool myShiftPressed;
	bool myControlPressed;
	bool myLeftMouseUp;
	bool myLeftMouseDown;
	bool myLeftMousePressed;
	bool myRightClicked;
	bool myMouseIsOverGUI;
	bool myRenderDragSelection;
	eSelectedAction mySelectedAction;

	bool myRenderGUI;

	bool myHasDoubleClicked;
	bool myHasClicked;
	bool myHasClickedF1;
	float myCurrentDoubleClickTimer;
	float myDoubleClickTime;

	float myTweakValueX;
	float myTweakValueY;

	float myCurrentCancleCursorTime;
	float myCancleCursorTime;

	CU::Vector2<float> mySelectionSpriteSize;
	CU::Vector2<float> mySelectionSpriteRenderPosition;
	CU::Vector2<float> mySelectionSpriteHotspot;

	CU::Vector2<float> myFirstMousePosition;
	CU::Vector3<float> myFirstCameraPosition;
	CU::Vector3<float> myFirstMousePositionInWorld;
	CU::GrowingArray<CU::Vector3<float>> myDragSelectionPositions;
	Prism::SpriteProxy* myDragSelectionSpriteVertical;
	Prism::SpriteProxy* myDragSelectionSpriteHorizontal;
	Prism::SpriteProxy* myDragSelectionSpriteVerticalFlip;
	Prism::SpriteProxy* myDragSelectionSpriteHorizontalFlip;

	Entity* myTotem;

	Entity* myRallyPoint;
	bool myRallypointVisible;

	int myAudioSFXID;
	int myMaxSelectedUnits;
	CU::Vector2<float> myLastEventPosition;
	bool myHasEventToGoTo;

	Prism::SpriteAnimator* myConfimrationAnimation;
	CU::Vector2<float> myConfirmationPosition;
	CU::Vector3<float> myConfimrationCameraPosition;

	TextEventManager* myTextEventManager;

	bool myIsFirstFrame;
};

inline const CU::GrowingArray<Entity*>& PlayerDirector::GetSelectedUnits() const
{
	return mySelectedUnits;
}

inline const int& PlayerDirector::GetGunpowder() const
{
	return myGunpowder;
}

inline const int& PlayerDirector::GetArtifact() const
{
	return myArtifacts;
}

inline const bool& PlayerDirector::GetRenderDragSelection() const
{
	return myRenderDragSelection;
}

inline const CU::StaticArray<CU::GrowingArray<Entity*>, AMOUNT_OF_CONTROL_GROUPS>& PlayerDirector::GetControlGroups() const
{
	return myControlGroups;
}

inline eSelectedAction PlayerDirector::GetSelectedAction() const
{
	return mySelectedAction;
}

inline const bool& PlayerDirector::HasUnlockedTank() const
{
	return myHasUnlockedTank;
}

inline const bool& PlayerDirector::HasUnlockedRanger() const
{
	return myHasUnlockedRanger;
}