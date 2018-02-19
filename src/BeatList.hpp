#pragma once
#include "OsuParser.hpp"
#include <vector>

struct Beat {	
	float offset;
	int x;
	int y;
	float duration;

	Beat() : offset(0.f), x(0), y(0), duration(0) {}
	Beat(float offset, int x, int y, float duration) : offset(offset), x(x), y(y), duration(duration) {}
};

class BeatList
{
public:
	BeatList(OsuBeatmap beatmap);
	~BeatList();
	
	std::vector<Beat> beats;
};

