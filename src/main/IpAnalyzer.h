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
    typedef std::map<std::string, std::set<int>> ID_IPS_MAP;
    typedef std::map<int, std::set<std::string>> IP_IDS_MAP;
    typedef std::map<std::string, std::list<std::string>> HIGHLIGHT_USER_MAP;

    struct Result {
        ID_IPS_MAP idIpsMap;
        IP_IDS_MAP ipIdsMap;
        HIGHLIGHT_USER_MAP highlightUserMap;
        int idWithMultiIpThreshold;
        int ipWithMultiIdThreshold;
    };

private:
    ID_IPS_MAP idIpsMap;
    IP_IDS_MAP ipIdsMap;
    std::list<ArticleInfo> articleInfoList;

public:

    void addParsedIndex(const IndexInfo &i_info);
    void addParsedDocument(const ArticleInfo &a_info);

    Result analyze(int idWithMultiIpThreshold = 3, int IpWithMultiIdThreshold = 2);

    void printReport(std::ostream &os, const Result &result);
};


#endif //ANALYZER_IPANALYZER_H
