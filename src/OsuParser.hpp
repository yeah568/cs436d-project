#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "common.hpp"
#include "utils\StringUtils.hpp"

#pragma once

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

struct Editor {
	std::vector<int> bookmarks;
	float distanceSpacing;
	int beatDivisor;
	int gridSize;
	int timelineZoom;
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

struct TimingPoint {
	int offset; // milliseconds
	// when positive, millisecondsPerBeat is the duration of one beat
	// when negative, it is a percentage of previous non-negative
	// milliseconds per beat.
	// For instance, 3 consecutive timing points with 500, -50, -100 will
	// have a resulting beat duration of half a second, a quarter of a second,
	// and half a second, respectively.
	float millisecondsPerBeat;
	int meter; // number of beats in a measure
	int sampleSet;
	int sampleIndex;
	int volume; // 0 to 100
	bool inherited; // says if it can be inherited from. Redundant with mPB.
	bool kialMode;
};

struct Colour {
	int r;
	int g;
	int b;
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

#define HIT_CIRCLE_TYPE 1
#define SLIDER_TYPE 2
#define NEW_COMBO_TYPE 4
#define SPINNER_TYPE 8
#define MANIA_HOLD_TYPE 128

// no special fields
struct HitCircle : HitObject {};
struct CurvePoint {
	int x;
	int y;
};
struct Slider : HitObject {
	std::string sliderType; // TODO jamesliu: replace with enum?
	std::vector<CurvePoint> curvePoints;
	int repeat;
	float pixelLength;
};
struct Spinner : HitObject {
	int endTime;
};
struct ManiaHold : HitObject {
	int endTime;
};


struct OsuBeatmap {
	struct GeneralInfo generalInfo;
	struct Editor editor;
	struct Metadata metadata;
	struct Difficulty difficulty;

	std::vector<TimingPoint> timingPoints;
	std::vector<Colour> colours;
	std::vector<HitObject> hitObjects;
};

class OsuParser
{
public:
	OsuParser(const char* path);
	~OsuParser();
	struct OsuBeatmap parse();

private:
	std::ifstream osufile;

	struct GeneralInfo parseGeneralInfo(std::string generalInfo);
	struct Editor parseEditor(std::string editor);
	struct Metadata parseMetadata(std::string metadata);
	struct Difficulty parseDifficulty(std::string difficulty);
	std::vector<struct Colour> parseColours(std::string colours);
	struct Colour parseColour(std::string colour);
	std::vector<struct TimingPoint> parseTimingPoints(std::string timingPoints);
	struct TimingPoint parseTimingPoint(std::string tp);
	std::vector<struct HitObject> parseHitObjects(std::string hitObjects);
	struct HitObject parseHitObject(std::string hitObjectString);
	void parseHitObjectExtras(HitObject hitObject, std::string extras);
};

