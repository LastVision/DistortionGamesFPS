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
	friend class AnimationComponent;
	friend class GraphicsComponent;
	friend class EntityFactory;

public:
	Entity(const EntityData& aEntityData, Prism::Scene& aScene, const CU::Vector3<float>& aStartPosition, 
		const CU::Vector3f& aRotation, const CU::Vector3f& aScale, eUnitType aUnitType);
	~Entity();

	void Reset();
	virtual void Update(float aDeltaTime);

	void AddComponent(Component* aComponent);
	void RemoveComponent(eComponentType aComponent);

	template <typename T> T* GetComponent();
	template <typename T> const T* GetComponent() const;
	template <typename T> void SendNote(const T& aNote);

	void AddToScene();
	void RemoveFromScene();

	const CU::Matrix44<float>& GetOrientation() const;
	void SetOrientation(const CU::Matrix44<float>& aOrientation);

	Prism::Scene& GetScene();
	eEntityType GetType() const;
	eUnitType GetUnitType() const;

	eEntityState GetState() const;
	void SetState(eEntityState aState);

	void AddEmitter(Prism::ParticleEmitterInstance* anEmitterConnection);
	Prism::ParticleEmitterInstance* GetEmitter();

private:
	void operator=(Entity&) = delete;
	CU::StaticArray<Component*, static_cast<int>(eComponentType::_COUNT)> myComponents;

	Prism::ParticleEmitterInstance* myEmitterConnection;

	bool myAlive;
	bool myIsInScene;
	const eEntityType myType;
	std::string mySubType;
	eUnitType myUnitType;
	eEntityState myState;
	
	Prism::Scene& myScene;

	CU::Matrix44<float> myOrientation;
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

inline void Entity::SetOrientation(const CU::Matrix44<float>& aOrientation)
{
	myOrientation = aOrientation;
}

inline Prism::Scene& Entity::GetScene()
{
	return myScene;
}

inline eEntityType Entity::GetType() const
{
	return myType;
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