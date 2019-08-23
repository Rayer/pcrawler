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

struct ArticleInfo {
    int index;
    std::string title;
    std::string author;
    std::string date;
    std::string url;

    std::map<std::string, int> pusherMap;
    std::map<std::string, int> haterMap;

    int parsedPlusScore = 0;
    int parsedNegativeScore = 0;
    //int parsedNeutralScore;

    //Parse from article
    int parsedArticleScore = 0;
    //Show on Index
    std::string shownArticleScore;

    //userid / ip
    std::map<std::string, std::set<int>> ipUserInfoMap;

    friend std::ostream &operator<<(std::ostream &os, const ArticleInfo &info);
};

struct IndexInfo {
    int index;
    std::vector<ArticleInfo> articles;

    friend std::ostream &operator<<(std::ostream &os, const IndexInfo &info);
};

class Crawler {
    std::string broadName;
    static std::string GetRawHtml(const std::string &url);
public:
    explicit Crawler(const std::string &broadName);
    int GetMaxIndex();
    IndexInfo GetArticleInIndex(int index);
    void ParseArticle(ArticleInfo& ainfo);

    virtual ~Crawler();
};

#endif //ANALYZER_PTTCRAWLER_H
