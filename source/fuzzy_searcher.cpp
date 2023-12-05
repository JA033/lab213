//
// Created by dxy on 23-12-4.
//

#include "fuzzy_searcher.h"
#include <iostream>
#include <utility>

namespace wildcard {

    void FuzzySearcher::initF() {
        F.clear();
        int m= pattern.length();

        for(int i =0;i<m;i++){
            std::string tempP=pattern.substr(i);
            std::pair<uindex,uindex> tempR=suffixRange(tempP);
            F.push_back(tempR);
        }
        F.emplace_back(0,csa->size());
    }

    void FuzzySearcher::set_pattern(std::string p) {
        this->pattern = std::move(p);
        initF();
    }

    std::pair<uindex, uindex> FuzzySearcher::mergeSA(std::pair<uindex, uindex> a, std::pair<uindex, uindex> b, const std::string& p1 ,const std::string& p2) {
        int m1 = p1.length();
        int m2 = p2.length();

        uindex L=NotMatch;
        uindex R=NotMatch;

        L=findSmallestL(a,b,m1);
        R=findLargestR(a,b,m1);

        if(L==NotMatch||R==NotMatch){
            return std::pair<uindex, uindex>(NotMatch,NotMatch);
        }
        else{
            return std::pair<uindex, uindex>(L,R);
        }
    }

    uindex FuzzySearcher::findSmallestL(std::pair<uindex, uindex> a, std::pair<uindex, uindex> b ,int k) {
        uindex start=a.first;
        uindex end=a.second;
        uindex l=b.first;
        uindex r=b.second;
        uindex idx=NotMatch;

        while(start <=end){
            uindex mid = start+(end-start)/2;
            uindex midVal = phiHop(mid,k);

            if(midVal>=l && midVal<=r){
                idx=mid;
                end=mid-1;
            }
            else if(midVal<l){
                start=mid+1;
            }
            else if(midVal>r){
                end=mid-1;
            }
        }

        return idx;
    }

    uindex FuzzySearcher::findLargestR(std::pair<uindex, uindex> a, std::pair<uindex, uindex> b, int k) {
        uindex start = a.first;
        uindex end = a.second;
        uindex l = b.first;
        uindex r = b.second;
        uindex idx = NotMatch;

        while(start <=end){
            uindex mid = start+(end-start)/2;
            uindex midVal = phiHop(mid,k);

            if(midVal>=l && midVal<=r){
                idx=mid;
                start=mid+1;
            }
            else if(midVal<l){
                start=mid+1;
            }
            else if(midVal>r){
                end=mid-1;
            }
        }

        return idx;
    }

    uindex FuzzySearcher::phiHop(uindex saNum, int k) {
        for(int i=0;i<k;i++){
            saNum=csa->getPhi(saNum);
        }
        return saNum;
    }

    void FuzzySearcher::testForMergeSA() {
        std::string p1="servi";
        std::string p2="cing";
        std::pair<uindex,uindex> a=csa->suffix_range(p1);
        std::pair<uindex,uindex> b=csa->suffix_range(p2);
        std::pair<uindex,uindex> result1=mergeSA(a,b,p1,p2);

        std::pair<uindex,uindex> result2=csa->suffix_range(p1+p2);

        std::cout<<"result1: "<<result1.first<<" "<<result1.second<<std::endl;
        std::cout<<"result2: "<<result2.first<<" "<<result2.second<<std::endl;
   }

    void FuzzySearcher::fuzzySearch(int k) {
        int m = pattern.length();

        //检查原串是否能直接查到结果，即k=0的情况
        std::pair<uindex,uindex> result=suffixRange(pattern);
        if(result.first!=NotMatch && result.second!=NotMatch){
            std::cout<<"origin pattern match: "<<result.first<<" "<<result.second<<std::endl;
        }

        for(int i=0;i<m;i++){
            deletion(i);
            replacement(i);
            insertion(i);
        }
        insertion(m);
    }

    void FuzzySearcher::deletion(int i) {
        int m= pattern.length();

        //位置0删除直接返回删除后的串
        if(i==0){
            //std::string p2 = pattern.substr(1);
            std::pair<uindex,uindex> rst=F[1];
            if(rst.first!=NotMatch && rst.second!=NotMatch){
                std::cout<<"position "<<i<<" deletion match: "<<rst.first<<" "<<rst.second<<std::endl;
            }
        }
        //位置m-1删除直接返回删除后的串
        else if(i==m-1){
            std::string p1 = pattern.substr(0,m-1);
            std::pair<uindex,uindex> rst=suffixRange(p1);
            if(rst.first!=NotMatch && rst.second!=NotMatch){
                std::cout<<"position "<<i<<" deletion match: "<<rst.first<<" "<<rst.second<<std::endl;
            }
        }
        else{
            std::string p1 = pattern.substr(0,i);
            std::string p2 = pattern.substr(i+1);

            std::pair<uindex,uindex> a=suffixRange(p1);
            std::pair<uindex,uindex> b=F[i+1];
            if(a.first==NotMatch || b.first==NotMatch ){
                return;
            }
            std::pair<uindex,uindex> rst=mergeSA(a,b,p1,p2);
            if(rst.first!=NotMatch && rst.second!=NotMatch){
                std::cout<<"position "<<i<<" deletion match: "<<rst.first<<" "<<rst.second<<std::endl;
            }
        }
    }

