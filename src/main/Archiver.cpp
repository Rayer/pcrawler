//
// Created by Rayer Tung on 2019-09-03.
//

#include "Archiver.h"
#include <chrono>
#include <fstream>

void ArchiveService::ArchiveFile(const std::string &filename, const std::list<IndexInfo> &infoList) {
    ArchivedInfo archivedInfo;
    archivedInfo.content = infoList;
    archivedInfo.timestamp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    archivedInfo.filename = filename;

    std::ofstream ofs{filename};
    boost::archive::text_oarchive oar{ofs};
    oar & archivedInfo;
    ofs.close();

}

//optimize with std::move
ArchivedInfo ArchiveService::RestoreFromFile(const std::string &filename) {
    ArchivedInfo archivedInfo;
    std::ifstream ifs{filename};
    boost::archive::text_iarchive iar{ifs};
    iar & archivedInfo;
    ifs.close();
    return archivedInfo;
}
