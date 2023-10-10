//
// Created by dxy on 23-10-9.
//

#include "build_rank_struct.h"

namespace wildcard {
    void BuildRankStruct::createIdxArray(const std::string& path) {
        std::ifstream file(path); // 打开文件

        if (file.is_open()) { // 检查文件是否成功打开
            std::string line;
            uindex index = 0;

            while (std::getline(file, line)) { // 逐行读取文件内容
                idxArray.push_back(index + line.length()); // 存储换行符所在的下标
                index += line.length() + 1; // 更新下标，考虑换行符
            }

            file.close(); // 关闭文件
        } else {
            std::cout << "build_rank_struct::无法打开文件" << std::endl;
        }
    }

    void BuildRankStruct::buildSparseBitmap(const std::string& path) {
        std::string bmPath=path+".bm";
        if(fs::exists(bmPath)) {
            bitmap = SparseBitMap::load_from_file(bmPath);
        }
        else{
            createIdxArray(path);
            bitmap = SparseBitMap(idxArray.back()+257, idxArray);
            bitmap.save(bmPath);
        }
    }

    usize BuildRankStruct::rank1(uindex pos) {
        return bitmap.rank1(pos);
    }

    usize BuildRankStruct::select1(uindex oneNum) {
        return bitmap.select1(oneNum);
    }

} // wildcard