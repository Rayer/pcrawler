//
// Created by Rayer on 2019-08-23.
//

#include <future>
#include <fstream>
#include <iostream>
#include "PttCrawlerTask.h"
#include "PttCrawler.h"
#include "IpAnalyzer.h"

PttCrawlerTask::PttCrawlerTask(const std::string &boardName) : crawler(new PttCrawler(boardName)) {}

PttCrawlerTask::~PttCrawlerTask() {
    delete crawler;
}

void PttCrawlerTask::startCrawl_recent(int pages, std::function<void(int, int)> callback) {
    int max_index = crawler->GetMaxIndex();
    std::list<std::future<IndexInfo>> threadList;
    for (int i = max_index; i > 0 && i > max_index - pages; --i) {
        threadList.push_back(std::async(&PttCrawler::GetArticleInIndex, crawler, i));
    }
    std::for_each(threadList.begin(), threadList.end(), [this](std::future<IndexInfo> &threadInfo) -> void {
        indexInfoList.push_back(threadInfo.get());
    });

    std::for_each(indexInfoList.begin(), indexInfoList.end(), [this](IndexInfo &info) -> void {
        std::list<std::future<void>> futureList;
        std::cout << "Start parsing index : " << info.index << std::endl;
        std::for_each(info.articles.begin(), info.articles.end(), [this, &futureList](ArticleInfo &ainfo) -> void {
            futureList.push_back(std::async(&PttCrawler::ParseArticle, crawler, std::ref(ainfo)));
        });
        std::for_each(futureList.begin(), futureList.end(), [](std::future<void> &threadInfo) -> void {
            threadInfo.get();
        });
    });

}

void PttCrawlerTask::generateReport(int nameIpCountThreshold, int ipNameCountThreshold, std::ostream &os) {
    IpAnalyzer *ipAnalyzer = new IpAnalyzer();

    std::for_each(indexInfoList.begin(), indexInfoList.end(), [&ipAnalyzer](const IndexInfo &i_info) -> void {
        ipAnalyzer->addParsedIndex(i_info);
    });

    os << "User have used " << nameIpCountThreshold << " and more IPs : " << std::endl;
    ipAnalyzer->printUserWithMultipleIp(os, nameIpCountThreshold);
    os << std::endl << "IPs with more then " << ipNameCountThreshold << " users :" << std::endl;
    ipAnalyzer->printIpSharedByMultipleUser(os, ipNameCountThreshold);
    os << std::endl << "Highlighted user's commit, who have appeared in above lists : " << std::endl;
    ipAnalyzer->whatDoesTheFoxSay(os);

    delete ipAnalyzer;
}
