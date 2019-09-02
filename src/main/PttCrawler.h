//
// Created by Rayer on 2019-08-15.
//

#ifndef ANALYZER_PTTCRAWLER_H
#define ANALYZER_PTTCRAWLER_H

#include <string>
#include <vector>
#include <ostream>
#include <map>
#include <set>
#include <list>
#include <chrono>
#include "ArticleInfo.h"
#include "IndexInfo.h"

class PttCrawler {
    std::string broadName;
    static std::string GetRawHtml(const std::string &url);

    int threadpool_size = 10;
public:
    explicit PttCrawler(const std::string &broadName);
    int GetMaxIndex();
    IndexInfo GetArticleInIndex(int index);
    ArticleInfo ParseArticle(ArticleInfo &articleInfo);

    virtual ~PttCrawler();
};

#endif //ANALYZER_PTTCRAWLER_H
