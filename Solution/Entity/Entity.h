#pragma once
#include <Matrix.h>
#include <StaticArray.h>
#include "Component.h"

namespace Prism
{
	class Camera;
	class Scene;
	class ParticleEmitterInstance;
	class PhysEntity;
}

class Component;

struct EntityData;

class Entity
{
	friend class AnimationComponent;
	friend class GraphicsComponent;
	friend class EntityFactory;

public:
	Entity(const EntityData& aEntityData, Prism::Scene* aScene, bool aClientSide, const CU::Vector3<float>& aStartPosition, 
		const CU::Vector3f& aRotation, const CU::Vector3f& aScale);
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
	void SetPosition(const CU::Vector3f& aPosition);
	const CU::Matrix44<float>& GetOrientation() const;
	void SetOrientation(const CU::Matrix44<float>& aOrientation);
	float* GetOrientationAsFloatPtr();

	Prism::Scene* GetScene();
	eEntityType GetType() const;

	eEntityState GetState() const;
	void SetState(eEntityState aState);

	void AddEmitter(Prism::ParticleEmitterInstance* anEmitterConnection);
	Prism::ParticleEmitterInstance* GetEmitter();

	Prism::PhysEntity* GetPhysEntity() const;
	void Kill();

	bool GetIsClient();

private:
	void operator=(Entity&) = delete;

	const EntityData& myEntityData;
	CU::StaticArray<Component*, static_cast<int>(eComponentType::_COUNT)> myComponents;

	Prism::ParticleEmitterInstance* myEmitterConnection;

	bool myAlive;
	bool myIsClientSide;
	bool myIsInScene;
	std::string mySubType;
	eEntityState myState;

	Prism::Scene* myScene;

	CU::Matrix44<float> myOrientation;
	Prism::PhysEntity* myPhysEntity;
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

inline float* Entity::GetOrientationAsFloatPtr()
{
	return &myOrientation.myMatrix[0];
}

inline Prism::Scene* Entity::GetScene()
{
	DL_ASSERT_EXP(myIsClientSide == true, "You can't get the scene on server side.");
	return myScene;
}

inline eEntityState Entity::GetState() const
{
	return myState;
}

inline void Entity::SetState(eEntityState aState)
{
	myState = aState;
}