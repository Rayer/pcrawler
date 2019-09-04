//
// Created by Rayer on 2019-08-23.
//

#include <future>
#include "PttCrawlerTask.h"
#include "PttCrawler.h"
#include "IpAnalyzer.h"
#include "ThreadPool.h"
#include "Archiver.h"

PttCrawlerTask::PttCrawlerTask(PttCrawlerTaskCallback *callback) : crawler(nullptr), callback(callback) {}

PttCrawlerTask::~PttCrawlerTask() {
    delete crawler;
}

void PttCrawlerTask::startCrawl_recent(const std::string &in_boardName, int pages) {
    this->boardName = in_boardName;
    crawler = new PttCrawler(boardName);
    int max_index = crawler->GetMaxIndex();
    std::list<std::future<IndexInfo>> threadList;
    int from = max_index;
    int to = max_index - pages;

    ThreadPool index_tp{threadpool_size};

    for (int i = from; i > 0 && i >= to; --i) {
        //threadList.push_back(std::async(&PttCrawler::GetArticleInIndex, crawler, i));
        threadList.push_back(index_tp.enqueue(std::bind(&PttCrawler::GetArticleInIndex, crawler, i)));
    }
    int articleCount = 0;
    std::for_each(threadList.begin(), threadList.end(),
                  [this, from, to, &articleCount](std::future<IndexInfo> &threadInfo) -> void {
        IndexInfo info = threadInfo.get();
        if (callback != nullptr) {
            callback->processingIndex(from, to, info.index);
        }
        indexInfoList.push_back(info);
                      articleCount += info.articles.size();
    });

    ThreadPool article_tp{threadpool_size};
    std::list<std::future<ArticleInfo>> futureList;

    std::for_each(indexInfoList.begin(), indexInfoList.end(),
                  [this, articleCount, &article_tp, &futureList](IndexInfo &info) mutable -> void {
                      //articleInfo.insert(articleInfo.end(), info.articles.begin(), info.articles.end());
                      std::for_each(info.articles.begin(), info.articles.end(),
                                    [this, &futureList, articleCount, &article_tp](
                                            ArticleInfo &articleInfo) -> void {
                                        futureList.push_back(
                                                //std::async(&PttCrawler::ParseArticle, crawler, std::ref(articleInfo)));
                                                article_tp.enqueue(std::bind(&PttCrawler::ParseArticle, crawler,
                                                                             std::ref(articleInfo))));
                                    });


                  });

    std::for_each(futureList.begin(), futureList.end(),
                  [this, currentArticle = 1, &futureList, articleCount](
                          std::future<ArticleInfo> &threadInfo) mutable -> void {
                      ArticleInfo info = threadInfo.get();
                      if (callback != nullptr) {
                          callback->doneParseDocument(info, currentArticle, articleCount);
                      }
                      currentArticle++;
                  });

    if (callback != nullptr) {
        callback->doneParseAllDocument(indexInfoList);
    }
}

void PttCrawlerTask::doAnalyze(int nameIpCountThreshold, int ipNameCountThreshold) {
    IpAnalyzer* analyzer = new IpAnalyzer();

    IpAnalyzer::ID_IPS_MAP idIpsMap;
    IpAnalyzer::IP_IDS_MAP ipIdsMap;
    IpAnalyzer::Result result = analyzer->analyze(nameIpCountThreshold, ipNameCountThreshold);
    if(callback != nullptr) {
        callback->analyzeFinished(result.idIpsMap, result.ipIdsMap, result.highlightUserMap);
    }

    delete analyzer;
}


void PttCrawlerTask::generateReport(int nameIpCountThreshold, int ipNameCountThreshold, std::ostream &os) {
    auto *ipAnalyzer = new IpAnalyzer();

    std::for_each(indexInfoList.begin(), indexInfoList.end(), [&ipAnalyzer, this](const IndexInfo &i_info) -> void {
        //ipAnalyzer->addParsedIndex(i_info);
        std::for_each(i_info.articles.begin(), i_info.articles.end(),
                      [&ipAnalyzer, this](const ArticleInfo &a_info) -> void {
                          if (callback != nullptr && !callback->shouldIncludeInReport(a_info)) {
                              return;
                          }
                          ipAnalyzer->addParsedDocument(a_info);
                      });
    });

    IpAnalyzer::Result result = ipAnalyzer->analyze(nameIpCountThreshold, ipNameCountThreshold);

    if (callback != nullptr) {
        callback->analyzeFinished(result.idIpsMap, result.ipIdsMap, result.highlightUserMap);
    }

    ipAnalyzer->printReport(os, result);

    delete ipAnalyzer;
}

void PttCrawlerTask::setThreadpoolSize(unsigned long threadpoolSize) {
    threadpool_size = threadpoolSize;
}

void PttCrawlerTask::load_snapshot(const std::string &filename) {
    ArchiveService service;
    ArchivedInfo archivedInfo = service.RestoreFromFile(filename);
    this->indexInfoList = archivedInfo.content;

    if (callback != nullptr) {
        callback->doneParseAllDocument(indexInfoList);
    }
}
