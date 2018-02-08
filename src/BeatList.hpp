#pragma once
#include "OsuParser.hpp"
#include <vector>

struct Beat {	
	int offset;
	int x;
	int y;

	Beat(int offset, int x, int y) : offset(offset), x(x), y(y) {}
};

class BeatList
{
public:
	BeatList(OsuBeatmap beatmap);
	~BeatList();
	
	std::vector<Beat> beats;
};

