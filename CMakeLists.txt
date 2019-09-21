cmake_minimum_required(VERSION 3.10)
project(Analyzer)

set(CMAKE_CXX_STANDARD 17)

if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup(TARGETS)
else()
    message(WARNING "The file conanbuildinfo.cmake doesn't exist, you have to run conan install first")
endif()

include_directories(src/main)
set(LIBS src/main/PttCrawler.cpp src/main/PttCrawler.h src/main/Utilities.cpp src/main/Utilities.h src/main/Utilities.cpp src/main/Utilities.h src/main/IpAnalyzer.cpp src/main/IpAnalyzer.h src/main/PttCrawlerTask.cpp src/main/PttCrawlerTask.h src/main/ArticleInfo.cpp src/main/ArticleInfo.h src/main/IndexInfo.cpp src/main/IndexInfo.h src/main/ThreadPool.h src/main/Archiver.cpp src/main/Archiver.h)

FIND_PACKAGE(Boost REQUIRED serialization filesystem system thread program_options)

include_directories(/usr/local/include/gq ${Boost_INCLUDE_DIRS})
link_directories(/usr/local/lib lib)

add_library(AnalyzerLibs ${LIBS})
add_executable(Analyzer src/main/cmdline_main.cpp)
add_executable(AnalyzerTest src/test/ThreadTest.cpp src/test/CrawlerTest.cpp src/test/UtilitiesTest.cpp)
target_link_libraries(AnalyzerLibs CONAN_PKG::libcurl boost_system boost_program_options boost_serialization gumbo gq)
target_link_libraries(Analyzer AnalyzerLibs)
target_link_libraries(AnalyzerTest AnalyzerLibs CONAN_PKG::libcurl CONAN_PKG::gtest)

enable_testing()
add_test(Crawler bin/AnalyzerTest --gtest_filter=Crawler_Test.*)
add_test(CrawlerTask bin/AnalyzerTest --gtest_filter=CrawlerTask_Test.*)
add_test(Serialization bin/AnalyzerTest --gtest_filter=Serialization.*)
add_test(Thread bin/AnalyzerTest --gtest_filter=ThreadTest.*)
add_test(CmdLine bin/Analyzer -b Gossiping -p 5)

