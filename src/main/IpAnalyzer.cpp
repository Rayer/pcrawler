//
// Created by Rayer Tung on 2019-08-23.
//

#include "IpAnalyzer.h"
#include "PttCrawler.h"
#include <iostream>
#include "Utilities.h"

void IpAnalyzer::addParsedIndex(const IndexInfo &i_info) {
    std::for_each(i_info.articles.begin(), i_info.articles.end(),
                  [this](const ArticleInfo &ainfo) -> void {
                      this->addParsedDocument(ainfo);
                  });
}

void IpAnalyzer::addParsedDocument(const ArticleInfo &a_info) {
    std::for_each(a_info.ipUserInfoMap.begin(), a_info.ipUserInfoMap.end(), [this](
            std::pair<std::string, std::set<int>> nameIpSetPair) -> void {
        std::string name = nameIpSetPair.first;
        std::set<int> &fromIpSet = nameIpSetPair.second;
        //std::cout << "Adding ip info set : " << fromIpSet.size() << " and ipAddrMap size is " << ipAddrMap.size() << std::endl;
        std::set<int> &toIpSet = ipAddrMap[name];
        toIpSet.insert(fromIpSet.begin(), fromIpSet.end());

        //Put ip into ip=>name map
        std::for_each(fromIpSet.begin(), fromIpSet.end(), [this, &name](int ip) -> void {
            ipSharedMap[ip].insert(name);
        });
    });
}

const std::map<std::string, std::set<int>> &IpAnalyzer::getIpAddrMap() {
    return this->ipAddrMap;
}

const std::map<int, std::set<std::string>> &IpAnalyzer::getIpSharedMap() {
    return this->ipSharedMap;
}

void IpAnalyzer::printUserWithMultipleIp(int threshold) {
    std::for_each(ipAddrMap.begin(), ipAddrMap.end(),
                  [threshold](const std::pair<std::string, std::set<int>> &nameIpSetPair) -> void {
                      if (nameIpSetPair.second.size() < threshold) return;
                      std::cout << nameIpSetPair.first << " : ";
                      std::for_each(nameIpSetPair.second.begin(), nameIpSetPair.second.end(), [](int ip) -> void {
                          std::cout << integerToIp4String(ip) << " ";
                      });
                      std::cout << std::endl;
                  });
}

void IpAnalyzer::printIpSharedByMultipleUser(int threshold) {
    std::for_each(ipSharedMap.begin(), ipSharedMap.end(),
                  [threshold](const std::pair<int, std::set<std::string>> &ipNameSetPair) -> void {
                      if (ipNameSetPair.second.size() < threshold) return;
                      std::cout << integerToIp4String(ipNameSetPair.first) << " : ";
                      std::for_each(ipNameSetPair.second.begin(), ipNameSetPair.second.end(),
                                    [](const std::string &name) -> void {
                                        std::cout << name << " ";
                                    });
                      std::cout << std::endl;
                  });
}



