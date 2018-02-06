#include "OsuParser.hpp"

#include <string>
#include <sstream>

OsuParser::OsuParser(const char* path)
{
	osufile.open(path);

	std::string line;
	int lineNum = 0;
	Metadata m;

	std::string metadata;
	while (getline(osufile, line))
	{
		if (line.find("[Metadata]" == 0)) {
			while (getline(osufile, line) && line.find("[") != 0) {
				metadata.append(line);
				metadata.append("\n");
			}
		}
		std::cout << line << '\n';
	}

	m = parseMetadata(metadata);
	m = parseMetadata(metadata);

}


OsuParser::~OsuParser()
{
	osufile.close();
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
