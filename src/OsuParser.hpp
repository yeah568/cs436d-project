#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#pragma once

struct OsuBeatmap {
	struct GeneralInfo generalInfo;
	struct Metadata metadata;
	struct Difficulty difficulty;

	std::vector<HitObject> hitObjects;
};

struct GeneralInfo {
	std::string audioFilename;
	int audioLeadIn;
	int previewTime;
	bool countdown;
	std::string sampleSet;
	float stackLeniency;
	int mode;
	bool letterboxInBreaks;
	bool widescreenStoryboard;
};

struct Metadata {
	std::string title;
	std::string titleUnicode;
	std::string artist;
	std::string artistUnicode;
	std::string creator;
	std::string version;
	std::string source;
	std::string tags; // should be a string list
	int beatmapID;
	int beatmapSetID;
};

struct Difficulty {
	float hpDrainRate;
	float circleSize;
	float overallDifficulty;
	float approachRate;
	float sliderMultiplier;
	float sliderTickRate;
};

// notes from osu docs:
/*
x and y are integers representing the position of the center of the hit object.
x ranges from 0 to 512 pixels, inclusive, and y ranges from 0 to 384 pixels,
inclusive. The origin, (0, 0) is at the top left of the screen.

To map these coordinates for a standard 640x480 screen, you need to add 64 
pixels to x and 48 pixels to y to respect a uniform padding. Without the 
padding, an object at (0, 0) will be cut on the top left for the screen.

For some hit objects, like spinners, the position is completely irrelevant.

For osu!mania, only x is relevant. See the osu!mania hold note section below.
*/

struct HitObject {
	int x;
	int y;
	// time is an integral number of milliseconds from the beginning of the 
	// song, and specifies when the hit begins.
	int time;
	// bitmap
	// Bit 0 (1): circle.
	// Bit 1 (2): slider.
	// Bit 2 (4): new combo.
	// Bit 3 (8): spinner.
	// Bits 4 - 6 (16, 32, 64) form a 3 - bit number(0 - 7) that chooses how many combo colours to skip.
	// Bit 7 (128) is for an osu!mania hold note.
	int type;
	// bitmap
	// Bit 0 (1): normal.
	// Bit 1 (2): whistle.
	// Bit 2 (4): finish.
	// Bit 3 (8): clap.
	int hitSound;

	// optionals
	int sampleSet;
	int additionSet;
	int customIndex;
	int sampleVolume;
	std::string filename;
};

struct HitCircle : HitObject {};
struct Slider : HitObject {
	
};
struct Spinner : HitObject {};
struct ManiaHold : HitObject {};


class OsuParser
{
public:
	OsuParser(const char* path);
	~OsuParser();

private:
	struct GeneralInfo parseGeneralInfo(std::string generalInfo);
	struct Metadata parseMetadata(std::string metadata);
	struct Difficulty parseDifficulty(std::string difficulty);
	std::ifstream osufile;
};

