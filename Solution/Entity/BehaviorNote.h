#pragma once

struct BehaviorNote
{
	BehaviorNote(bool aFinished)
		: myFinished(aFinished) {}
	void operator=(BehaviorNote&) = delete;
	const bool myFinished;
};