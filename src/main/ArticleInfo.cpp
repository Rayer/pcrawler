//
// Created by Rayer on 2019/8/31.
//

#include "ArticleInfo.h"
#include <iostream>
#include <iomanip>

std::ostream &operator<<(std::ostream &os, const ArticleInfo &info) {
    std::time_t time = std::chrono::system_clock::to_time_t(info.parsedTime);
    os << "index: " << info.index << " title: " << info.title << " author: " << info.author << " date: "
       << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S")
       << " url: " << info.url << " pusherMap: " << info.pusherMap.size() << " haterMap: " << info.haterMap.size()
       << " parsedPlusScore: " << info.parsedPlusScore << " parsedNegativeScore: " << info.parsedNegativeScore
       << " parsedArticleScore: " << info.parsedArticleScore << " shownArticleScore: " << info.shownArticleScore
       << " ipUserInfoMap: " << info.ipUserInfoMap.size();
    return os;
}
