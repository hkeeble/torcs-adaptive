
#ifndef _STRING_UTIL_
#define _STRING_UTIL_

#include <vector>
#include <string>
#include <sstream>
#include <iostream>

std::vector<std::string> split(const std::string &s, char delim)
{
	std::vector<std::string> elems = std::vector<std::string>();
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		if (!item.empty())
			elems.push_back(item);
	}

	return elems;
}

#endif // _STRING_UTIL_