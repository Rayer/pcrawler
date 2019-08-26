//
// Created by Rayer Tung on 2019-08-23.
//

#ifndef ANALYZER_IPANALYZER_H
#define ANALYZER_IPANALYZER_H

#include <string>
#include <map>
#include <set>
#include <list>

class IndexInfo;

class ArticleInfo;

class IpAnalyzer {
public:
    typedef std::map<std::string, std::set<int>> IP_ADDR_MAP;
    typedef std::map<int, std::set<std::string>> IP_SHARED_MAP;
    typedef std::map<std::string, std::list<std::string>> HIGHLIGHT_USER_MAP;

private:
    IP_ADDR_MAP ipAddrMap;
    IP_SHARED_MAP ipSharedMap;
    //Highlight name, Highlight reason
    HIGHLIGHT_USER_MAP highlightMap;
    std::list<ArticleInfo> articleInfoList;
public:

    void addParsedIndex(const IndexInfo &i_info);

    void addParsedDocument(const ArticleInfo &a_info);

    const IP_ADDR_MAP &getIpAddrMap();

    const IP_SHARED_MAP &getIpSharedMap();

    const HIGHLIGHT_USER_MAP &getHighlightUserMap();

    void printUserWithMultipleIp(std::ostream &os, int threshold = 2);

    void printIpSharedByMultipleUser(std::ostream &os, int threshold = 2);

    void whatDoesTheFoxSay(std::ostream &os);
};


#endif //ANALYZER_IPANALYZER_H
