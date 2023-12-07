#include <algorithm>
#include <chrono>
#include <deque>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <regex>

#include "gecsa/gecsa.hpp"
#include "gecsa/sparse_bitmap.hpp"

#include "wildcard_executor.hpp"
#include "document_searcher.hpp"
#include "wildcard_pattern.hpp"
#include "utils.hpp"
#include "timer.hpp"
#include "build_rank_struct.h"

#include "wildcard_searcher.h"
#include "fuzzy_searcher.h"

using namespace gecsa;
using namespace wildcard;
using namespace std::string_view_literals;
namespace fs = std::filesystem;

i32 main(i32 argc, const char *argv[]) {
    std::string folder = "/home/dxy/Documents/testdata/";
    std::string filename;
    std::cout<<"输入要构建的文件名："<<std::endl;
    std::cin>> filename;
    std::string path = folder + filename;
    GeCSA csa;

    std::string archive_filename = path + ".csa"; //创建的压缩文件最后会变成.csa的文件
    //如果没构建csa就重新构建并保存，如果已经构建了就直接读取
    if (fs::exists(archive_filename)) {
        csa = GeCSA::load_from_file(archive_filename);
    } else {
        csa = GeCSA::create_from_file(path); //默认是256 256
        csa.save(archive_filename);
    }


    FuzzySearcher fuzzySearcher(csa);

//    std::vector<std::string> patterns={"amdxe","search","Recycle","Software","Installer","Management","unsupported","localization"};
//
//    for(std::string p:patterns){
//        fuzzySearcher.timeTestForDepthFuzzySearch(3,1,p,"normal");
//    }

    std::vector<std::string> DNAPatterns={"CCTCA","TAAGCC","TCGCTTC","ACCAATAG","AGCGGTACC","CCTGGTCTTT","ATGTGATTTAT","GTGCTGGGATTA"};

    for(std::string p:DNAPatterns){
        fuzzySearcher.timeTestForDepthFuzzySearch(4,2,p,"dna");
    }

}