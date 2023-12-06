//
// Created by dxy on 23-12-4.
//

#ifndef CSA_WILDCARD_FUZZY_SEARCHER_H
#define CSA_WILDCARD_FUZZY_SEARCHER_H

#include<vector>
#include "gecsa/int_types.hpp"
#include "gecsa/gecsa.hpp"
#include <string>
#include<iostream>

using namespace gecsa;

namespace wildcard {

    class FuzzySearcher {
    private:
        std::vector<std::pair<uindex,uindex> > F;
        GeCSA *csa;
        std::string pattern;
        const int NotMatch = -1;

        //二分法找到满足跳k次后仍在第二个区间内的最小的L和最大的R
        uindex findSmallestL(std::pair<uindex, uindex> a, std::pair<uindex, uindex> b ,int k);
        uindex findLargestR(std::pair<uindex, uindex> a, std::pair<uindex, uindex> b ,int k);

        //返回给定的sa值跳k次后的值
        uindex phiHop(uindex saNum, int k);

        //输入模式串p1和p2的SA区间a和b，尝试返回p1p2的SA区间，合并失败则返回NotMatch
        std::pair<uindex,uindex> mergeSA(std::pair<uindex,uindex> a,std::pair<uindex,uindex> b ,const std::string& p1 ,const std::string& p2);

        //初始化F数组
        void initF();

        //位置i删除
        void deletion(int i);

        //位置i替换
        void replacement(int i);

        //位置i插入
        void insertion(int i);

        //转换一下，因为suffix_range返回的是左闭右开的区间，而我们需要的是左闭右闭的区间
        std::pair<uindex,uindex> suffixRange(std::string p){
            auto rst = csa->suffix_range(p);
            if(rst.first!=NotMatch && rst.second!=NotMatch){
                rst.second--;
                return rst;
            }
            else{
                return std::pair<uindex,uindex>(NotMatch,NotMatch);
            }
        }

    public:
        FuzzySearcher(GeCSA &csa){
            this->csa = &csa;
        }
        ~FuzzySearcher() = default;

        //更改模式串
        void set_pattern(std::string p);

        //k是允许的错误数,默认为1,模式串直接在类里面取，需要修改模式串用
        void fuzzySearch(int k=1);

        //递归深度搜索的模糊搜索
        void depthFuzzySearch(int k,int i,std::string p1, std::pair<uindex, uindex> a);

        void testForFuzzySearch();
        void testForDeletion(int i);
        void testForReplacement(int i);
        void testForInsertion(int i);
        void testForMergeSA();
        void testForDepthFuzzySearch(int k, int i, std::string p1,std::pair<uindex, uindex> a);

        //只提取一个sa值对应位置的串并输出
        void testExtract(uindex saNum,int len){
            auto idx = csa->suffix_range_to_indices(std::pair<uindex,uindex>(saNum,saNum+1));
            auto str = csa->extract(idx[0],len);
            for(auto c:str){
                std::cout<<static_cast<char>(c);
            }
            std::cout<<std::endl;
        }

    };

} // wildcard

#endif //CSA_WILDCARD_FUZZY_SEARCHER_H
