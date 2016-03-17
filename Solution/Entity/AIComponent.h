#pragma once
#include "Component.h"

class AIComponent : public Component
{
public:
	AIComponent(Entity& anEntity);
	~AIComponent();
};

