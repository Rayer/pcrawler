//
// Created by Rayer on 2019/8/31.
//

#ifndef ANALYZER_INDEXINFO_H
#define ANALYZER_INDEXINFO_H

#include <boost/serialization/access.hpp>
#include "ArticleInfo.h"

struct IndexInfo {
    int index;
    std::list<ArticleInfo> articles;
};

std::ostream &operator<<(std::ostream &os, const IndexInfo &info);


#endif //ANALYZER_INDEXINFO_H
