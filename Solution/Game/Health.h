#pragma once

class XMLReader;

namespace tinyxml2
{
	class XMLElement;
}

class Health
{
public:
	Health(XMLReader& aReader, tinyxml2::XMLElement* anElement);
	~Health();

	bool TakeDamage(int aDamage);

	void Reset();

	int GetCurrentHealth();
	int GetMaxHealth();

private:
	
	int myMaxHealth;
	int myCurrentHealth;
};

inline int Health::GetCurrentHealth()
{
	return myCurrentHealth;
}

inline int Health::GetMaxHealth()
{
	return myMaxHealth;
}