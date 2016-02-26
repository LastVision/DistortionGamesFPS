#include "stdafx.h"
#include "Health.h"
#include <XMLReader.h>

Health::Health(XMLReader& aReader, tinyxml2::XMLElement* anElement)
{
	aReader.ForceReadAttribute(aReader.ForceFindFirstChild(anElement, "speed"), "value", myMaxHealth);
	myCurrentHealth = myMaxHealth;
}

Health::~Health()
{
}

void Health::TakeDamage(int aDamage)
{
	myCurrentHealth -= aDamage;
	if (myCurrentHealth <= 0)
	{
		myCurrentHealth = 0;
		//myEntity.Kill();
	}
}