//
// Created by Rayer on 2019/8/31.
//

#include "IndexInfo.h"
#include <iostream>
#include <algorithm>

std::ostream &operator<<(std::ostream &os, const IndexInfo &info) {
    os << "index: " << info.index << std::endl;

    std::for_each(info.articles.begin(), info.articles.end(), [&os](const ArticleInfo &a_info) -> void {
        os << a_info << std::endl;
    });
    return os;
}
