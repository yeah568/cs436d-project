#pragma once
#include "OsuParser.hpp"
#include <vector>

struct Beat {	
	float offset;
	int x;
	int y;
	float duration;

	HitObject& hitObject;
	Beat(float offset, int x, int y, float duration, HitObject& hitObject) : offset(offset), x(x), y(y), duration(duration), hitObject(hitObject) {}
};

class BeatList
{
public:
	BeatList(OsuBeatmap* beatmap);
	~BeatList();
	
	std::vector<Beat> beats;
};

