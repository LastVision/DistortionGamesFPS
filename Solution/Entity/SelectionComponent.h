#pragma once

namespace Prism
{
	class Instance;
}

struct SelectionComponentData;

class SelectionComponent : public Component
{
public:
	SelectionComponent(Entity& aEntity, SelectionComponentData& aComponentData);
	~SelectionComponent();

	void Update(float aDeltaTime) override;

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

	Prism::Instance* GetSelectedInstance();
	Prism::Instance* GetHoveredInstance();

private:
	Prism::Instance* mySelectedInstance;
	Prism::Instance* myHoveredInstance;
	bool myPreviousHover;
	float myCullingRadius;
};

inline eComponentType SelectionComponent::GetTypeStatic()
{
	return eComponentType::SELECTION;
}

inline eComponentType SelectionComponent::GetType()
{
	return GetTypeStatic();
}

inline Prism::Instance* SelectionComponent::GetSelectedInstance()
{
	return mySelectedInstance;
}

inline Prism::Instance* SelectionComponent::GetHoveredInstance()
{
	return myHoveredInstance;
}