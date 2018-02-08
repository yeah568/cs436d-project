#include "OsuParser.hpp"

#include <string>
#include <sstream>


OsuParser::OsuParser(const char* path)
{
	osufile.open(path);
}
 
OsuParser::~OsuParser()
{
	osufile.close();
}

OsuBeatmap OsuParser::parse() {
	std::string line;
	OsuBeatmap beatmap = OsuBeatmap();

	// check file version
	getline(osufile, line);
	if (line.compare("osu file format v14") != 0) {
		// TODO jamesliu: return an error
		return beatmap;
	}

	while (getline(osufile, line))
	{
		if (line.compare("[General]") == 0) {
			std::string generalInfo;
			while (getline(osufile, line)) {
				if (line.find("[") == 0) {
					break;
				}
				else {
					generalInfo.append(line);
					generalInfo.append("\n");
				}
			}
			trim(generalInfo);
			beatmap.generalInfo = parseGeneralInfo(generalInfo);
		}
		if (line.compare("[Editor]") == 0) {
			std::string editor;
			while (getline(osufile, line)) {
				if (line.find("[") == 0) {
					break;
				}
				else {
					editor.append(line);
					editor.append("\n");
				}
			}
			trim(editor);
			beatmap.editor = parseEditor(editor);
		}
		if (line.compare("[Metadata]") == 0) {
			std::string metadata;
			while (getline(osufile, line)) {
				if (line.find("[") == 0) {
					break;
				}
				metadata.append(line);
				metadata.append("\n");
			}
			trim(metadata);
			beatmap.metadata = parseMetadata(metadata);
		}
		if (line.compare("[Difficulty]") == 0) {
			std::string difficulty;
			while (getline(osufile, line)) {
				if (line.find("[") == 0) {
					break;
				}
				difficulty.append(line);
				difficulty.append("\n");
			}
			trim(difficulty);
			beatmap.difficulty = parseDifficulty(difficulty);
		}
		if (line.compare("[TimingPoints]") == 0) {
			std::string timingPoints;
			while (getline(osufile, line)) {
				if (line.find("[") == 0) {
					break;
				}
				timingPoints.append(line);
				timingPoints.append("\n");
			}
			trim(timingPoints);
			beatmap.timingPoints = parseTimingPoints(timingPoints);
		}
		if (line.compare("[Colours]") == 0) {
			std::string colours;
			while (getline(osufile, line)) {
				if (line.find("[") == 0) {
					break;
				}
				colours.append(line);
				colours.append("\n");
			}
			trim(colours);
			beatmap.colours = parseColours(colours);
		}
		if (line.compare("[HitObjects]") == 0) {
			std::string hitObjects;
			while (getline(osufile, line)) {
				if (line.find("[") == 0) {
					break;
				}
				hitObjects.append(line);
				hitObjects.append("\n");
			}
			trim(hitObjects);
			beatmap.hitObjects = parseHitObjects(hitObjects);
		}

		std::cout << line << '\n';
	}
	
	return beatmap;
}

GeneralInfo OsuParser::parseGeneralInfo(std::string generalInfo) {
	GeneralInfo gi = GeneralInfo();

	std::istringstream iss(generalInfo);

	for (std::string line; std::getline(iss, line); ) {
		std::string delim = ":";
		size_t pos = line.find(delim);
		std::string rest = line.substr(pos + 1);
		trim(rest);
		if (line.find("AudioFilename:") == 0) {
			gi.audioFilename = rest;
		}
		else if (line.find("AudioLeadIn:") == 0) {
			gi.audioLeadIn = stoi(rest);
		}
		else if (line.find("PreviewTime:") == 0) {
			gi.previewTime = stoi(rest);
		}
		else if (line.find("Countdown:") == 0) {
			gi.countdown = !!stoi(rest); // warning-less cast to bool
		}
		else if (line.find("SampleSet:") == 0) {
			gi.sampleSet = rest;
		}
		else if (line.find("StackLeniency:") == 0) {
			gi.stackLeniency = stof(rest);
		}
		else if (line.find("Mode:") == 0) {
			gi.mode = stoi(rest);
		}
		else if (line.find("LetterboxInBreaks:") == 0) {
			gi.letterboxInBreaks = !!stoi(rest);
		}
		else if (line.find("WidescreenStoryboard:") == 0) {
			gi.widescreenStoryboard = !!stoi(rest);
		}
	}
	return gi;
}

