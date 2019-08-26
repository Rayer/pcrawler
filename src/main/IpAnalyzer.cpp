//
// Created by Rayer Tung on 2019-08-23.
//

#include "IpAnalyzer.h"
#include "PttCrawler.h"
#include <iostream>
#include "Utilities.h"
#include <sstream>
#include <boost/format.hpp>
#include <iomanip>

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
        //std::cout << "Adding ip info set : " << fromIpSet.size() << " and idIpsMap size is " << idIpsMap.size() << std::endl;
        std::set<int> &toIpSet = idIpsMap[name];
        toIpSet.insert(fromIpSet.begin(), fromIpSet.end());

        //Put ip into ip=>name map
        std::for_each(fromIpSet.begin(), fromIpSet.end(), [this, &name](int ip) -> void {
            ipIdsMap[ip].insert(name);
        });
    });
}


IpAnalyzer::Result IpAnalyzer::analyze(int idWithMultiIpThreshold, int IpWithMultiIdThreshold) {

    IP_IDS_MAP filtered_IpIdsMap;
    ID_IPS_MAP filtered_IdIpsMap;
    HIGHLIGHT_USER_MAP highlightUserMap;

    std::for_each(idIpsMap.begin(), idIpsMap.end(),
                  [idWithMultiIpThreshold, &filtered_IdIpsMap, &highlightUserMap](
                          const std::pair<std::string, std::set<int>> &nameIpSetPair) -> void {
                      if (nameIpSetPair.second.size() < idWithMultiIpThreshold) return;
                      filtered_IdIpsMap.insert(nameIpSetPair);
                      std::ostringstream buf;
                      buf << nameIpSetPair.first << " (" << nameIpSetPair.second.size() << ") : ";
                      std::for_each(nameIpSetPair.second.begin(), nameIpSetPair.second.end(), [&buf](int ip) -> void {
                          buf << integerToIp4String(ip) << " ";
                      });
                      buf << std::endl;
                      std::string reason = buf.str();
                      highlightUserMap[nameIpSetPair.first].push_back(reason);
                  });

    std::for_each(ipIdsMap.begin(), ipIdsMap.end(),
                  [IpWithMultiIdThreshold, &filtered_IpIdsMap, &highlightUserMap](
                          const std::pair<int, std::set<std::string>> &ipNameSetPair) -> void {
                      if (ipNameSetPair.second.size() < IpWithMultiIdThreshold) return;
                      filtered_IpIdsMap.insert(ipNameSetPair);
                      std::ostringstream buf;
                      buf << integerToIp4String(ipNameSetPair.first) << " (" << ipNameSetPair.second.size() << ") : ";
                      std::for_each(ipNameSetPair.second.begin(), ipNameSetPair.second.end(),
                                    [&buf](const std::string &name) -> void {
                                        buf << name << " ";
                                    });
                      buf << std::endl;
                      std::string reason = buf.str();
                      std::for_each(ipNameSetPair.second.begin(), ipNameSetPair.second.end(),
                                    [&reason, &highlightUserMap](const std::string &name) -> void {
                                        highlightUserMap[name].push_back(reason);
                                    });
                  });

    return Result{filtered_IdIpsMap, filtered_IpIdsMap, highlightUserMap, idWithMultiIpThreshold,
                  IpWithMultiIdThreshold};
}

void IpAnalyzer::printReport(std::ostream &os, const IpAnalyzer::Result &result) {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    os << "Report generated at : " << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X") << std::endl;
    os << "User have used " << result.idWithMultiIpThreshold << " and more IPs : " << std::endl;
    std::for_each(result.idIpsMap.begin(), result.idIpsMap.end(),
                  [&os](const std::pair<std::string, std::set<int>> &nameIpSetPair) -> void {
                      std::ostringstream buf;
                      buf << nameIpSetPair.first << " (" << nameIpSetPair.second.size() << ") : ";
                      std::for_each(nameIpSetPair.second.begin(), nameIpSetPair.second.end(), [&buf](int ip) -> void {
                          buf << integerToIp4String(ip) << " ";
                      });
                      buf << std::endl;
                      std::string reason = buf.str();
                      os << reason;
                  });

    os << std::endl << "IPs with more then " << result.ipWithMultiIdThreshold << " users :" << std::endl;
    //Print IP with multiple IDs
    std::for_each(result.ipIdsMap.begin(), result.ipIdsMap.end(),
                  [&os](const std::pair<int, std::set<std::string>> &ipNameSetPair) -> void {
                      os << integerToIp4String(ipNameSetPair.first) << " (" << ipNameSetPair.second.size() << ") : ";
                      std::for_each(ipNameSetPair.second.begin(), ipNameSetPair.second.end(),
                                    [&os](const std::string &name) -> void {
                                        os << name << " ";
                                    });
                      os << std::endl;
                  });

    //Highlight user maps
    os << std::endl << "Highlighted user's commit, who have appeared in above lists : " << std::endl;
    std::for_each(result.highlightUserMap.begin(), result.highlightUserMap.end(),
                  [&os, this](const std::pair<std::string, std::list<std::string>> &nameReasonPair) -> void {
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





