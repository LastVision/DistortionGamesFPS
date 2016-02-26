#pragma once

struct DamageNote
{
	DamageNote(int aDamage)
	 : myDamage(aDamage) {}

	void operator=(DamageNote&) = delete;

	const int myDamage;
};