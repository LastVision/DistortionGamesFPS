#include "stdafx.h"

#include <stdlib.h>
#include "Time.h"
#include <time.h>

CU::Time::Time(CU::TimeUnit aMicroSeconds, CU::TimeUnit aFrameMicroSeconds)
{
	myMicroSeconds = aMicroSeconds;
	myFrameMicroSeconds = aFrameMicroSeconds;
}

CU::Time::~Time()
{
}

void CU::Time::DoSrand()
{
	srand(static_cast<unsigned int>(time(0)));
}