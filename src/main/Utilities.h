//
// Created by Rayer Tung on 2019-08-23.
//

#ifndef ANALYZER_UTILITIES_H
#define ANALYZER_UTILITIES_H


#include <string>

int ip4ToInteger(std::string ipStr);

std::string integerToIp4String(int ip);

constexpr unsigned int str2int(const char *str, int h = 0) {
    return !str[h] ? 5381 : (str2int(str, h + 1) * 33) ^ str[h];
}

#endif //ANALYZER_UTILITIES_H
