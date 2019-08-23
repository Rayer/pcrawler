//
// Created by Rayer on 2019-08-23.
//

#ifndef ANALYZER_PTTCRAWLERTASK_H
#define ANALYZER_PTTCRAWLERTASK_H

#include <string>
#include <list>
#include <ostream>
#include "PttCrawler.h"

class PttCrawlerTask {
    PttCrawler *crawler;
    bool downloading = false;
    std::list<IndexInfo> indexInfoList;
public:
    explicit PttCrawlerTask(const std::string &boardName);

    void startCrawl_recent(int pages, std::function<void(int current, int total)> callback = nullptr);

    void generateReport(int nameIpCountThreshold, int ipNameCountThreshold, std::ostream &os);

    virtual ~PttCrawlerTask();
};


#endif //ANALYZER_PTTCRAWLERTASK_H
