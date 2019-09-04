//
// Created by Rayer on 2019-08-17.
//

#include <gtest/gtest.h>
#include "PttCrawler.h"
#include <iostream>
#include <boost/format.hpp>
#include <list>
#include <IpAnalyzer.h>
#include <PttCrawlerTask.h>
#include "Utilities.h"

static const int fetch_pages = 10;
static const char* target = "Gossiping";

TEST(Crawler_Test, IndexFetch) {


    auto *crawler = new PttCrawler(target);
    int max_index = crawler->GetMaxIndex();

    std::cout << "Max index for " << target << " is " << crawler->GetMaxIndex() << std::endl;
    for(int i = max_index; i > 0 && i > max_index - fetch_pages; --i) {
        std::string url = (boost::format("https://www.ptt.cc/bbs/%1%/index%2%.html") % target % i).str();
        std::cout << "Creating parsing request for : " << url << std::endl;
    }
    delete crawler;
}

TEST(Crawler_Test, IndexDetail) {
    auto *crawler = new PttCrawler(target);
    IndexInfo indexInfo = crawler->GetArticleInIndex(700);
    std::for_each(indexInfo.articles.begin(), indexInfo.articles.end(), [](const ArticleInfo &info) -> void {
        std::cout << info << std::endl;
    });

}

TEST(Crawler_Test, PinnedArticleTest) {
    auto *crawler = new PttCrawler(target);
    IndexInfo indexInfo = crawler->GetArticleInIndex(crawler->GetMaxIndex());
    std::for_each(indexInfo.articles.begin(), indexInfo.articles.end(), [](const ArticleInfo &info) -> void {
        std::cout << info << std::endl;
    });

    for_each(indexInfo.articles.begin(), indexInfo.articles.end(), [crawler](ArticleInfo &article) -> void {
        crawler->ParseArticle(article);
        std::cout << article << std::endl;
    });

    delete crawler;
}

TEST(CrawlerTest, DocParse) {
    //std::string url = {"https://www.ptt.cc/bbs/Gossiping/M.1566137207.A.E21.html"};
    std::string url{"https://www.ptt.cc/bbs/Gossiping/M.1567144923.A.E94.html"};
    auto *crawler = new PttCrawler(target);

    ArticleInfo info;
    info.url = url;
    crawler->ParseArticle(info);
    std::cout << info << std::endl;

    delete crawler;
}


TEST(Crawler_Test, DocumentParse) {
    auto *crawler = new PttCrawler(target);

    IndexInfo indexInfo = crawler->GetArticleInIndex(crawler->GetMaxIndex());
    std::transform(indexInfo.articles.begin(), indexInfo.articles.end(), indexInfo.articles.begin(), [crawler](ArticleInfo& info)->ArticleInfo{
        crawler->ParseArticle(info);
        return info;
    });

    std::cout << indexInfo << std::endl;

    delete crawler;
}

TEST(Crawler_Test, MultipleIPDetect) {
    auto *crawler = new PttCrawler(target);

    IndexInfo indexInfo = crawler->GetArticleInIndex(700);
    std::transform(indexInfo.articles.begin(), indexInfo.articles.end(), indexInfo.articles.begin(),
                   [crawler](ArticleInfo &info) -> ArticleInfo {
                       crawler->ParseArticle(info);
                       return info;
                   });

    auto *ipAnalyzer = new IpAnalyzer();

    ipAnalyzer->addParsedIndex(indexInfo);
    IpAnalyzer::Result result = ipAnalyzer->analyze();
    ipAnalyzer->printReport(std::cout, result);

    delete ipAnalyzer;
    delete crawler;
}

TEST(CrawlerTask_Test, CallbackTest) {

    class Callback : public PttCrawlerTaskCallback {
        void processingIndex(int from, int to, int current) override {
            std::cout << "processingIndex : " << from << " " << to << " " << current << std::endl;
            std::cout.flush();
        }

        bool shouldIncludeInReport(const ArticleInfo &articleInfo) override {
            std::cout << "shouldIncludeInReport : " << articleInfo << std::endl;

            if (std::chrono::system_clock::now() - std::chrono::hours(48) > articleInfo.parsedTime) {
                std::cout << "Dropped article " << articleInfo.title << "(" << articleInfo.url
                          << ") due to it's 48 hours ago" << std::endl;
                return false;
            }
            return true;
        }

        void doneParseDocument(const ArticleInfo &articleInfo, int current, int total) override {
            std::cout << "doneParseDocument : " << current << " " << total << " " << articleInfo << std::endl;
            std::cout.flush();
        }

        void analyzeFinished(const IpAnalyzer::ID_IPS_MAP &idAddrMap, const IpAnalyzer::IP_IDS_MAP &ipSharedMap,
                             const IpAnalyzer::HIGHLIGHT_USER_MAP &highlightUserMap) override {
            std::cout << "analyzerFinished : " << idAddrMap.size() << " " << ipSharedMap.size() << " "
                      << highlightUserMap.size() << std::endl;
            std::cout.flush();

        }
    };

    auto *cb = new Callback();
    auto *task = new PttCrawlerTask(cb);
    task->startCrawl_recent("gossiping", 1);
    task->generateReport(7, 2, std::cout);
    delete cb;
    delete task;
}