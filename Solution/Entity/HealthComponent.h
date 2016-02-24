#pragma once
#include "Component.h"

class Entity;

class HealthComponent : public Component
{
public:
	HealthComponent(Entity& anEntity);
	~HealthComponent();

private:

};

