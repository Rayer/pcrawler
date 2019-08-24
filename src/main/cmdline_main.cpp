//
// Created by Rayer on 2019-08-23.
//
#include <boost/program_options.hpp>
#include <iostream>
#include <fstream>
#include "PttCrawlerTask.h"

namespace bpo = boost::program_options;

int main(int argc, char *argv[]) {

    bpo::options_description desc(R"(PTT Board analyzer. Example usage : -b gossiping -o report.txt -p 10)");
    desc.add_options()
            ("output,o", bpo::value<std::string>()->default_value(""), "Output File.")
            ("force_console", bpo::value<bool>()->default_value(false), "Force output to console")
            ("board,b", bpo::value<std::string>()->required(), "Board name")
            ("pages,p", bpo::value<int>()->required(), "Parse page n from most recent")
            ("ip_count_threshold", bpo::value<int>()->default_value(6), "User with different IP threshold.")
            ("same_ip_name_threshold", bpo::value<int>()->default_value(4), "Same IP with user threshold.");

    bpo::variables_map opts;
    bpo::store(bpo::parse_command_line(argc, argv, desc), opts);

    try {
        bpo::notify(opts);
    } catch (std::exception &e) {
        std::cout << e.what() << std::endl;
        std::cout << desc << std::endl;
    }

    //std::string boardName = opts["board"].as<std::string>();
    std::string output_file = opts["output"].as<std::string>();
    int pages = opts["pages"].as<int>();
    int ipCountThreshold = opts["ip_count_threshold"].as<int>();
    int ipWithNameThreshold = opts["same_ip_name_threshold"].as<int>();

    PttCrawlerTask task(opts["board"].as<std::string>());
    std::cout << "Start download and parsing..." << std::endl;
    task.startCrawl_recent(pages);
    std::cout << "Complete download and parsing." << std::endl;

    if (opts.count("output") > 0) {
        std::ofstream fs(opts["output"].as<std::string>());
        task.generateReport(ipCountThreshold, ipWithNameThreshold, fs);
    } else {
        task.generateReport(ipCountThreshold, ipWithNameThreshold, std::cout);
    }
}