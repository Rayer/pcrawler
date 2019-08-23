//
// Created by Rayer Tung on 2019-08-23.
//

#include "Utilities.h"
#include <arpa/inet.h>

int ip4ToInteger(std::string ipStr) {
    return inet_addr(ipStr.c_str());
}

std::string integerToIp4String(int ip) {
    in_addr paddr;
    paddr.s_addr = ip;
    return std::string(inet_ntoa(paddr));
}
