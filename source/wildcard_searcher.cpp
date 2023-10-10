//
// Created by dxy on 23-10-7.
//

#include "wildcard_searcher.h"
#include<chrono>
#include<iostream>
#include<cstring>
#include<execution>
#include<unordered_map>

namespace wildcard {
    bool WildcardSearcher::star_search(std::string pattern, double &runtime) {
        i64 num, num1, num2; //接受locating函数返回的匹配个数
        //按*分割字符串
        char delimiter = '*';
        std::vector<std::string> substrings = splitString(pattern, delimiter);

        const int search_len = 256; //对每个成功匹配的结果进行取词时，最大的判定长度，即最后输出的单个串长最大值

        //如果只分割出来一个串，根据c++特性，说明*在串的末尾
        if (substrings.size() == 1) {
            auto start = std::chrono::high_resolution_clock::now(); //测时间开始

            std::vector<uindex> rst = csa->locate(substrings[0]); //所有匹配的串的起始下标
            num=rst.size();
            int j=0;
            for(usize i=0;i<num;i++){
                usize oneNum=builder.rank1(rst[i]); //当前串的行数
                usize nextIdx=builder.select1(oneNum); //下一个串的起始位置
                std::vector<u8> str=csa->extract(rst[i], nextIdx-rst[i]);
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
            num=rst.size();
            for(usize i=0;i<num;i++){
                usize oneNum=builder.rank1(rst[i]); //当前串的行数
                usize startIdx=builder.select1(oneNum-1); //当前串的起始位置

                std::vector<u8> str=csa->extract(startIdx+1, rst[i]-startIdx+substrings[1].length());
            }

            auto end = std::chrono::high_resolution_clock::now(); //测时间结束
            std::chrono::duration<double> duration = end - start;
            //std::cout << "运行时间: " << duration.count() << " 秒" << std::endl;
            runtime = duration.count();
            return true;
        }
            //TODO:原来的*写法

            //否则的话*在串中间
        else {
            auto start = std::chrono::high_resolution_clock::now(); //测时间开始

            std::vector<uindex> rst1 = csa->locate(substrings[0]);
            std::vector<uindex> rst2 = csa->locate(substrings[1]);
            num1=rst1.size(); num2=rst2.size();

            //对p2创建的每个locate值计算rank值，并记录到map中
            std::unordered_map<usize,uindex> p2map;
            for(uindex locateIdx:rst2){
                usize rankNum=builder.rank1(locateIdx);
                p2map.insert(std::make_pair(rankNum,locateIdx));
            }

            for(uindex locateIdx:rst1){
                usize rankNum=builder.rank1(locateIdx);
                auto it=p2map.find(rankNum);
                if(it!=p2map.end()){
                    usize p2Idx=it->second;
                    std::vector<u8> str=csa->extract(locateIdx, p2Idx-locateIdx+substrings[1].length());
                }
            }

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
        }
        else{
            std::cout<<"模式输入错误"<<std::endl;
        }
        return;
    }
}
// wildcard