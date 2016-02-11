#pragma once

struct GrenadeComponentData
{
	float myDelay;
	float myRange;
	float myRadius;
	float myDamage;
	float myCooldown;

	bool myAlliesShouldTakeDamage = false;
	bool myExistsInEntity = false;
};