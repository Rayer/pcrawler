//
// Created by Rayer Tung on 2019-08-23.
//

#include "IpAnalyzer.h"
#include "PttCrawler.h"
#include <iostream>
#include "Utilities.h"
#include <sstream>
#include <boost/format.hpp>

void IpAnalyzer::addParsedIndex(const IndexInfo &i_info) {
    std::for_each(i_info.articles.begin(), i_info.articles.end(),
                  [this](const ArticleInfo &ainfo) -> void {
                      this->addParsedDocument(ainfo);
                  });
}

void IpAnalyzer::addParsedDocument(const ArticleInfo &a_info) {
    articleInfoList.push_back(a_info);
    std::for_each(a_info.ipUserInfoMap.begin(), a_info.ipUserInfoMap.end(), [this](
            std::pair<std::string, std::set<int>> nameIpSetPair) -> void {
        std::string name = nameIpSetPair.first;
        std::set<int> &fromIpSet = nameIpSetPair.second;
        //std::cout << "Adding ip info set : " << fromIpSet.size() << " and ipAddrMap size is " << ipAddrMap.size() << std::endl;
        std::set<int> &toIpSet = ipAddrMap[name];
        toIpSet.insert(fromIpSet.begin(), fromIpSet.end());

        //Put ip into ip=>name map
        std::for_each(fromIpSet.begin(), fromIpSet.end(), [this, &name](int ip) -> void {
            ipSharedMap[ip].insert(name);
        });
    });
}

const IpAnalyzer::IP_ADDR_MAP &IpAnalyzer::getIpAddrMap() {
    return this->ipAddrMap;
}

const IpAnalyzer::IP_SHARED_MAP &IpAnalyzer::getIpSharedMap() {
    return this->ipSharedMap;
}

const IpAnalyzer::HIGHLIGHT_USER_MAP &IpAnalyzer::getHighlightUserMap() {
    return this->highlightMap;
}

void IpAnalyzer::printUserWithMultipleIp(std::ostream &os, int threshold) {
    std::for_each(ipAddrMap.begin(), ipAddrMap.end(),
                  [&os, threshold, this](const std::pair<std::string, std::set<int>> &nameIpSetPair) -> void {
                      if (nameIpSetPair.second.size() < threshold) return;
                      std::ostringstream buf;
                      buf << nameIpSetPair.first << " (" << nameIpSetPair.second.size() << ") : ";
                      std::for_each(nameIpSetPair.second.begin(), nameIpSetPair.second.end(), [&buf](int ip) -> void {
                          buf << integerToIp4String(ip) << " ";
                      });
                      buf << std::endl;
                      std::string reason = buf.str();
                      os << reason;
                      highlightMap[nameIpSetPair.first].push_back(reason);
                  });
}

void IpAnalyzer::printIpSharedByMultipleUser(std::ostream &os, int threshold) {
    std::for_each(ipSharedMap.begin(), ipSharedMap.end(),
                  [&os, threshold, this](const std::pair<int, std::set<std::string>> &ipNameSetPair) -> void {
                      if (ipNameSetPair.second.size() < threshold) return;
                      std::ostringstream buf;
                      buf << integerToIp4String(ipNameSetPair.first) << " (" << ipNameSetPair.second.size() << ") : ";
                      std::for_each(ipNameSetPair.second.begin(), ipNameSetPair.second.end(),
                                    [&buf](const std::string &name) -> void {
                                        buf << name << " ";
                                    });
                      buf << std::endl;
                      std::string reason = buf.str();
                      std::for_each(ipNameSetPair.second.begin(), ipNameSetPair.second.end(),
                                    [this, &reason](const std::string &name) -> void {
                                        highlightMap[name].push_back(reason);
                                    });
                      os << reason;
                  });
}

void IpAnalyzer::whatDoesTheFoxSay(std::ostream &os) {
    std::for_each(highlightMap.begin(), highlightMap.end(),
                  [this, &os](const std::pair<std::string, std::list<std::string>> &nameReasonPair) -> void {
                      os << "Name : " << nameReasonPair.first << std::endl;
                      std::for_each(nameReasonPair.second.begin(), nameReasonPair.second.end(),
                                    [&os](const std::string &reason) -> void {
                                        os << reason;
                                    });

                      std::for_each(articleInfoList.begin(), articleInfoList.end(),
                                    [&os, &nameReasonPair](ArticleInfo &aInfo) -> void {
                                        bool firstTime = true;
                                        std::for_each(aInfo.commitMap.begin(), aInfo.commitMap.end(),
                                                      [&os, &nameReasonPair, &firstTime, &aInfo](
                                                              const std::pair<std::string, std::list<std::string>> &nameCommitListPair) -> void {
                                                          if (nameCommitListPair.first == nameReasonPair.first) {
                                                              if (firstTime) {
                                                                  os << (boost::format("%1% (%2%)") % aInfo.title %
                                                                         aInfo.url).str() << std::endl;
                                                                  firstTime = false;
                                                              }
                                                              std::for_each(nameCommitListPair.second.begin(),
                                                                            nameCommitListPair.second.end(),
                                                                            [&os](const std::string &commit) -> void {
                                                                                os << commit << std::endl;
                                                                            });
                                                          }
                                                      });
                                    });
                  });

}





