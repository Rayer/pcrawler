//
// Created by Rayer Tung on 2019-08-23.
//

#ifndef ANALYZER_IPANALYZER_H
#define ANALYZER_IPANALYZER_H

#include <string>
#include <map>
#include <set>

class IndexInfo;

class ArticleInfo;

class IpAnalyzer {
    std::map<std::string, std::set<int>> ipAddrMap;
    std::map<int, std::set<std::string>> ipSharedMap;
public:
    void addParsedIndex(const IndexInfo &i_info);

    void addParsedDocument(const ArticleInfo &a_info);

    const std::map<std::string, std::set<int>> &getIpAddrMap();

    const std::map<int, std::set<std::string>> &getIpSharedMap();

    void printUserWithMultipleIp(int threshold = 2);

    void printIpSharedByMultipleUser(int threshold = 2);
};


#endif //ANALYZER_IPANALYZER_H
