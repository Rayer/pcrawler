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

struct ArticleInfo {
    int index;
    std::string title;
    std::string author;
    std::string date;
    std::string url;

    std::map<std::string, int> pusherMap;
    std::map<std::string, int> haterMap;
    std::map<std::string, std::list<std::string>> commitMap;

    int parsedPlusScore = 0;
    int parsedNegativeScore = 0;
    //int parsedNeutralScore;

    //Parse from article
    int parsedArticleScore = 0;
    //Show on Index
    std::string shownArticleScore;

    //userid / ip
    std::map<std::string, std::set<int>> ipUserInfoMap;

    //Will exclude if it is pinned article(置頂)
    bool willExclude = false;

    friend std::ostream &operator<<(std::ostream &os, const ArticleInfo &info);
};

struct IndexInfo {
    int index;
    std::list<ArticleInfo> articles;

    friend std::ostream &operator<<(std::ostream &os, const IndexInfo &info);
};

class PttCrawler {
    std::string broadName;
    static std::string GetRawHtml(const std::string &url);
public:
    explicit PttCrawler(const std::string &broadName);
    int GetMaxIndex();
    IndexInfo GetArticleInIndex(int index);

    ArticleInfo ParseArticle(ArticleInfo &articleInfo);

    virtual ~PttCrawler();
};

#endif //ANALYZER_PTTCRAWLER_H
