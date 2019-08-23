cmake_minimum_required(VERSION 3.14)
project(Analyzer)

set(CMAKE_CXX_STANDARD 17)

if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup(TARGETS)
else()
    message(WARNING "The file conanbuildinfo.cmake doesn't exist, you have to run conan install first")
endif()

# SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -pthread")

include_directories(src/main)
set(LIBS src/main/PttCrawler.cpp src/main/PttCrawler.h src/main/Utilities.cpp src/main/Utilities.h)

FIND_PACKAGE(Boost REQUIRED serialization filesystem system thread)

include_directories(/usr/local/include/gq ${Boost_INCLUDE_DIR} )
link_directories(/usr/local/lib lib)

add_library(AnalyzerLibs ${LIBS})
add_executable(Analyzer src/main/WebServiceMain.cpp)
add_executable(AnalyzerTest src/test/ThreadTest.cpp src/test/CrawlerTest.cpp)
target_link_libraries(AnalyzerLibs CONAN_PKG::libcurl boost_system)
target_link_libraries(Analyzer AnalyzerLibs CONAN_PKG::Poco)
target_link_libraries(AnalyzerTest AnalyzerLibs CONAN_PKG::libcurl CONAN_PKG::gtest gumbo gq)

