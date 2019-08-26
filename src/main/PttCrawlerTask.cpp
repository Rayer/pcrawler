//
// Created by Rayer on 2019-08-23.
//

#include <future>
#include <fstream>
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

    //Very tricky one.....
    std::list<std::reference_wrapper<ArticleInfo>> articleInfo;

    std::for_each(indexInfoList.begin(), indexInfoList.end(), [&articleInfo](IndexInfo &info) -> void {
        articleInfo.insert(articleInfo.end(), info.articles.begin(), info.articles.end());
    });


    std::list<std::future<ArticleInfo>> futureList;
    std::for_each(articleInfo.begin(), articleInfo.end(), [this, &futureList](ArticleInfo &ainfo) -> void {
        if (callback != nullptr && !callback->preProcessingDocument(ainfo)) return;
        futureList.push_back(std::async(&PttCrawler::ParseArticle, crawler, std::ref(ainfo)));
    });
    std::for_each(futureList.begin(), futureList.end(),
                  [this, idx = 1, &futureList](std::future<ArticleInfo> &threadInfo) mutable -> void {
                      ArticleInfo info = threadInfo.get();
                      if (callback != nullptr) {
                          callback->doneProcessDocument(info, idx, futureList.size());
                      }
                      idx++;
                  });

}
//
//void PttCrawlerTask::doAnalyze(int nameIpCountThreshold, int ipNameCountThreshold) {
//    IpAnalyzer* analyzer = new IpAnalyzer();
//
//    IpAnalyzer::ID_IPS_MAP idIpsMap;
//    IpAnalyzer::IP_IDS_MAP ipIdsMap;
//    IpAnalyzer::Result result = analyzer->analyze(nameIpCountThreshold, ipNameCountThreshold);
//    if(callback != nullptr) {
//        callback->analyzeFinished(result.idIpsMap, result.ipIdsMap, result.highlightUserMap);
//    }
//
//    delete analyzer;
//}


void PttCrawlerTask::generateReport(int nameIpCountThreshold, int ipNameCountThreshold, std::ostream &os) {
    IpAnalyzer *ipAnalyzer = new IpAnalyzer();

    std::for_each(indexInfoList.begin(), indexInfoList.end(), [&ipAnalyzer](const IndexInfo &i_info) -> void {
        ipAnalyzer->addParsedIndex(i_info);
    });

    IpAnalyzer::Result result = ipAnalyzer->analyze(nameIpCountThreshold, ipNameCountThreshold);

    if (callback != nullptr) {
        callback->analyzeFinished(result.idIpsMap, result.ipIdsMap, result.highlightUserMap);
    }

    ipAnalyzer->printReport(os, result);

    delete ipAnalyzer;
}
