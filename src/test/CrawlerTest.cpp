//
// Created by Rayer on 2019-08-17.
//

#include <gtest/gtest.h>
#include "PttCrawler.h"
#include <iostream>
#include <boost/format.hpp>
#include <list>
#include "Utilities.h"

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
    std::transform(indexInfo.articles.begin(), indexInfo.articles.end(), indexInfo.articles.begin(), [crawler](ArticleInfo& info)->ArticleInfo{
        crawler->ParseArticle(info);
        return info;
    });

    std::cout << indexInfo << std::endl;

    delete crawler;
}

TEST(Crawler_Test, MultipleIPDetect) {
    Crawler *crawler = new Crawler(target);

    IndexInfo indexInfo = crawler->GetArticleInIndex(6001);
    std::transform(indexInfo.articles.begin(), indexInfo.articles.end(), indexInfo.articles.begin(),
                   [crawler](ArticleInfo &info) -> ArticleInfo {
                       crawler->ParseArticle(info);
                       return info;
                   });

    //Merge IP Info
    std::map<std::string, std::set<int>> ipAddrMap;
    std::map<int, std::set<std::string>> ipSharedMap;
    std::for_each(indexInfo.articles.begin(), indexInfo.articles.end(),
                  [&ipAddrMap, &ipSharedMap](const ArticleInfo &ainfo) -> void {
                      std::cout << "Parsing info : " << ainfo << std::endl;
                      std::for_each(ainfo.ipUserInfoMap.begin(), ainfo.ipUserInfoMap.end(), [&ipAddrMap, &ipSharedMap](
                              std::pair<std::string, std::set<int>> nameIpSetPair) -> void {
                          std::string name = nameIpSetPair.first;
                          std::set<int> &fromIpSet = nameIpSetPair.second;
                          //std::cout << "Adding ip info set : " << fromIpSet.size() << " and ipAddrMap size is " << ipAddrMap.size() << std::endl;
                          std::set<int> &toIpSet = ipAddrMap[name];
                          toIpSet.insert(fromIpSet.begin(), fromIpSet.end());

                          //Put ip into ip=>name map
                          std::for_each(fromIpSet.begin(), fromIpSet.end(), [&ipSharedMap, &name](int ip) -> void {
                              ipSharedMap[ip].insert(name);
                          });
                      });
                  });

    std::for_each(ipAddrMap.begin(), ipAddrMap.end(),
                  [](const std::pair<std::string, std::set<int>> &nameIpSetPair) -> void {
                      if (nameIpSetPair.second.size() <= 1) return;
                      std::cout << nameIpSetPair.first << " : ";
                      std::for_each(nameIpSetPair.second.begin(), nameIpSetPair.second.end(), [](int ip) -> void {
                          std::cout << integerToIp4String(ip) << " ";
                      });
                      std::cout << std::endl;
                  });

    std::for_each(ipSharedMap.begin(), ipSharedMap.end(),
                  [](const std::pair<int, std::set<std::string>> &ipNameSetPair) -> void {
                      if (ipNameSetPair.second.size() <= 1) return;
                      std::cout << integerToIp4String(ipNameSetPair.first) << " : ";
                      std::for_each(ipNameSetPair.second.begin(), ipNameSetPair.second.end(),
                                    [](const std::string &name) -> void {
                                        std::cout << name;
                                    });
                      std::cout << std::endl;
                  });

    delete crawler;
}

