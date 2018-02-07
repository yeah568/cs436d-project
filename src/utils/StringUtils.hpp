#pragma once

#include <string>
#include <vector>
#include <iterator>
#include <sstream>

void ltrim(std::string & s);

void rtrim(std::string & s);

void trim(std::string & s);

std::vector<std::string> split(const std::string & s, char delim);
