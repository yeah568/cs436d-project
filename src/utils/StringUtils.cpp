#include "StringUtils.hpp"

template<typename Out>
void str_split(Out result, const std::string &s, char delim) {
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		*(result++) = item;
	}
}