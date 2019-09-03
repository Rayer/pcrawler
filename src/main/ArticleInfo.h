//
// Created by Rayer on 2019/8/31.
//

#ifndef ANALYZER_ARTICLEINFO_H
#define ANALYZER_ARTICLEINFO_H

#include <string>
#include <chrono>
#include <map>
#include <list>
#include <set>
#include <boost/serialization/access.hpp>

struct ArticleInfo {
    int index;
    std::string title;
    std::string author;
    std::string url;

    std::map<std::string, int> pusherMap;
    std::map<std::string, int> haterMap;
    std::map<std::string, std::list<std::string>> commitMap;

    std::chrono::system_clock::time_point parsedTime;

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

    friend class boost::serialization::access;

};

std::ostream &operator<<(std::ostream &os, const ArticleInfo &info);

#endif //ANALYZER_ARTICLEINFO_H
