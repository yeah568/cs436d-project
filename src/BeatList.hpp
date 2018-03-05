#pragma once
#include "OsuParser.hpp"
#include <vector>

enum Dir { left = 1, up, right, down };

struct Beat {
	float absoluteOffset;
	float relativeOffset;
	int x;
	int y;
	float duration;
	bool spawned;
	Dir dir;

	Beat() : absoluteOffset(0.f), relativeOffset(0.f), x(0), y(0), duration(0) {}
	Beat(float absoluteOffset, float relativeOffset, int x, int y, float duration);
};

class BeatList
{
public:
	BeatList(OsuBeatmap beatmap);
	~BeatList();
	
	std::vector<Beat> beats;
};