    void FuzzySearcher::replacement(int i) {
        int m= pattern.length();

        //位置0替换直接返回替换后的串
        if(i==0){
            for(char c='a';c<='z';c++){
                std::string p2 = c+pattern.substr(1);
                std::pair<uindex,uindex> rst=suffixRange(p2);
                if(rst.first!=NotMatch && rst.second!=NotMatch){
                    std::cout<<"position "<<i<<" replacement match: "<<rst.first<<" "<<rst.second<<std::endl;
                }
            }
        }
        //位置m-1直接替换返回后的串
        else if(i==m-1){
            for(char c='a';c<='z';c++){
                std::string p1 = pattern.substr(0,m-1)+c;
                std::pair<uindex,uindex> rst=suffixRange(p1);
                if(rst.first!=NotMatch && rst.second!=NotMatch){
                    std::cout<<"position "<<i<<" replacement match: "<<rst.first<<" "<<rst.second<<std::endl;
                }
            }
        }
        else{
            for(char c='a';c<='z';c++){
                std::string p1 = pattern.substr(0,i)+c;
                std::string p2 = pattern.substr(i+1);

                std::pair<uindex,uindex> a=suffixRange(p1);
                std::pair<uindex,uindex> b=F[i+1];
                if(a.first==NotMatch || b.first==NotMatch ){
                    return;
                }
                std::pair<uindex,uindex> rst=mergeSA(a,b,p1,p2);
                if(rst.first!=NotMatch && rst.second!=NotMatch){
                    std::cout<<"position "<<i<<" replacement match: "<<rst.first<<" "<<rst.second<<std::endl;
                }
            }
        }
    }

    void FuzzySearcher::insertion(int i) {
        int m= pattern.length();

        //位置0插入直接返回插入后的串
        if(i==0){
            for(char c='a';c<='z';c++){
                std::string p2 = c+pattern;
                std::pair<uindex,uindex> rst=suffixRange(p2);
                if(rst.first!=NotMatch && rst.second!=NotMatch){
                    std::cout<<"position "<<i<<" insertion match: "<<rst.first<<" "<<rst.second<<std::endl;
                }
            }
        }
        //位置m插入直接返回插入后的串
        else if(i==m){
            for(char c='a';c<='z';c++){
                std::string p1 = pattern+c;
                std::pair<uindex,uindex> rst=suffixRange(p1);
                if(rst.first!=NotMatch && rst.second!=NotMatch){
                    std::cout<<"position "<<i<<" insertion match: "<<rst.first<<" "<<rst.second<<std::endl;
                }
            }
        }
        else{
            for(char c='a';c<='z';c++){
                std::string p1 = pattern.substr(0,i)+c;
                std::string p2 = pattern.substr(i);

                std::pair<uindex,uindex> a=suffixRange(p1);
                std::pair<uindex,uindex> b=F[i];
                if(a.first==NotMatch || b.first==NotMatch ){
                    return;
                }
                std::pair<uindex,uindex> rst=mergeSA(a,b,p1,p2);
                if(rst.first!=NotMatch && rst.second!=NotMatch){
                    std::cout<<"position "<<i<<" insertion match: "<<rst.first<<" "<<rst.second<<std::endl;
                }
            }
        }
    }

    void FuzzySearcher::testForFuzzySearch() {
        int m = pattern.length();

        //检查原串是否能直接查到结果，即k=0的情况
        std::pair<uindex,uindex> result=suffixRange(pattern);
        if(result.first!=NotMatch && result.second!=NotMatch){
            std::cout<<"origin pattern match: ";
            testExtract(result.first,m);
        }

        for(int i=0;i<m;i++){
            testForDeletion(i);
            testForReplacement(i);
            testForInsertion(i);
        }
        testForInsertion(m);
    }

