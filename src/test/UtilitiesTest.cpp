//
// Created by Rayer Tung on 2019-08-23.
//

#include <gtest/gtest.h>
#include <PttCrawler.h>
#include <algorithm>
#include "Archiver.h"
#include <fstream>

TEST(Serialization, Serialize_Part_A) {
    auto crawler = new PttCrawler("gossiping");
    auto indexInfo = crawler->GetArticleInIndex(1000);
    std::for_each(indexInfo.articles.begin(), indexInfo.articles.end(), [crawler](ArticleInfo &a_info) -> void {
        crawler->ParseArticle(a_info);
    });

    std::ofstream ofs{"archive.txt"};
    boost::archive::text_oarchive ar{ofs};
    ar & indexInfo;
    ofs.close();
}


TEST(Serialization, Serialize_Part_B) {
    std::ifstream ifs{"archive.txt"};
    boost::archive::text_iarchive iar{ifs};
    IndexInfo indexInfo_re;
    iar & indexInfo_re;
    std::cout << indexInfo_re;
    ifs.close();
}

TEST(Serialization, ArchiveService) {
    auto crawler = new PttCrawler("gossiping");
    auto indexInfo = crawler->GetArticleInIndex(1000);
    std::for_each(indexInfo.articles.begin(), indexInfo.articles.end(), [crawler](ArticleInfo &a_info) -> void {
        crawler->ParseArticle(a_info);
    });
    std::list<IndexInfo> list;
    list.push_back(indexInfo);
    ArchiveService service;
    service.ArchiveFile("test.pcl", list);
    delete crawler;
}
