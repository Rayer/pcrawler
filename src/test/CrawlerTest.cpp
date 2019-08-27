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

TEST(CrawlerTest, DocParse) {
    std::string url = {"https://www.ptt.cc/bbs/Gossiping/M.1566137207.A.E21.html"};
    auto *crawler = new PttCrawler(target);

    ArticleInfo info;
    info.url = url;
    crawler->ParseArticle(info);

    delete crawler;
}


TEST(Crawler_Test, DocumentParse) {
    auto *crawler = new PttCrawler(target);

    IndexInfo indexInfo = crawler->GetArticleInIndex(700);
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

        bool preProcessingDocument(const ArticleInfo &articleInfo) override {
            std::cout << "preProcessingDocument : " << articleInfo << std::endl;
            std::cout.flush();
            return true;
        }

        void doneProcessDocument(const ArticleInfo &articleInfo, int current, int total) override {
            std::cout << "doneProcessDocument : " << current << " " << total << " " << articleInfo << std::endl;
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
    auto *task = new PttCrawlerTask("Gossiping", cb);
    task->startCrawl_recent(20);
    task->generateReport(7, 2, std::cout);
    delete cb;
    delete task;
}