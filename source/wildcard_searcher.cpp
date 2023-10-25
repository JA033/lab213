//
// Created by dxy on 23-10-7.
//

#include "wildcard_searcher.h"
#include<chrono>
#include<iostream>
#include<cstring>
#include<execution>
#include<unordered_map>
#include <utility>
#include"parallel_hashmap/phmap.h"
#include<mutex>
#include "ThreadPool.h"

namespace wildcard {
    bool WildcardSearcher::star_search(std::string pattern, double &runtime) {
        ThreadPool pool(8);

        //按*分割字符串
        char delimiter = '*';
        std::vector<std::string> substrings = splitString(pattern, delimiter);

        //如果只分割出来一个串，根据c++特性，说明*在串的末尾
        if (substrings.size() == 1) {
            auto start = std::chrono::high_resolution_clock::now(); //测时间开始

            std::vector<uindex> rst = csa->locate(substrings[0]); //所有匹配的串的起始下标

            //调用线程池执行intercept
            std::vector<std::vector<uindex> > taskGroups;
            std::vector<uindex> currentTaskGroup;
            //把任务打包成1000个一组
            for (auto idx : rst) {
                currentTaskGroup.push_back(idx);

                if (currentTaskGroup.size() >= 1000) {
                    taskGroups.push_back(currentTaskGroup);
                    currentTaskGroup.clear();
                }
            }

            // 处理剩余的不足1000个的任务
            if (!currentTaskGroup.empty()) {
                taskGroups.push_back(currentTaskGroup);
            }

            for (auto taskGroup : taskGroups) {
                pool.enqueue([&,taskGroup]() {
                    for (auto idx : taskGroup) {
                        usize oneNum = builder.rank1(idx);
                        usize nextIdx = builder.select1(oneNum);
                        std::vector<u8> str = csa->extract(idx, nextIdx - idx);

                        // 执行任务的代码
                    }
                });
            }


            auto end = std::chrono::high_resolution_clock::now(); //测时间结束
            std::chrono::duration<double> duration = end - start;
            //std::cout << "运行时间: " << duration.count() << " 秒" << std::endl;
            runtime = duration.count();
            return true;
        }
            //如果第一个串长度为0，说明第一个字符为*
        else if (substrings[0].length() == 0) {
            auto start = std::chrono::high_resolution_clock::now(); //测时间开始

            std::vector<uindex> rst = csa->locate(substrings[1]);

            //调用线程池执行intercept
            std::vector<std::vector<uindex> > taskGroups;
            std::vector<uindex> currentTaskGroup;
            //把任务打包成1000个一组
            for (auto idx : rst) {
                currentTaskGroup.push_back(idx);

                if (currentTaskGroup.size() >= 1000) {
                    taskGroups.push_back(currentTaskGroup);
                    currentTaskGroup.clear();
                }
            }

            // 处理剩余的不足1000个的任务
            if (!currentTaskGroup.empty()) {
                taskGroups.push_back(currentTaskGroup);
            }

            for (auto taskGroup : taskGroups) {
                pool.enqueue([&,taskGroup]() {
                    for (auto idx : taskGroup) {
                        usize oneNum = builder.rank1(idx);
                        usize startIdx = builder.select1(oneNum-1);
                        std::vector<u8> str=csa->extract(startIdx+1, idx-startIdx+substrings[1].length());
                    }
                });
            }

            auto end = std::chrono::high_resolution_clock::now(); //测时间结束
            std::chrono::duration<double> duration = end - start;
            //std::cout << "运行时间: " << duration.count() << " 秒" << std::endl;
            runtime = duration.count();
            return true;
        }

        //否则的话*在串中间
        else {
            auto start = std::chrono::high_resolution_clock::now(); //测时间开始

            std::vector<uindex> rst1 = csa->locate(substrings[0]);
            std::vector<uindex> rst2 = csa->locate(substrings[1]);

            //std::unordered_map<usize,uindex> p2map;
            phmap::parallel_flat_hash_map<usize, uindex> p2map;
            std::for_each(std::execution::unseq,rst2.begin(),rst2.end(),[&](uindex idx){
                usize oneNum=builder.rank1(idx);
                p2map.insert(std::make_pair(oneNum,idx));
            });

//            std::vector< std::pair<uindex,usize> > temp;
//            for(auto idx: rst1){
//                usize oneNum=builder.rank1(idx);
//                auto it=p2map.find(oneNum);
//                if(it!=p2map.end()){
//                    usize p2Idx=it->second;
//                    temp.emplace_back(idx,p2Idx);
//                }
//            }


//            //调用线程池执行intercept
//            std::vector<std::vector<uindex> > taskGroups;
//            std::vector<uindex> currentTaskGroup;
//            //把任务打包成1000个一组
//            for (auto idx : rst1) {
//                currentTaskGroup.push_back(idx);
//
//                if (currentTaskGroup.size() >= 1000) {
//                    taskGroups.push_back(currentTaskGroup);
//                    currentTaskGroup.clear();
//                }
//            }
//
//            // 处理剩余的不足1000个的任务
//            if (!currentTaskGroup.empty()) {
//                taskGroups.push_back(currentTaskGroup);
//            }
//
//            for (const auto& taskGroup : taskGroups) {
//                pool.enqueue([&,taskGroup]() {
//                    for (auto idx : taskGroup) {
//                        usize oneNum=builder.rank1(idx);
//                        auto it=p2map.find(oneNum);
//                        if(it!=p2map.end()){
//                            usize p2Idx=it->second;
//                            std::vector<u8> str=csa->extract(idx, p2Idx-idx+substrings[1].length());
//                        }
//                    }
//                });
//            }

            std::for_each(std::execution::par,rst1.begin(),rst1.end(),[&](uindex idx){
                usize oneNum=builder.rank1(idx);
                auto it=p2map.find(oneNum);
                if(it!=p2map.end()){
                    usize p2Idx=it->second;
                    std::vector<u8> str=csa->extract(idx, p2Idx-idx+substrings[1].length());
                }
            });


            auto end = std::chrono::high_resolution_clock::now(); //测时间结束
            std::chrono::duration<double> duration = end - start;
            //std::cout << "运行时间2: " << duration2.count() << " 秒" << std::endl;
            runtime = duration.count();
            return true;
        }

    }

