//
// Created by Rayer on 2019-08-17.
//

#include <gtest/gtest.h>
#include "PttCrawler.h"
#include <iostream>
#include <boost/format.hpp>
#include <list>

static const int fetch_pages = 10;
static const char* target = "Gossiping";

TEST(Crawler_Test, IndexFetch) {


    Crawler* crawler = new Crawler(target);
    int max_index = crawler->GetMaxIndex();

    std::cout << "Max index for " << target << " is " << crawler->GetMaxIndex() << std::endl;
    for(int i = max_index; i > 0 && i > max_index - fetch_pages; --i) {
        std::string url = (boost::format("https://www.ptt.cc/bbs/%1%/index%2%.html") % target % i).str();
        std::cout << "Creating parsing request for : " << url << std::endl;
    }
    delete crawler;
}

TEST(Crawler_Test, IndexDetail) {
    Crawler* crawler = new Crawler(target);
    IndexInfo indexInfo = crawler->GetArticleInIndex(700);
    for (int i = 0; i < indexInfo.articles.size(); ++i) {
        std::cout << indexInfo.articles[i] << std::endl;
    }

}

TEST(CrawlerTest, DocParse) {
    std::string url = {"https://www.ptt.cc/bbs/Gossiping/M.1566137207.A.E21.html"};
    Crawler *crawler = new Crawler(target);

    ArticleInfo info;
    info.url = url;
    crawler->ParseArticle(info);

    delete crawler;
}


TEST(Crawler_Test, DocumentParse) {
    Crawler* crawler = new Crawler(target);

    IndexInfo indexInfo = crawler->GetArticleInIndex(700);
    std::transform(indexInfo.articles.begin(), indexInfo.articles.end(), indexInfo.articles.begin(), [=](ArticleInfo& info)->ArticleInfo{
        crawler->ParseArticle(info);
        return info;
    });

    std::cout << indexInfo << std::endl;

    delete crawler;
}

