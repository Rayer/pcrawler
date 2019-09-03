//
// Created by Rayer on 2019-08-23.
//
#include <boost/program_options.hpp>
#include <iostream>
#include <fstream>
#include "PttCrawlerTask.h"
#include <cmath>
#include <iomanip>
#include "Utilities.h"
#include <boost/format.hpp>
#include <Archiver.h>

namespace bpo = boost::program_options;

class Callback : public PttCrawlerTaskCallback {
private:
    int article_ignore_age_threshold;

private:
    bool verbose;
    int parsed_index;
    int parsed_document;
    std::list<IndexInfo> parsed_indexinfo;

public:
    [[nodiscard]] int getParsedIndex() const {
        return parsed_index;
    }

    [[nodiscard]] int getParsedDocument() const {
        return parsed_document;
    }

    void setArticleIgnoreAgeThreshold(int articleIgnoreAgeThreshold) {
        article_ignore_age_threshold = articleIgnoreAgeThreshold;
    }

    [[nodiscard]] std::list<IndexInfo> getParsedIndexinfo() const {
        return parsed_indexinfo;
    }

    explicit Callback(int ignore_older_hour_article, bool verbose) {
        article_ignore_age_threshold = ignore_older_hour_article;
        this->verbose = verbose;
    }
    void processingIndex(int from, int to, int current) override {
        float done_block = (float(std::abs(current - from)) / float(std::abs(from - to))) / 0.02f; //2% per block
        std::string progressBar{"[--------------------------------------------------]"};
        std::fill_n(progressBar.begin() + 1, (int) done_block, '#');
        std::cout << "\r" << progressBar << done_block * 2 << "% Fetching index URL : "
                  << "https://www.ptt.cc/bbs/Gossiping/index" << current << ".html";
        if (to == current) {
            std::cout << std::endl;
        }
        std::cout << std::flush;
        parsed_index = from - to + 1;
    }

    bool shouldIncludeInReport(const ArticleInfo &articleInfo) override {
        if (article_ignore_age_threshold == 0) {
            return true;
        }

        if (std::chrono::system_clock::now() - articleInfo.parsedTime >
            std::chrono::hours(article_ignore_age_threshold)) {
            std::cout << "Dropped article " << articleInfo.title << "(" << articleInfo.url
                      << ") due to it's " << article_ignore_age_threshold << " hours ago" << std::endl;
            std::cout << articleInfo << std::endl;
            return false;
        }
        return true;
    }

    void doneParseDocument(const ArticleInfo &articleInfo, int current, int total) override {

        if (verbose) {
            std::cout << "Parsed document (" << current << "/" << total << ") :" << articleInfo << std::endl
                      << std::flush;
        } else {
            float done_block = (float(current) / float(total)) / 0.02f; //5% per block
            std::string progressBar{"[--------------------------------------------------]"};
            std::fill_n(progressBar.begin() + 1, (int) done_block, '#');
            std::cout << "\r" << progressBar << done_block * 2 << "% " << articleInfo.title;
            if (total == current) {
                std::cout << std::endl;
            }
            std::cout << std::flush;
        }
        parsed_document = total;
    }

    void analyzeFinished(const IpAnalyzer::ID_IPS_MAP &idAddrMap, const IpAnalyzer::IP_IDS_MAP &ipSharedMap,
                         const IpAnalyzer::HIGHLIGHT_USER_MAP &highlightUserMap) override {
        std::string output = boost::str(boost::format(
                "Analyze completed!\nID with multiple ID : %1%\nIP shared by multiple ID : %2%\nHighlighted user : %3%") %
                                        idAddrMap.size() % ipSharedMap.size() % highlightUserMap.size());
        std::cout << output << std::endl;
        std::cout.flush();

    }

    void doneParseAllDocument(const std::list<IndexInfo> &info) override {
        this->parsed_indexinfo = info;
    }
};

