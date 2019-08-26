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

    virtual bool preProcessingDocument(const ArticleInfo &articleInfo) { return true; }

    virtual void doneProcessDocument(const ArticleInfo &articleInfo, int current, int total) {};

    virtual void analyerFinished(IpAnalyzer::ID_ADDR_MAP idAddrMap, IpAnalyzer::IP_SHARED_MAP ipSharedMap,
                                 IpAnalyzer::HIGHLIGHT_USER_MAP highlightUserMap) = 0;
};

class PttCrawlerTask {
    PttCrawlerTaskCallback *callback;
    PttCrawler *crawler;
    bool downloading = false;
    std::list<IndexInfo> indexInfoList;
public:
    explicit PttCrawlerTask(const std::string &boardName, PttCrawlerTaskCallback *callback = nullptr);

    void startCrawl_recent(int pages);

    void generateReport(int nameIpCountThreshold, int ipNameCountThreshold, std::ostream &os);

    virtual ~PttCrawlerTask();
};


#endif //ANALYZER_PTTCRAWLERTASK_H
