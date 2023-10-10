//
// Created by dxy on 23-10-7.
//

#ifndef CSA_WILDCARD_WILDCARD_SEARCHER_H
#define CSA_WILDCARD_WILDCARD_SEARCHER_H

#include "gecsa/int_types.hpp"
#include "gecsa/gecsa.hpp"
#include <fstream>
#include <sstream>
#include "gecsa/bitmap.hpp"
#include "build_rank_struct.h"

using namespace gecsa;

namespace wildcard {

    class WildcardSearcher {
    private:
        GeCSA *csa;
        BuildRankStruct builder;

        //工具函数，输入一个字符串str，以及分隔符delimiter，根据分隔符将原
        //字符串分割并返回到一个vector容器中，当中的每一个子串仍为string类型
        std::vector<std::string> splitString(std::string str, char delimiter);
    public:
        WildcardSearcher() = default;
        ~WildcardSearcher() = default;

        WildcardSearcher(GeCSA &csa,const std::string& path){
            this->csa = &csa;
            builder.buildSparseBitmap(path);
        }
        // 输入模式串，最终会返回匹配模式串在原串中的起始位置到rst中
        bool star_search(std::string pattern,double& runtime);

        //从文件中读取测试语句并逐条查询，最后计算平均时间
        void testForSsearch();

        //单次测试
        void testForWildcard();

    };

} // wildcard

#endif //CSA_WILDCARD_WILDCARD_SEARCHER_H
