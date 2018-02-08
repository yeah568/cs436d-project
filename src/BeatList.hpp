#pragma once
#include "OsuParser.hpp"
#include <vector>

struct Beat {	
	float offset;
	int x;
	int y;

	Beat(float offset, int x, int y) : offset(offset), x(x), y(y) {}
};

class BeatList
{
public:
	BeatList(OsuBeatmap beatmap);
	~BeatList();
	
	std::vector<Beat> beats;
};

