#include "OsuParser.hpp"

#include "utils\StringUtils.hpp"

#include <string>
#include <sstream>


OsuParser::OsuParser(const char* path)
{
	osufile.open(path);

	std::string line;
	int lineNum = 0;
	
	OsuBeatmap beatmap = OsuBeatmap();
	
	std::string generalInfo;
	std::string metadata;


	while (getline(osufile, line))
	{
		if (line.find("[General]" == 0)) {
			while (getline(osufile, line) && line.find("[") != 0) {
				generalInfo.append(line);
				generalInfo.append("\n");
			}
		}		
		if (line.find("[Metadata]" == 0)) {
			while (getline(osufile, line) && line.find("[") != 0) {
				metadata.append(line);
				metadata.append("\n");
			}
		}
		std::cout << line << '\n';
	}

	beatmap.generalInfo = parseGeneralInfo(generalInfo);
	beatmap.metadata = parseMetadata(metadata);
}


OsuParser::~OsuParser()
{
	osufile.close();
}

struct GeneralInfo OsuParser::parseGeneralInfo(std::string generalInfo) {
	GeneralInfo gi = GeneralInfo();

	std::istringstream iss(generalInfo);

	for (std::string line; std::getline(iss, line); ) {
		std::string delim = ":";
		size_t pos = line.find(delim);
		std::string rest = line.substr(pos + 1);
		if (line.find("AudioFilename:") == 0) {
			gi.audioFilename = rest;
		}
		else if (line.find("AudioLeadIn:" == 0)) {
			gi.audioLeadIn = stoi(rest);
		}
		else if (line.find("PreviewTime:" == 0)) {
			gi.previewTime = stoi(rest);
		}
		else if (line.find("Countdown:" == 0)) {
			gi.countdown = !!stoi(rest); // warning-less cast to bool
		}
		else if (line.find("SampleSet:" == 0)) {
			gi.sampleSet = rest;
		}
		else if (line.find("StackLeniency:" == 0)) {
			gi.stackLeniency = stof(rest);
		}
		else if (line.find("Mode:" == 0)) {
			gi.mode = stoi(rest);
		}
		else if (line.find("LetterboxInBreaks:" == 0)) {
			gi.letterboxInBreaks = !!stoi(rest);
		}
		else if (line.find("WidescreenStoryboard:" == 0)) {
			gi.widescreenStoryboard = !!stoi(rest);
		}

		return gi;
	}
}

struct Editor OsuParser::parseEditor(std::string editor) {
	Editor e = Editor();

	std::istringstream iss(editor);

	for (std::string line; std::getline(iss, line); ) {
		std::string delim = ":";
		size_t pos = line.find(delim);
		std::string rest = line.substr(pos + 1);
		if (line.find("Bookmarks:") == 0) {
			std::vector<std::string> bookmarkStrings;
			str_split(bookmarkStrings, rest, ',');
			std::vector<int> bookmarks;

			e.bookmarks = bookmarks;
		}
		else if (line.find("DistanceSpacing:" == 0)) {
			e.distanceSpacing = stof(rest);
		}
		else if (line.find("BeatDivisor:" == 0)) {
			e.beatDivisor = stoi(rest);
		}
		else if (line.find("GridSize:" == 0)) {
			e.gridSize = stoi(rest); // warning-less cast to bool
		}
		else if (line.find("TimelineZoom:" == 0)) {
			e.timelineZoom = stoi(rest);
		}

		return e;
	}
}

struct Metadata OsuParser::parseMetadata(std::string metadata)
{
	Metadata m = Metadata();

	// TODO jamesliu: replace this with boost functions?
	std::istringstream iss(metadata);
	for (std::string line; std::getline(iss, line); )
	{
		std::string delim = ":";
		size_t pos = line.find(delim);
		std::string rest = line.substr(pos + 1);
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

struct Difficulty OsuParser::parseDifficulty(std::string difficulty)
{
	Difficulty d = Difficulty();

	// TODO jamesliu: replace this with boost functions?
	std::istringstream iss(difficulty);
	for (std::string line; std::getline(iss, line); )
	{
		std::string delim = ":";
		size_t pos = line.find(delim);
		std::string rest = line.substr(pos + 1);
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