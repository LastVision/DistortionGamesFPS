#pragma once
#include <Matrix.h>
#include <StaticArray.h>

namespace Prism
{
	class Camera;
	class Scene;
	class Terrain;
	enum class eOctreeType;
	class ParticleEmitterInstance;
}

class Component;

struct EntityData;


class Entity
{
	friend class ActorComponent;
	friend class AnimationComponent;
	friend class BuildingComponent;
	friend class ControllerComponent;
	friend class GraphicsComponent;
	friend class EntityFactory;

public:
	Entity(eOwnerType aOwner, Prism::eOctreeType anOctreeType, EntityData& aEntityData,
		Prism::Scene& aScene, const CU::Vector3<float>& aStartPosition, const Prism::Terrain& aTerrain, 
		const CU::Vector3f& aRotation, const CU::Vector3f& aScale, eUnitType aUnitType);
	~Entity();

	void Reset();

	virtual void Update(float aDeltaTime);

	void AddComponent(Component* aComponent);
	void RemoveComponent(eComponentType aComponent);

	template <typename T>
	T* GetComponent();

	template <typename T>
	const T* GetComponent() const;

	template <typename T>
	void SendNote(const T& aNote);

	void AddToScene();
	void RemoveFromScene();
	void RemoveSelectionRingFromScene();

	const CU::Matrix44<float>& GetOrientation() const;
	void SetPosition(const CU::Vector3f& aPosition);
	const CU::Vector2<float>& GetPosition() const;
	Prism::Scene& GetScene();
	eOwnerType GetOwner() const;
	eOwnerType GetTemporaryOwner() const;
	void SetOwner(eOwnerType anOwner);
	eEntityType GetType() const;
	const std::string& GetSubType() const;
	eUnitType GetUnitType() const;
	eEntityState GetState() const;
	void SetState(eEntityState aState);
	Prism::eOctreeType GetOctreeType() const;
	bool GetAlive() const;
	void Kill();
	void Spawn(const CU::Vector2<float>& aSpawnPoint, const CU::Vector2<float>& aRallyPoint);

	void SetSelect(bool aStatus);
	bool IsSelected() const;

	void SetHovered(bool aStatus);
	bool IsHovered() const;

	void SetShouldRender(bool aStatus);

	bool GetShouldBeRemoved() const;

	CU::GrowingArray<CU::Vector2<float>> GetCutMesh() const;

	int GetId() const;
	float GetMaxSpeed() const;
	void SetMaxSpeed(float aSpeed);

	const CU::Vector2<float>& GetVelocity() const;

	void SetTemporaryOwner(eOwnerType aOwner);
	void RestoreRealOwner();

	void SetSelectable(const bool aFlag);
	const bool IsSelectable() const;

	const bool IsInScene() const;

	void AddEmitter(Prism::ParticleEmitterInstance* anEmitterConnection);

	Prism::ParticleEmitterInstance* GetEmitter();

private:
	void UpdateArtifact(float aDeltaTime);
	void operator=(Entity&) = delete;
	CU::StaticArray<Component*, static_cast<int>(eComponentType::_COUNT)> myComponents;

	Prism::ParticleEmitterInstance* myEmitterConnection;

	int myId;
	
	bool myAlive;
	const eEntityType myType;
	std::string mySubType;
	eUnitType myUnitType;
	eOwnerType myOwner;
	eOwnerType myTemporaryOwner;
	eEntityState myState;
	
	const Prism::eOctreeType myOctreeType;
	Prism::Scene& myScene;

	CU::Matrix44<float> myOrientation;
	CU::Vector2<float> myPosition;

	CU::Vector2<float> myVelocity;
	float myMaxSpeed;

	bool mySelected;
	bool myHovered;
	bool myIsInScene;
	bool myDecayFlag;
	bool myIsSelectable;

	float myCurrentDecayTime;

	float myArtifactTotalTime;
	float myOriginalY;
};

template <typename T>
inline T* Entity::GetComponent()
{
	return static_cast<T*>(myComponents[static_cast<int>(T::GetTypeStatic())]);
}

template <typename T>
inline const T* Entity::GetComponent() const
{
	return static_cast<T*>(myComponents[static_cast<int>(T::GetTypeStatic())]);
}

template <typename T>
inline void Entity::SendNote(const T& aMessage)
{
	for (int i = 0; i < static_cast<int>(eComponentType::_COUNT); ++i)
	{
		if (myComponents[i] != nullptr)
		{
			myComponents[i]->ReceiveNote(aMessage);
		}
	}
}

inline const CU::Matrix44<float>& Entity::GetOrientation() const
{
	return myOrientation;
}

inline const CU::Vector2<float>& Entity::GetPosition() const
{
	return myPosition;
}

inline Prism::Scene& Entity::GetScene()
{
	return myScene;
}

inline eOwnerType Entity::GetOwner() const
{
	return myOwner;
}

inline eOwnerType Entity::GetTemporaryOwner() const
{
	return myTemporaryOwner;
}

inline eEntityType Entity::GetType() const
{
	return myType;
}

inline const std::string& Entity::GetSubType() const
{
	return mySubType;
}

inline eUnitType Entity::GetUnitType() const
{
	return myUnitType;
}

inline eEntityState Entity::GetState() const
{
	return myState;
}

inline void Entity::SetState(eEntityState aState)
{
	myState = aState;
}

inline Prism::eOctreeType Entity::GetOctreeType() const
{
	return myOctreeType;
}

inline bool Entity::GetAlive() const
{
	return myAlive;
}

inline int Entity::GetId() const
{
	return myId;
}

inline void Entity::SetOwner(eOwnerType anOwner)
{
	myOwner = anOwner;
}

inline float Entity::GetMaxSpeed() const
{
	return myMaxSpeed;
}

inline void Entity::SetMaxSpeed(float aSpeed)
{
	myMaxSpeed = aSpeed;
}

inline const CU::Vector2<float>& Entity::GetVelocity() const
{
	return myVelocity;
}

inline void Entity::SetTemporaryOwner(eOwnerType aOwner)
{
	myTemporaryOwner = aOwner;
}

inline void Entity::RestoreRealOwner()
{
	myTemporaryOwner = myOwner;
}

inline void Entity::SetSelectable(const bool aFlag)
{
	myIsSelectable = aFlag;
}

inline const bool Entity::IsSelectable() const
{
	return myIsSelectable;
}

inline const bool Entity::IsInScene() const
{
	return myIsInScene;
}