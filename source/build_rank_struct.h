//
// Created by dxy on 23-10-9.
//

#ifndef CSA_WILDCARD_BUILD_RANK_STRUCT_H
#define CSA_WILDCARD_BUILD_RANK_STRUCT_H

#include "gecsa/sparse_bitmap.hpp"
#include "gecsa/int_types.hpp"
#include <filesystem>
#include <vector>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;
using namespace gecsa;

namespace wildcard {

    class BuildRankStruct {
    public:
        BuildRankStruct()=default;
        ~BuildRankStruct()=default;

        //从原文件建立bitmap
        void buildSparseBitmap(const std::string& path);
        usize rank1(uindex pos);
        usize select1(uindex oneNum); //这个select是从1开始的，所以oneNum应该-1
    private:
        std::vector<uindex> idxArray;
        SparseBitMap bitmap;

        //输入原文件的路径，根据原文件得到分割符的位置并写入idxArray
        void createIdxArray(const std::string& path);
    };

} // wildcard

#endif //CSA_WILDCARD_BUILD_RANK_STRUCT_H
