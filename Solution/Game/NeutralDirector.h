#pragma once

#include "Director.h"
#include <XMLReader.h>

class NeutralDirector : public Director
{
	friend class LevelFactory;
public:
	NeutralDirector(const Prism::Terrain& aTerrain, Prism::Scene& aScene);
	~NeutralDirector();

	void ReceiveMessage(const SpawnUnitMessage& aMessage) override;
	void ReceiveMessage(const TimeMultiplierMessage& aMessage) override;

private:
};

