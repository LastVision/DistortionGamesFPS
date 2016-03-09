#pragma once

class PhysicComponent;

struct CollisionNote
{
	CollisionNote(PhysicComponent* anOther)
		: myOther(anOther) {}
	void operator=(CollisionNote&) = delete;


	PhysicComponent* myOther;
};