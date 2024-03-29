//
// Created by Rayer on 2019-08-23.
//

#ifndef ANALYZER_PTTCRAWLERTASK_H
#define ANALYZER_PTTCRAWLERTASK_H

#include <string>
#include <list>
#include <ostream>
#include "PttCrawler.h"
#include "IpAnalyzer.h"

class PttCrawlerTaskCallback {
public:
    virtual void processingIndex(int from, int to, int current) {};

    virtual bool shouldIncludeInReport(const ArticleInfo &articleInfo) { return true; }

    virtual void doneParseDocument(const ArticleInfo &articleInfo, int current, int total) {};

    virtual void doneParseAllDocument(const std::list<IndexInfo> &info) {};

    virtual void analyzeFinished(const IpAnalyzer::ID_IPS_MAP &idAddrMap, const IpAnalyzer::IP_IDS_MAP &ipSharedMap,
                                 const IpAnalyzer::HIGHLIGHT_USER_MAP &highlightUserMap) = 0;
};

class PttCrawlerTask {
    PttCrawlerTaskCallback *callback;
    PttCrawler *crawler;
    std::list<IndexInfo> indexInfoList;
    unsigned long threadpool_size = 15;
    std::string boardName;

private:
    IpAnalyzer::Result doAnalyze_impl(int nameIpCountThreshold, int ipNameCountThreshold, IpAnalyzer *ipAnalyzer) const;

public:
    void setThreadpoolSize(unsigned long threadpoolSize);

    explicit PttCrawlerTask(PttCrawlerTaskCallback *callback = nullptr);

    void startCrawl_recent(const std::string &in_boardName, int pages);

    void load_snapshot(const std::string &filename);

    void doAnalyze(int nameIpCountThreshold, int ipNameCountThreshold);

    void generateReport(int nameIpCountThreshold, int ipNameCountThreshold, std::ostream &os);

    virtual ~PttCrawlerTask();


};


#endif //ANALYZER_PTTCRAWLERTASK_H
