//
// Created by Rayer on 2019-08-23.
//

#include <future>
#include <fstream>
#include <iostream>
#include "PttCrawlerTask.h"
#include "PttCrawler.h"
#include "IpAnalyzer.h"

PttCrawlerTask::PttCrawlerTask(const std::string &boardName, PttCrawlerTaskCallback *callback) : crawler(
        new PttCrawler(boardName)), callback(callback) {}

PttCrawlerTask::~PttCrawlerTask() {
    delete crawler;
}

void PttCrawlerTask::startCrawl_recent(int pages) {
    int max_index = crawler->GetMaxIndex();
    std::list<std::future<IndexInfo>> threadList;
    int from = max_index;
    int to = max_index - pages;
    for (int i = from; i > 0 && i > to; --i) {
        threadList.push_back(std::async(&PttCrawler::GetArticleInIndex, crawler, i));
    }
    std::for_each(threadList.begin(), threadList.end(), [this, from, to](std::future<IndexInfo> &threadInfo) -> void {
        IndexInfo info = threadInfo.get();
        if (callback != nullptr) {
            callback->processingIndex(from, to, info.index);
        }
        indexInfoList.push_back(info);
    });

    std::for_each(indexInfoList.begin(), indexInfoList.end(), [this](IndexInfo &info) -> void {
        std::list<std::future<ArticleInfo &>> futureList;
        //std::cout << "Start parsing index : " << info.index << std::endl;
        std::for_each(info.articles.begin(), info.articles.end(), [this, &futureList](ArticleInfo &ainfo) -> void {
            if (callback != nullptr && !callback->preProcessingDocument(ainfo)) return;
            futureList.push_back(std::async(&PttCrawler::ParseArticle, crawler, std::ref(ainfo)));
        });
        std::for_each(futureList.begin(), futureList.end(),
                      [this, idx = 1, &futureList](std::future<ArticleInfo &> &threadInfo) mutable -> void {
                          ArticleInfo &info = threadInfo.get();
                          if (callback != nullptr) {
                              callback->doneProcessDocument(info, idx, futureList.size());
                          }
                          idx++;
        });
    });

}

void PttCrawlerTask::generateReport(int nameIpCountThreshold, int ipNameCountThreshold, std::ostream &os) {
    IpAnalyzer *ipAnalyzer = new IpAnalyzer();

    std::for_each(indexInfoList.begin(), indexInfoList.end(), [&ipAnalyzer](const IndexInfo &i_info) -> void {
        ipAnalyzer->addParsedIndex(i_info);
    });

    if (callback != nullptr) {
        callback->analyerFinished(ipAnalyzer->getIpAddrMap(), ipAnalyzer->getIpSharedMap(),
                                  ipAnalyzer->getHighlightUserMap());
    }

    os << "User have used " << nameIpCountThreshold << " and more IPs : " << std::endl;
    ipAnalyzer->printUserWithMultipleIp(os, nameIpCountThreshold);
    os << std::endl << "IPs with more then " << ipNameCountThreshold << " users :" << std::endl;
    ipAnalyzer->printIpSharedByMultipleUser(os, ipNameCountThreshold);
    os << std::endl << "Highlighted user's commit, who have appeared in above lists : " << std::endl;
    ipAnalyzer->whatDoesTheFoxSay(os);

    delete ipAnalyzer;
}
