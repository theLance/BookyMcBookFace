#pragma once

#include <sstream>
#include <string>

/// Convenience converter. Assumes str is fully numeric
int strToInt(const std::string& str) {
    std::stringstream ss(str);
    int num;
    ss >> num;
    return num;
}