//
// Created by Rayer Tung on 2019-09-03.
//

#ifndef ANALYZER_ARCHIVER_H
#define ANALYZER_ARCHIVER_H

#include <string>
#include "IndexInfo.h"
#include <ctime>

struct ArchivedInfo {
    std::string filename;
    time_t timestamp;
    std::list<IndexInfo> content;
};


#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/binary_object.hpp>
#include "IndexInfo.h"

template<typename Archive>
void serialize(Archive &ar, IndexInfo &index, const unsigned int version) {
    ar & index.index;
    ar & index.articles;
}

template<typename Archive>
void serialize(Archive &ar, ArticleInfo &article, const unsigned int version) {
    ar & article.index;
    ar & article.title;
    ar & article.author;
    ar & article.url;
    ar & article.pusherMap;
    ar & article.haterMap;
    ar & article.commitMap;
    ar & boost::serialization::make_binary_object(&article.parsedTime, sizeof(article.parsedTime));
    ar & article.parsedPlusScore;
    ar & article.parsedNegativeScore;
    ar & article.parsedArticleScore;
    ar & article.ipUserInfoMap;

}

template<typename Archive>
void serialize(Archive &ar, ArchivedInfo &archiveFile, const unsigned int version) {
    ar & archiveFile.filename;
    ar & archiveFile.timestamp;
    ar & archiveFile.content;
}

class ArchiveService {
public:
    void ArchiveFile(const std::string &filename, const std::list<IndexInfo> &infoList);

    ArchivedInfo RestoreFromFile(const std::string &filename);
};


#endif //ANALYZER_ARCHIVER_H