Editor OsuParser::parseEditor(std::string editor) {
	Editor e = Editor();

	std::istringstream iss(editor);

	for (std::string line; std::getline(iss, line); ) {
		std::string delim = ":";
		size_t pos = line.find(delim);
		std::string rest = line.substr(pos + 1);
		trim(rest);
		if (line.find("Bookmarks:") == 0) {
			std::vector<std::string> bookmarkStrings = split(rest, ',');
			e.bookmarks = strs_to_ints(bookmarkStrings);
		}
		else if (line.find("DistanceSpacing:") == 0) {
			e.distanceSpacing = stof(rest);
		}
		else if (line.find("BeatDivisor:") == 0) {
			e.beatDivisor = stoi(rest);
		}
		else if (line.find("GridSize:") == 0) {
			e.gridSize = stoi(rest); // warning-less cast to bool
		}
		else if (line.find("TimelineZoom:") == 0) {
			e.timelineZoom = stoi(rest); // TODO jamesliu: spec says int, but decimals show up?
		}
	}
	return e;
}

Metadata OsuParser::parseMetadata(std::string metadata)
{
	Metadata m = Metadata();

	// TODO jamesliu: replace this with boost functions?
	std::istringstream iss(metadata);
	for (std::string line; std::getline(iss, line); )
	{
		std::string delim = ":";
		size_t pos = line.find(delim);
		std::string rest = line.substr(pos + 1);
		trim(rest);
		if (line.find("Title:") == 0) {
			m.title = rest;
		}
		else if (line.find("TitleUnicode:") == 0) {
			m.titleUnicode = rest;
		}
		else if (line.find("Artist:") == 0) {
			m.artist = rest;
		}
		else if (line.find("ArtistUnicode:") == 0) {
			m.artistUnicode = rest;
		}
		else if (line.find("Creator:") == 0) {
			m.creator = rest;
		}
		else if (line.find("Version:") == 0) {
			m.version = rest;
		}
		else if (line.find("Source:") == 0) {
			m.source = rest;
		}
		else if (line.find("Tags:") == 0) {
			m.tags = rest;
		}
		else if (line.find("BeatmapID:") == 0) {
			m.beatmapID = stoi(rest);
		}
		else if (line.find("BeatmapSetID:") == 0) {
			m.beatmapSetID = stoi(rest);
		}
	}

	return m;
}

Difficulty OsuParser::parseDifficulty(std::string difficulty)
{
	Difficulty d = Difficulty();

	// TODO jamesliu: replace this with boost functions?
	std::istringstream iss(difficulty);
	for (std::string line; std::getline(iss, line); )
	{
		std::string delim = ":";
		size_t pos = line.find(delim);
		std::string rest = line.substr(pos + 1);
		trim(rest);
		if (line.find("HPDrainRate:") == 0) {
			d.hpDrainRate = stof(rest);
		}
		else if (line.find("CircleSize:") == 0) {
			d.circleSize = stof(rest);
		}
		else if (line.find("OverallDifficulty:") == 0) {
			d.overallDifficulty = stof(rest);
		}
		else if (line.find("ApproachRate:") == 0) {
			d.approachRate = stof(rest);
		}
		else if (line.find("SliderMultiplier:") == 0) {
			d.sliderMultiplier = stof(rest);
		}
		else if (line.find("SliderTickRate:") == 0) {
			d.sliderTickRate = stof(rest);
		}
	}

	return d;
}

std::vector<Colour> OsuParser::parseColours(std::string colours) {
	trim(colours);
	std::vector<std::string> colourStrings = split(colours, '\n');
	std::vector<Colour> colourObjects;

	for (auto const& value : colourStrings) {
		std::vector<std::string> lineValues = split(value, ':');
		if (lineValues.at(0).find("Combo") == 0) {
			colourObjects.push_back(parseColour(lineValues.at(1)));

		}
	}

	return colourObjects;
}

Colour OsuParser::parseColour(std::string colour) {
	Colour c = Colour();
	
	trim(colour);

	std::vector<std::string> rgbStrings = split(colour, ',');

	c.r = stoi(rgbStrings.at(0));
	c.g = stoi(rgbStrings.at(1));
	c.b = stoi(rgbStrings.at(2));

	return c;
}

