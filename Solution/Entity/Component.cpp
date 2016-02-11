#include "stdafx.h"
#include "Component.h"

Component::Component(Entity& aEntity)
	: myEntity(aEntity)
{
}

Component::~Component()
{
}

void Component::Reset()
{
}

void Component::Init()
{
}

void Component::Update(float)
{
}