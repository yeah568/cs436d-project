#include "BeatList.hpp"



BeatList::BeatList(OsuBeatmap beatmap)
{
	int prevOffset = 0;
	for (auto const& hitObject : beatmap.hitObjects) {
		float delta = hitObject.time - prevOffset;
		prevOffset = hitObject.time;
		beats.push_back(Beat(delta, hitObject.x, hitObject.y));
	}
}


BeatList::~BeatList()
{
}