std::vector<TimingPoint> OsuParser::parseTimingPoints(std::string timingPoints) {
	trim(timingPoints);
	std::vector<std::string> timingPointStrings = split(timingPoints, '\n');
	std::vector<TimingPoint> timingPointObjects;

	for (auto const& value : timingPointStrings) {
		timingPointObjects.push_back(parseTimingPoint(value));
	}
	return timingPointObjects;
}

TimingPoint OsuParser::parseTimingPoint(std::string timingPoint)
{
	TimingPoint tp = TimingPoint();

	std::vector<std::string> parts = split(timingPoint, ',');

	tp.offset = stoi(parts.at(0));
	tp.millisecondsPerBeat = stof(parts.at(1));
	tp.meter = stoi(parts.at(2));
	tp.sampleSet = stoi(parts.at(3));
	tp.sampleIndex = stoi(parts.at(4));
	tp.volume = stoi(parts.at(5));
	tp.inherited = !!stoi(parts.at(6));
	tp.kialMode = !!stoi(parts.at(7));
	
	return tp;
}

std::vector<HitObject> OsuParser::parseHitObjects(std::string hitObjectsString) {
	trim(hitObjectsString);
	std::vector<HitObject> hitObjects;
	std::vector<std::string> hitObjectStrings = split(hitObjectsString, '\n');

	for (auto const& value : hitObjectStrings) {
		hitObjects.push_back(parseHitObject(value));
	}

	return hitObjects;
}

HitObject OsuParser::parseHitObject(std::string hitObjectString) {
	trim(hitObjectString);
	std::vector<std::string> parts = split(hitObjectString, ',');
	int type = stoi(parts.at(3));

	if (type & HIT_CIRCLE_TYPE) {
		return parseHitCircle(parts);
	}
	else if (type & SLIDER_TYPE) {
		return parseSlider(parts);
	}
	else if (type & SPINNER_TYPE) {
		return parseSpinner(parts);
	}
	else if (type & MANIA_HOLD_TYPE) {
		return parseManiaHold(parts);
	}
	return HitObject();
}

void OsuParser::parseHitObjectCommon(HitObject &hitObject, std::vector<std::string> parts) {
	hitObject.x = stoi(parts.at(0));
	hitObject.y = stoi(parts.at(1));
	hitObject.time = stoi(parts.at(2));
	hitObject.type = stoi(parts.at(3));
	hitObject.hitSound = stoi(parts.at(4));
}

HitCircle OsuParser::parseHitCircle(std::vector<std::string> parts) {
	HitCircle hitObject = HitCircle();
	parseHitObjectCommon(hitObject, parts);
	parseHitObjectExtras(hitObject, parts.back());
	return hitObject;
}

Slider OsuParser::parseSlider(std::vector<std::string> parts) {
	Slider hitObject = Slider();
	parseHitObjectCommon(hitObject, parts);

	hitObject.repeat = stoi(parts.at(6));
	hitObject.pixelLength = stof(parts.at(7));
	//hitObject.edgeHitsounds = strs_to_ints(split(parts.at(8), '|'));
	parseHitObjectExtras(hitObject, parts.back());
	return hitObject;
}

Spinner OsuParser::parseSpinner(std::vector<std::string> parts) {
	Spinner hitObject = Spinner();
	parseHitObjectCommon(hitObject, parts);
	hitObject.endTime = stoi(parts.at(5));
	parseHitObjectExtras(hitObject, parts.back());
	return hitObject;
}

ManiaHold OsuParser::parseManiaHold(std::vector<std::string> parts) {
	ManiaHold hitObject = ManiaHold();
	parseHitObjectCommon(hitObject, parts);
	hitObject.endTime = stoi(parts.at(5));
	parseHitObjectExtras(hitObject, parts.back());
	return hitObject;
}

void OsuParser::parseHitObjectExtras(HitObject &hitObject, std::string extras) {
	//trim(extras);
	//std::vector<std::string> parts = split(extras, ':');
	//parts.resize(5, "");
	//hitObject.sampleSet = stoi(parts.at(0));
	//hitObject.additionSet = stoi(parts.at(1));
	//hitObject.customIndex = stoi(parts.at(2));
	//hitObject.sampleVolume = stoi(parts.at(3));
	//hitObject.filename = parts.at(4);
}