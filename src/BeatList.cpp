#include "BeatList.hpp"

BeatList::BeatList(OsuBeatmap beatmap)
{
	int prevOffset = 0;
	auto tps = beatmap.timingPoints;
	int tps_index = 0;
	float last_duration = tps[0].millisecondsPerBeat;
	float duration = 0;
	auto tp_range = tps[tps_index+1].offset - tps[tps_index].offset;
	for (auto const& hitObject : beatmap.hitObjects) {
		float delta = hitObject.time - prevOffset;
		prevOffset = hitObject.time;

		float temp_delta = delta;
		while (temp_delta >= tp_range) {
			tps_index++;
			temp_delta -= tp_range;
			tp_range = tps[tps_index+1].offset - tps[tps_index].offset;
		}
		tp_range -= temp_delta;
		
		if (tps[tps_index].millisecondsPerBeat < 0) {
			duration = last_duration*tps[tps_index].millisecondsPerBeat/-100.0f;
		} else {
			duration = tps[tps_index].millisecondsPerBeat;
			last_duration = duration;
		}
		beats.push_back(Beat(hitObject.time, delta, hitObject.x, hitObject.y, duration));
	}
}

BeatList::~BeatList()
{
}