    std::vector<std::string> WildcardSearcher::splitString(std::string str, char delimiter) {
        std::vector<std::string> tokens;
        std::stringstream ss(str);
        std::string token;
        while (getline(ss, token, delimiter)) {
            tokens.push_back(token);
        }
        return tokens;
    }

    void WildcardSearcher::testForSsearch() {
        std::string path="/home/dxy/Documents/testdata/";
        std::string patternFile;  // 测试模式串文件路径
        std::cout<<"输入测试文件名"<<std::endl;
        std::cin>>patternFile;
        patternFile=path+patternFile;
        std::string pattern;  // 当前读取的测试模式串
        double totalTime = 0.0;  // 总查询时间
        int queryCount = 0;  // 查询次数

        // 打开测试模式串文件
        std::ifstream file(patternFile);
        if (!file)
        {
            std::cerr << "无法打开测试模式串文件" << std::endl;
            return;
        }
        // 逐行读取测试模式串并进行查询
        while (getline(file, pattern,'\n'))
        {
            if (!pattern.empty() && pattern.back() == '\r')
            {
                // 如果字符串最后一个字符是回车符，则移除它
                pattern = pattern.substr(0, pattern.length() - 1);
            }

            // 调用star_search()函数进行查询
            double runtime=0;
            //cout<<pattern<<endl;
            //cout<<pattern.length()<<endl;
            bool result = star_search(pattern,runtime);


            // 计算查询总时间和查询次数
            totalTime += runtime;
            queryCount++;
            std::cout<<queryCount<<' ' <<runtime<< std::endl;
        }

        // 计算平均查询时间
        double averageTime = (queryCount > 0) ? (totalTime / queryCount) : 0.0;

        std::cout << "平均查询时间：" << averageTime << " 秒" << std::endl;
        return;
    }

    void WildcardSearcher::testForWildcard() {
        std::string pattern;
        char mod;
        std::cout<<"输入查询模式"<<std::endl;
        std::cin>>mod;
        std::cout<<"输入查询模式串"<<std::endl;
        std::cin>>pattern;


        if(mod=='*'){
            double runtime;
            star_search(pattern,runtime);
            std::cout<<runtime<<std::endl;
        }
        else{
            std::cout<<"模式输入错误"<<std::endl;
        }
        return;
    }
}
// wildcard