//
// Created by Rayer on 2019-08-14.
//

#include <gtest/gtest.h>
#include "PttCrawler.h"
#include <list>
#include <future>
#include <IpAnalyzer.h>

static const int fetch_pages = 10;
static const char *target = "gossiping";
static const int stress_fetch_pages = 30;

TEST(ThreadTest, StressIndexWithoutThread) {
    Crawler* crawler = new Crawler(target);
    int max_index = crawler->GetMaxIndex();
    for(int i = max_index; i > 0 && i > max_index - stress_fetch_pages; --i) {
        std::cout << crawler->GetArticleInIndex(i) << std::endl;
    }
    delete crawler;
}


TEST(ThreadTest, StressIndexWithFullThread) {
    Crawler* crawler = new Crawler(target);
    int max_index = crawler->GetMaxIndex();
    std::list<std::future<IndexInfo>> threadList;
    for(int i = max_index; i > 0 && i > max_index - stress_fetch_pages; --i) {
        threadList.push_back(std::async(&Crawler::GetArticleInIndex, crawler, i));
    }

    std::for_each(threadList.begin(), threadList.end(), [](auto& future)->void{
        std::cout << future.get() << std::endl;
    });

    delete crawler;
}

TEST(ThreadTest, StressDocParseWithoutThread) {
    Crawler* crawler = new Crawler(target);
    int max_index = crawler->GetMaxIndex();
    std::list<IndexInfo> indexInfoList;
    for(int i = max_index; i > 0 && i > max_index - stress_fetch_pages; --i) {
        indexInfoList.push_back(crawler->GetArticleInIndex(i));
    }

    std::for_each(indexInfoList.begin(), indexInfoList.end(), [crawler](IndexInfo& iinfo)->void{
        std::for_each(iinfo.articles.begin(), iinfo.articles.end(), [crawler](ArticleInfo& ainfo)->void{
            crawler->ParseArticle(ainfo);
        });
        std::cout << iinfo << std::endl;
    });

    delete crawler;
}

TEST(ThreadTest, StressDocParseWithThread) {
    Crawler* crawler = new Crawler(target);
    int max_index = crawler->GetMaxIndex();
    std::list<IndexInfo> indexInfoList;
    std::list<std::future<IndexInfo> > threadList;
    for(int i = max_index; i > 0 && i > max_index - stress_fetch_pages; --i) {
        threadList.push_back(std::async(&Crawler::GetArticleInIndex, crawler, i));
    }
    std::for_each(threadList.begin(), threadList.end(), [&indexInfoList](std::future<IndexInfo>& threadInfo)->void{
        indexInfoList.push_back(threadInfo.get());
    });
    //IndexInfo won't multi thread but articleList will
    std::for_each(indexInfoList.begin(), indexInfoList.end(), [crawler](IndexInfo& info)->void{
        std::list<std::future<void>> futureList;
        std::for_each(info.articles.begin(), info.articles.end(), [crawler, &futureList](ArticleInfo& ainfo)->void{
            futureList.push_back(std::async(&Crawler::ParseArticle, crawler, std::ref(ainfo)));
        });
        std::for_each(futureList.begin(), futureList.end(), [](std::future<void>& threadInfo)->void{
            threadInfo.get();
        });
        std::cout << info << std::endl;
    });

    delete crawler;
}


TEST(ThreadTest, StressMultiIpDetectionWithThread) {
    Crawler *crawler = new Crawler(target);
    int max_index = crawler->GetMaxIndex();
    std::list<IndexInfo> indexInfoList;
    std::list<std::future<IndexInfo> > threadList;
    for (int i = max_index; i > 0 && i > max_index - stress_fetch_pages; --i) {
        threadList.push_back(std::async(&Crawler::GetArticleInIndex, crawler, i));
    }
    std::for_each(threadList.begin(), threadList.end(), [&indexInfoList](std::future<IndexInfo> &threadInfo) -> void {
        indexInfoList.push_back(threadInfo.get());
    });
    //IndexInfo won't multi thread but articleList will
    IpAnalyzer *ipAnalyzer = new IpAnalyzer();
    std::for_each(indexInfoList.begin(), indexInfoList.end(), [crawler, ipAnalyzer](IndexInfo &info) -> void {
        std::list<std::future<void>> futureList;
        std::for_each(info.articles.begin(), info.articles.end(), [crawler, &futureList](ArticleInfo &ainfo) -> void {
            futureList.push_back(std::async(&Crawler::ParseArticle, crawler, std::ref(ainfo)));
        });
        std::for_each(futureList.begin(), futureList.end(), [](std::future<void> &threadInfo) -> void {
            threadInfo.get();
        });
        std::cout << info.index << std::endl;
        ipAnalyzer->addParsedIndex(info);
    });

    ipAnalyzer->printUserWithMultipleIp(std::cout, 4);
    ipAnalyzer->printIpSharedByMultipleUser(std::cout, 3);
    ipAnalyzer->whatDoesTheFoxSay(std::cout);

    delete ipAnalyzer;
    delete crawler;
}


