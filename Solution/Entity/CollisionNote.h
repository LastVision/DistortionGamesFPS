#pragma once

class Entity;

struct CollisionNote
{
	CollisionNote(Entity* anOther, bool aHasEntered)
		: myOther(anOther)
		, myHasEntered(aHasEntered) {}

	void operator=(CollisionNote&) = delete;


	Entity* myOther;
	bool myHasEntered;
};