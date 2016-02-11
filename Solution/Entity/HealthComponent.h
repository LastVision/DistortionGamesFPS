#pragma once

namespace GUI
{
	class BarWidget;
}

struct HealthComponentData;
class HealthComponent : public Component
{
public:
	HealthComponent(Entity& aEntity, HealthComponentData& aData);
	~HealthComponent();

	void RenderHealthBar(const Prism::Camera& aCamera);

	void Update(float aDelta) override;

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

	bool TakeDamageAndCheckSurvive(float aDamage);
	void Heal(float aHealing);

	void Reset() override;

	float GetArmor() const;
	float GetCurrentHealth() const;
	float GetMaxHealth() const;

	void SetArmor(float aArmor);

	void SetIsHealing(bool aBool);
	bool GetIsHealing() const;

private:
	float myMaxHealth;
	float myCurrentHealth;
	float myArmor;

	GUI::BarWidget* myHealthBar;

	bool myIsHealing;
};

inline eComponentType HealthComponent::GetTypeStatic()
{
	return eComponentType::HEALTH;
}

inline eComponentType HealthComponent::GetType()
{
	return GetTypeStatic();
}

inline float HealthComponent::GetArmor() const
{
	return myArmor;
}

inline float HealthComponent::GetCurrentHealth() const
{
	return myCurrentHealth;
}

inline float HealthComponent::GetMaxHealth() const
{
	return myMaxHealth;
}

inline void HealthComponent::SetArmor(float aArmor)
{
	myArmor = static_cast<float>(static_cast<int>(aArmor));
}

inline void HealthComponent::SetIsHealing(bool aBool)
{
	myIsHealing = aBool;
}

inline bool HealthComponent::GetIsHealing() const
{
	return myIsHealing;
}