    void FuzzySearcher::testForDeletion(int i) {
        int m= pattern.length();

        //位置0删除直接返回删除后的串
        if(i==0){
            //std::string p2 = pattern.substr(1);
            std::pair<uindex,uindex> rst=F[1];
            if(rst.first!=NotMatch && rst.second!=NotMatch){
                std::cout<<"position "<<i<<" deletion match: "<<std::endl;
                std::cout<<"suppose to be:"<<pattern.substr(1)<<std::endl;
                std::cout<<"real result:";
                testExtract(rst.first,m-1);
            }
        }
            //位置m-1删除直接返回删除后的串
        else if(i==m-1){
            std::string p1 = pattern.substr(0,m-1);
            std::pair<uindex,uindex> rst=suffixRange(p1);
            if(rst.first!=NotMatch && rst.second!=NotMatch){
                std::cout<<"position "<<i<<" deletion match: "<<std::endl;
                std::cout<<"suppose to be:"<<p1<<std::endl;
                std::cout<<"real result:";
                testExtract(rst.first,m-1);
            }
        }
        else{
            std::string p1 = pattern.substr(0,i);
            std::string p2 = pattern.substr(i+1);

            std::pair<uindex,uindex> a=suffixRange(p1);
            std::pair<uindex,uindex> b=F[i+1];
            if(a.first==NotMatch || b.first==NotMatch ){
                return;
            }
            std::pair<uindex,uindex> rst=mergeSA(a,b,p1,p2);
            if(rst.first!=NotMatch && rst.second!=NotMatch){
                std::cout<<"position "<<i<<" deletion match: "<<std::endl;
                std::cout<<"suppose to be:"<<p1+p2<<std::endl;
                std::cout<<"real result:";
                testExtract(rst.first,m-1);
            }
        }
    }

    void FuzzySearcher::testForReplacement(int i) {
        int m= pattern.length();

        //位置0替换直接返回替换后的串
        if(i==0){
            for(char c='a';c<='z';c++){
                std::string p2 = c+pattern.substr(1);
                std::pair<uindex,uindex> rst=suffixRange(p2);
                if(rst.first!=NotMatch && rst.second!=NotMatch){
                    std::cout<<"position "<<i<<" replacement match: "<<std::endl;
                    std::cout<<"suppose to be:"<<p2<<std::endl;
                    std::cout<<"real result:";
                    testExtract(rst.first,m);
                }
            }
        }
            //位置m-1直接替换返回后的串
        else if(i==m-1){
            for(char c='a';c<='z';c++){
                std::string p1 = pattern.substr(0,m-1)+c;
                std::pair<uindex,uindex> rst=suffixRange(p1);
                if(rst.first!=NotMatch && rst.second!=NotMatch){
                    std::cout<<"position "<<i<<" replacement match: "<<std::endl;
                    std::cout<<"suppose to be:"<<p1<<std::endl;
                    std::cout<<"real result:";
                }
            }
        }
        else{
            for(char c='a';c<='z';c++){
                std::string p1 = pattern.substr(0,i)+c;
                std::string p2 = pattern.substr(i+1);

                std::pair<uindex,uindex> a=suffixRange(p1);
                std::pair<uindex,uindex> b=F[i+1];
                if(a.first==NotMatch || b.first==NotMatch ){
                    return;
                }
                std::pair<uindex,uindex> rst=mergeSA(a,b,p1,p2);
                if(rst.first!=NotMatch && rst.second!=NotMatch){
                    std::cout<<"position "<<i<<" replacement match: "<<std::endl;
                    std::cout<<"suppose to be:"<<p1+p2<<std::endl;
                    std::cout<<"real result:";
                    testExtract(rst.first,m);
                }
            }
        }
    }

    void FuzzySearcher::testForInsertion(int i) {
        int m= pattern.length();

        //位置0插入直接返回插入后的串
        if(i==0){
            for(char c='a';c<='z';c++){
                std::string p2 = c+pattern;
                std::pair<uindex,uindex> rst=suffixRange(p2);
                if(rst.first!=NotMatch && rst.second!=NotMatch){
                    std::cout<<"position "<<i<<" insertion match: "<<std::endl;
                    std::cout<<"suppose to be:"<<p2<<std::endl;
                    std::cout<<"real result:";
                    testExtract(rst.first,m+1);
                }
            }
        }
            //位置m插入直接返回插入后的串
        else if(i==m){
            for(char c='a';c<='z';c++){
                std::string p1 = pattern+c;
                std::pair<uindex,uindex> rst=suffixRange(p1);
                if(rst.first!=NotMatch && rst.second!=NotMatch){
                    std::cout<<"position "<<i<<" insertion match: "<<std::endl;
                    std::cout<<"suppose to be:"<<p1<<std::endl;
                    std::cout<<"real result:";
                    testExtract(rst.first,m+1);
                }
            }
        }
        else{
            for(char c='a';c<='z';c++){
                std::string p1 = pattern.substr(0,i)+c;
                std::string p2 = pattern.substr(i);

                std::pair<uindex,uindex> a=suffixRange(p1);
                std::pair<uindex,uindex> b=F[i];
                if(a.first==NotMatch || b.first==NotMatch ){
                    return;
                }
                std::pair<uindex,uindex> rst=mergeSA(a,b,p1,p2);
                if(rst.first!=NotMatch && rst.second!=NotMatch){
                    std::cout<<"position "<<i<<" insertion match: "<<std::endl;
                    std::cout<<"suppose to be:"<<p1+p2<<std::endl;
                    std::cout<<"real result:";
                    testExtract(rst.first,m+1);
                }
            }
        }
    }
} // wildcard