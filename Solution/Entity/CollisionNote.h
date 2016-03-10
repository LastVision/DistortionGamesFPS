#pragma once

class Entity;

struct CollisionNote
{
	CollisionNote(Entity* anOther)
		: myOther(anOther) {}
	void operator=(CollisionNote&) = delete;


	Entity* myOther;
};