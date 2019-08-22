//
// Created by Rayer on 2019-08-15.
//

#include "PttCrawler.h"
#include <curl/curl.h>
#include <sstream>
#include <gq/Document.h>
#include <gq/Node.h>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>


std::string Crawler::GetRawHtml(const std::string &url) {

    CURL* curl = curl_easy_init();
    curl_slist *headers = nullptr;
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_URL,url.c_str());

    std::string resultBody{};
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resultBody);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, static_cast<size_t (__stdcall *)(char *, size_t, size_t, void *)>(
            [](char *ptr, size_t size, size_t nmemb, void *resultBody) {
                *(static_cast<std::string *>(resultBody)) += std::string{ptr, size * nmemb};
                return size * nmemb;
            }
    ));

    curl_easy_setopt(curl, CURLOPT_COOKIE, "over18=1");
    CURLcode res = curl_easy_perform(curl);
    //std::fprintf(fp, "Return code : %d", res);
    curl_easy_cleanup(curl);
    //return std::string(buf);
    return resultBody;
}

Crawler::Crawler(const std::string &broadName) {
    this->broadName = broadName;
    curl_global_init(CURL_GLOBAL_ALL);
}

Crawler::~Crawler() {
    curl_global_cleanup();
}


int Crawler::GetMaxIndex() {
    std::stringstream url;
    url << "https://www.ptt.cc/bbs/" << this->broadName << "/index.html";
    std::string page_context = this->GetRawHtml(url.str());
    CDocument doc;
    doc.parse(page_context);
    CSelection c = doc.find("div.btn-group.btn-group-paging").find("a");

    std::string link = c.nodeAt(1).attribute("href");
    std::vector<std::string> splitted;
    boost::algorithm::split(splitted, link, boost::is_any_of("/"));

    std::string token = splitted[3];
    boost::erase_all(token, "index");
    boost::erase_all(token, ".html");
    return boost::lexical_cast<int>(token) + 1;
}

IndexInfo Crawler::GetArticleInIndex(int index) {
    //std::cout << "Starting for index " << index << std::endl;
    std::string url = (boost::format("https://www.ptt.cc/bbs/%1%/index%2%.html") % this->broadName % index).str();
    std::string content = this->GetRawHtml(url);
    IndexInfo ret;
    ret.index = index;
    std::vector<ArticleInfo> articleList;
    CDocument doc;
    doc.parse(content);
    CSelection c = doc.find("div.r-ent");
    for(auto i = 0; i < c.nodeNum(); ++i) {
        ArticleInfo info;
        CNode articleNode = c.nodeAt(i);
        //處理「本文已被刪除」
        CNode titleNode = articleNode.find(".title").nodeAt(0);
        if(titleNode.find("a").nodeNum() != 0) {
            info.title = articleNode.find(".title").nodeAt(0).find("a").nodeAt(0).text();
        } else {
            continue;
        }
        info.url = "https://www.ptt.cc" + articleNode.find("a").nodeAt(0).attribute("href");
        info.author = articleNode.find(".meta").nodeAt(0).find(".author").nodeAt(0).text();
        info.index = index;
        articleList.push_back(info);
    }
    ret.articles = articleList;
    return ret;
}

void Crawler::ParseArticle(ArticleInfo &articleInfo) {
    std::string content = this->GetRawHtml(articleInfo.url);
    CDocument doc;
    doc.parse(content);
    //Parse push
    //要處理 </span></div><div class="push center warning-box">檔案過大！部分文章無法顯示</div>
    CSelection pushes = doc.find("div.push:not(.warning-box)");
    for (int j = 0; j < pushes.nodeNum(); ++j) {
        CNode pushNode = pushes.nodeAt(j);
        //std::cout << pushNode.text() << std::endl;
        std::string tag = pushNode.find("span.push-tag").nodeAt(0).text();
        std::string name = pushNode.find("span.push-userid").nodeAt(0).text();
        if(tag == "推 ") {
            articleInfo.pusherMap[name] += 1;
            articleInfo.parsedPlusScore += 1;
            articleInfo.parsedArticleScore += 1;
        }

        if(tag == "噓 ") {
            articleInfo.haterMap[name] += 1;
            articleInfo.parsedNegativeScore += 1;
            articleInfo.parsedArticleScore -= 1;
        }

        if(tag == "→ ") {
            //do nothing
        }
    }
}

std::ostream &operator<<(std::ostream &os, const IndexInfo &info) {
    os << "index: " << info.index << std::endl;
    for (int j = 0; j < info.articles.size(); ++j) {
        os << info.articles[j] << std::endl;
    }
    return os;
}

std::ostream &operator<<(std::ostream &os, const ArticleInfo &info) {
    os << "index: " << info.index << " title: " << info.title << " author: " << info.author << " date: " << info.date
       << " url: " << info.url << " pusherMapSize: " << info.pusherMap.size() << " haterMapSize: " << info.haterMap.size()
       << " parsedPlusScore: " << info.parsedPlusScore << " parsedNegativeScore: " << info.parsedNegativeScore
       << " parsedArticleScore: " << info.parsedArticleScore << " shownArticleScore: " << info.shownArticleScore;
    return os;
}
