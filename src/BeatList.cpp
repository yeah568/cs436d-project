#include "BeatList.hpp"

Beat::Beat(float absoluteOffset, float relativeOffset, int x, int y, float duration)
	: absoluteOffset(absoluteOffset), relativeOffset(relativeOffset), x(x), y(y), duration(duration), spawned(false) 
{
	float pX = ((64.f + (float)x) / 640.f);
	float pY = ((48.f + (float)y) / 480.f);
	float dA = pX - pY;
	float dB = -1 * (pX + pY) + 1;
	if (dA == 0 && dB == 0)
		dir = left;
	else if (dA < 0 && dB >= 0)
		dir = left;
	else if (dA >= 0 && dB >= 0)
		dir = up;
	else if (dA >= 0 && dB < 0)
		dir = right;
	else if (dA < 0 && dB < 0)
		dir = down;
}

BeatList::BeatList(OsuBeatmap beatmap)
{
	int prevOffset = 0;
	for (auto const& hitObject : beatmap.hitObjects) {
		int delta = hitObject.time - prevOffset;
		prevOffset = hitObject.time;

		beats.push_back(
			Beat(
				static_cast<float>(hitObject.time), 
				static_cast<float>(delta), 
				hitObject.x, 
				hitObject.y,
				0.f)
		);
	}
}

BeatList::~BeatList()
{
}