int main(int argc, char *argv[]) {

    bpo::options_description desc(R"(PTT Board analyzer. Example usage : -b gossiping -o report.txt -p 10)");
    desc.add_options()
            ("output,o", bpo::value<std::string>()->default_value(""), "Output File.")
            ("force_console", "Force output to console")
            ("board,b", bpo::value<std::string>()->required(), "Board name")
            ("pages,p", bpo::value<int>()->required(), "Parse page n from most recent")
            ("ip_count_threshold", bpo::value<int>()->default_value(6), "User with different IP threshold.")
            ("same_ip_name_threshold", bpo::value<int>()->default_value(4), "Same IP with user threshold.")
            ("ignore_old_post_hour", bpo::value<int>()->default_value(0),
             "Don't analyze article older then n hours. 0 = no drop")
            ("verbose,v", "Verbose mode, output parsed article statistics.")
            ("interactive,i", "Interactive mode while generating report after documents are parsed.")
            ("help,h", "Help")
            ("thread_count,t", bpo::value<int>()->default_value(15),
             "Thread count. If there are some curl error 6, consider lower this value");

    bpo::variables_map opts;
    bpo::store(bpo::parse_command_line(argc, argv, desc), opts);

    try {
        bpo::store(bpo::parse_command_line(argc, argv, desc), opts);
        bpo::notify(opts);
    } catch (std::exception &e) {
        std::cout << e.what() << std::endl;
        std::cout << desc << std::endl;
    }

    if (opts.count("help")) {
        std::cout << desc << std::endl;
        return 0;
    }

    //std::string boardName = opts["board"].as<std::string>();
    std::string output_file = opts["output"].as<std::string>();
    int pages = opts["pages"].as<int>();
    int ipCountThreshold = opts["ip_count_threshold"].as<int>();
    int ipWithNameThreshold = opts["same_ip_name_threshold"].as<int>();
    int article_ignore_age_threshold = opts["ignore_old_post_hour"].as<int>();
    int thread_count = opts["thread_count"].as<int>();

    auto *callback = new Callback(article_ignore_age_threshold, opts.count("verbose"));

    PttCrawlerTask task(opts["board"].as<std::string>(), callback);
    task.setThreadpoolSize(thread_count);
    std::cout << "Start download and parsing..." << std::endl;
    task.startCrawl_recent(pages);
    std::cout << "Complete download and parsing." << std::endl;

    if (opts.count("interactive")) {
        //Interactive mode
        std::string cmd;
        do {
            std::cout << "Parsed index : " << callback->getParsedIndex() << std::endl;
            std::cout << "Parsed documents : " << callback->getParsedDocument() << std::endl;
            std::cout << "(a) ID with multiple IP threshold : " << ipCountThreshold << std::endl;
            std::cout << "(b) IP with multiple ID threshold : " << ipWithNameThreshold << std::endl;
            std::cout << "(c) Drop document if age more then : " << article_ignore_age_threshold << std::endl;
            std::cout << "(o) Output file : " << output_file << std::endl;
            std::cout << "(s) Save snapshot" << std::endl;
            std::cout << "(g) Generate report and statistics " << std::endl;
            std::cout << "(e) Exit" << std::endl;
            std::cout << "Command : ";
            std::cin >> cmd;

            switch (str2int(cmd.c_str())) {
                case str2int("a"): {
                    std::cout << "New value [" << ipCountThreshold << "] : ";
                    int input;
                    std::cin >> input;
                    if (std::cin.fail()) {
                        std::cin.clear();
                    } else {
                        ipCountThreshold = input;
                    }
                    break;
                }
                case str2int("b"): {
                    std::cout << "New value [" << ipWithNameThreshold << "] : ";
                    int input;
                    std::cin >> input;
                    if (std::cin.fail()) {
                        std::cin.clear();
                    } else {
                        ipWithNameThreshold = input;
                    }
                    break;
                }
                case str2int("c"): {
                    std::cout << "New value [" << article_ignore_age_threshold << "] : ";
                    int input;
                    std::cin >> input;
                    if (std::cin.fail()) {
                        std::cin.clear();
                    } else {
                        article_ignore_age_threshold = input;
                        callback->setArticleIgnoreAgeThreshold(article_ignore_age_threshold);
                    }
                    break;
                }
                case str2int("o"): {
                    std::cout << "New value [" << output_file << "] : ";
                    std::string input;
                    std::cin >> input;
                    if (std::cin.fail()) {
                        std::cin.clear();
                    } else {
                        output_file = input;
                    }
                    break;
                }
                case str2int("g"): {
                    if (output_file.empty()) {
                        task.generateReport(ipCountThreshold, ipWithNameThreshold, std::cout);
                    } else {
                        std::ofstream fs(output_file);
                        task.generateReport(ipCountThreshold, ipWithNameThreshold, fs);
                        if (opts.count("force_console")) {
                            task.generateReport(ipCountThreshold, ipWithNameThreshold, std::cout);
                        }
                        break;
                    }
                }
                case str2int("s"): {
                    ArchiveService archiverService;
                    archiverService.ArchiveFile("snapshot.txt", callback->getParsedIndexinfo());
                }
                case str2int("e"):
                    break;
                default:
                    std::cout << "Invalid command!" << std::endl;
            }

        } while (cmd != "e");

    } else {
        if (opts.count("output") > 0) {
            std::ofstream fs(opts["output"].as<std::string>());
            task.generateReport(ipCountThreshold, ipWithNameThreshold, fs);
            if (opts.count("force_console")) {
                task.generateReport(ipCountThreshold, ipWithNameThreshold, std::cout);
            }
        } else {
            task.generateReport(ipCountThreshold, ipWithNameThreshold, std::cout);
        }
    }

}