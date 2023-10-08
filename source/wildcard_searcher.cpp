//
// Created by dxy on 23-10-7.
//

#include "wildcard_searcher.h"
#include<chrono>
#include<iostream>
#include<cstring>
#include<execution>

namespace wildcard {
    bool WildcardSearcher::star_search(std::string pattern, double &runtime) {
        i64 L, R;
        i64 num, num1, num2; //接受locating函数返回的匹配个数
        //按*分割字符串
        char delimiter = '*';
        std::vector<std::string> substrings = splitString(pattern, delimiter);

        const int search_len = 256; //对每个成功匹配的结果进行取词时，最大的判定长度，即最后输出的单个串长最大值

        //如果只分割出来一个串，根据c++特性，说明*在串的末尾
        if (substrings.size() == 1) {
            auto start = std::chrono::high_resolution_clock::now(); //测时间开始

            std::vector<uindex> rst = csa->locate(substrings[0]);
            num=rst.size();
            //cout<<"*在串末尾，所有以返回串为前缀的串均匹配成功，locate匹配的个数为"<<num<<endl;
            //输出匹配的串,目前只允许输出字母和数字，别的遇到就截断
            for (int i = 0; i < num; i++) {
                std::vector<u8> str=csa->extract(rst[i] + substrings[0].length(), search_len);
                for (int j = 0; j < search_len; j++) {
                    if (str[j] != ' ' && str[j] != '?' && str[j] != '*' && str[j] != '/' && str[j] != '\\' &&
                        str[j] != '\'' && str[j] != '[' && str[j] != ']' && str[j] != '\n') {
                        //cout<<str[j];
                    }
                        //if((str[j]>='a' && str[j]<='z') || (str[j]>='A' && str[j]<='Z') || (str[j]>='0' && str[j]<='9') || str[j]=='.' || str[j]=='-' || str[j]=='_' || str[j]=='\\'){
                        //    //cout<<str[j];
                        //}
                    else {
                        //cout<<endl;
                        break;
                    }
                }
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
            std::cout << "*在串开头，所有以返回串为后缀的串均匹配成功，locate匹配的个数为" << num << std::endl;
            //输出匹配的串,目前只允许输出字母和数字，别的遇到就截断
            for (int i = 0; i < num; i++) {
                auto str = csa->extract(rst[i] - (search_len - substrings[1].length()),
                                        search_len);//提取出以模式串为后缀的长度为search_len的串

                char str_buf[search_len];
                memset(str_buf, 0, sizeof(str_buf));
                int acc_len = 0;//记录实际输出的串长
                for (int j = search_len - 1; j >= 0; j--) {
                    if (str[j] != ' ' && str[j] != '?' && str[j] != '*' && str[j] != '\'' && str[j] != '[' &&
                        str[j] != ']' && str[j] != '\n') {
                        str_buf[acc_len] = str[j];
                        acc_len++;
                    }
                        //if((str[j]>='a' && str[j]<='z') || (str[j]>='A' && str[j]<='Z') || (str[j]>='0' && str[j]<='9') || str[j]=='.' || str[j]=='-' || str[j]=='_' || str[j]=='\\'){
                        //    str_buf[acc_len]=str[j];
                        //    acc_len++;
                        //}
                    else {
                        break;
                    }
                }

                //for(int j=acc_len-1;j>=0;j--){
                //    cout<<str_buf[j];
                //}
                //cout<<endl;

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

            if (num1 > 100000) sort(std::execution::par, rst1.begin(), rst1.end());
            else sort(rst1.begin(), rst1.end());
            if (num1 > 100000) sort(std::execution::par, rst2.begin(), rst2.end());
            else sort(rst2.begin(), rst2.end());

            std::vector <std::pair<i64, i64>> pairs, final_pairs;

            //第一个数组从前开始遍历到最后，第二个数组从后往前遍历，直到第二个
            //数字小于第一个的时候break
            int front_satisfy = 0; //记录遍历第一个数组时每个数得到的在第二个数组中满足条件的第一个数的位置（第二个数组中的位置）
            int tail_satisfy = 0; //记录遍历第二个数组时每个数得到的在第一个数组中满足条件的最后一个数的位置（第一个数组中的位置）

            //auto str = csa->extract(rst1[0] + substrings[0].length(), search_len);

            for (i64 i = 0; i < num1; i++) {
                i64 j = 0;
                //如果上一轮的最后那个都不能满足这一轮的第一个标准的话，则从它的下一位开始搜索
                if (rst1[i] + substrings[0].length() > rst2[tail_satisfy]) {
                    j = tail_satisfy + 1;
                }
                    //否则从第一个满足条件的地方开始搜索
                else {
                    j = front_satisfy;
                }

                bool flag = true;
                for (; j < num2; j++) {
                    //满足条件,则加入到待检查队列中，并在第一次找到的时候更新front_satisfy
                    if (rst1[i] + substrings[0].length() <= rst2[j] && rst1[i] + search_len > rst2[j]) {
                        pairs.push_back(std::make_pair(rst1[i], rst2[j]));
                        if (flag) {
                            front_satisfy = j;
                            flag = false;
                        }
                        continue;
                    }
                    //不满足条件，但还暂时没找到第一个满足条件的，继续搜索
                    if (flag) {
                        continue;
                    }
                    //不满足条件，且flag已经为false，则更新tail_satisfy,并退出循环
                    tail_satisfy = j - 1;
                    break;
                }

                //如果待检查队列为空，则继续下一轮
                if (pairs.empty()) continue;

                //开始筛选，并把最终结果汇总到final_pairs中
                auto str = csa->extract(rst1[i] + substrings[0].length(),
                                        search_len); //从第一个串末尾的下一个位置开始提取出长度为search_len的串

                int acc_len = 0;//记录实际输出的串长
                //检查满足条件的最末位置
                for (int tt = 0; tt < search_len; tt++) {
                    //if((str[tt]>='a' && str[tt]<='z') || (str[tt]>='A' && str[tt]<='Z') || (str[tt]>='0' && str[tt]<='9') || str[tt]=='.' || str[tt]=='-' || str[tt]=='_' || str[tt]=='\\'){
                    //    acc_len++;
                    //}
                    if (str[tt] != ' ' && str[tt] != '?' && str[tt] != '*' && str[tt] != '/' && str[tt] != '\\' &&
                        str[tt] != '\'' && str[tt] != '[' && str[tt] != ']') {
                        acc_len++;
                    } else {
                        break;
                    }
                }

                for (auto p: pairs) {
                    //从第一个开始检查，满足条件则加入最终列表
                    if (rst1[i] + substrings[0].length() + acc_len >= p.second + substrings[1].length()) {
                        final_pairs.push_back(p);
                    }
                        //不满足条件则退出循环
                    else {
                        break;
                    }
                }
                //清空待检查队列
                pairs.clear();
            }

            //cout<<"*在串中间，共有"<<final_pairs.size()<<"个串满足匹配条件"<<endl;
            //输出满足条件的串
            for (auto p: final_pairs) {
                auto str = csa->extract(p.first, p.second - p.first + substrings[1].length());
                //cout<<str<<endl;
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