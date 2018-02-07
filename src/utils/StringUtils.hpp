#pragma once

#include <string>
#include <vector>
#include <iterator>
#include <sstream>

template<typename Out>
void str_split(Out result, const std::string &s, char delim);
