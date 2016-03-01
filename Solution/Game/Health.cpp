#include "stdafx.h"
#include "Health.h"
#include <XMLReader.h>

Health::Health(XMLReader& aReader, tinyxml2::XMLElement* anElement)
{
	aReader.ForceReadAttribute(aReader.ForceFindFirstChild(anElement, "maxHealth"), "value", myMaxHealth);
	myCurrentHealth = myMaxHealth;
}

Health::~Health()
{
}

bool Health::TakeDamage(int aDamage)
{
	myCurrentHealth -= aDamage;
	if (myCurrentHealth <= 0)
	{
		myCurrentHealth = 0;
		//myEntity.Kill();
		return false;
	}
	return true;
}

void Health::Reset()
{
	myCurrentHealth = myMaxHealth;
}