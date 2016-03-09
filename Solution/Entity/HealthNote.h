#pragma once

struct HealthNote
{
	HealthNote(int anAmount)
		: myAmount(anAmount) {}

	void operator=(HealthNote&) = delete;

	const int myAmount;
};