#pragma once
#include "OsuParser.hpp"
#include <vector>

struct Beat {
	float absoluteOffset;
	float relativeOffset;
	int x;
	int y;
	float duration;

	Beat() : absoluteOffset(0.f), relativeOffset(0.f), x(0), y(0), duration(0) {}
	Beat(float absoluteOffset, float relativeOffset, int x, int y, float duration) : absoluteOffset(absoluteOffset), relativeOffset(relativeOffset), x(x), y(y), duration(duration) {}
};

class BeatList
{
public:
	BeatList(OsuBeatmap beatmap);
	~BeatList();
	
	std::vector<Beat> beats;
};

