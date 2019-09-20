//
// Created by Rayer on 2019-08-14.
//

#include <gtest/gtest.h>
#include "PttCrawler.h"
#include <list>
#include <future>
#include <IpAnalyzer.h>
#include <fstream>

//static const int fetch_pages = 10;
static const char *target = "gossiping";
static const int stress_fetch_pages = 20;

TEST(ThreadTest, StressIndexWithFullThread) {
    PttCrawler *crawler = new PttCrawler(target);
    int max_index = crawler->GetMaxIndex();
    std::list<std::future<IndexInfo>> threadList;
    for(int i = max_index; i > 0 && i > max_index - stress_fetch_pages; --i) {
        threadList.push_back(std::async(&PttCrawler::GetArticleInIndex, crawler, i));
    }

    std::for_each(threadList.begin(), threadList.end(), [](auto& future)->void{
        std::cout << future.get() << std::endl;
    });

    delete crawler;
}


TEST(ThreadTest, StressDocParseWithThread) {
    PttCrawler *crawler = new PttCrawler(target);
    int max_index = crawler->GetMaxIndex();
    std::list<IndexInfo> indexInfoList;
    std::list<std::future<IndexInfo> > threadList;
    for(int i = max_index; i > 0 && i > max_index - stress_fetch_pages; --i) {
        threadList.push_back(std::async(&PttCrawler::GetArticleInIndex, crawler, i));
    }
    std::for_each(threadList.begin(), threadList.end(), [&indexInfoList](std::future<IndexInfo>& threadInfo)->void{
        indexInfoList.push_back(threadInfo.get());
    });
    //IndexInfo won't multi thread but articleList will
    std::for_each(indexInfoList.begin(), indexInfoList.end(), [crawler](IndexInfo& info)->void{
        std::list<std::future<ArticleInfo>> futureList;
        std::for_each(info.articles.begin(), info.articles.end(), [crawler, &futureList](ArticleInfo& ainfo)->void{
            futureList.push_back(std::async(&PttCrawler::ParseArticle, crawler, std::ref(ainfo)));
        });
        std::for_each(futureList.begin(), futureList.end(), [](std::future<ArticleInfo> &threadInfo) -> void {
            threadInfo.get();
        });
        std::cout << info << std::endl;
    });

    delete crawler;
}


TEST(ThreadTest, StressMultiIpDetectionWithThread) {
    PttCrawler *crawler = new PttCrawler(target);
    int max_index = crawler->GetMaxIndex();
    std::list<IndexInfo> indexInfoList;
    std::list<std::future<IndexInfo> > threadList;
    for (int i = max_index; i > 0 && i > max_index - stress_fetch_pages; --i) {
        threadList.push_back(std::async(&PttCrawler::GetArticleInIndex, crawler, i));
    }
    std::for_each(threadList.begin(), threadList.end(), [&indexInfoList](std::future<IndexInfo> &threadInfo) -> void {
        indexInfoList.push_back(threadInfo.get());
    });
    //IndexInfo won't multi thread but articleList will
    auto *ipAnalyzer = new IpAnalyzer();
    std::for_each(indexInfoList.begin(), indexInfoList.end(), [crawler, ipAnalyzer](IndexInfo &info) -> void {
        std::list<std::future<ArticleInfo>> futureList;
        std::for_each(info.articles.begin(), info.articles.end(), [crawler, &futureList](ArticleInfo &ainfo) -> void {
            futureList.push_back(std::async(&PttCrawler::ParseArticle, crawler, std::ref(ainfo)));
        });
        std::for_each(futureList.begin(), futureList.end(), [](std::future<ArticleInfo> &threadInfo) -> void {
            threadInfo.get();
        });
        std::cout << info.index << std::endl;
        ipAnalyzer->addParsedIndex(info);
    });

    std::ofstream of("/tmp/report.txt");

    IpAnalyzer::Result result = ipAnalyzer->analyze(8, 3);
    ipAnalyzer->printReport(of, result);

    of.close();
    delete ipAnalyzer;
    delete crawler;
}


