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

	void TakeDamage(int aDamage);

private:
	
	int myMaxHealth;
	int myCurrentHealth;
};

