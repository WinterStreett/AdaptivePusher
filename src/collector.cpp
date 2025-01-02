#include<collector.h>
#include<vector>
#include<string>
#include<curl/curl.h>
#include<map>
#include"global.h"
#include"proformance.h"
#include<iostream>
#include<chrono>
#include <regex>
#include <ctime>

std::map<std::string, CURL*> exporterUrls2CURL;

//一些辅助函数和变量
// double totaltimes = 0;//cpu总时间
// double idletimes = 0;//cpu空闲时间
// double prevtotaltimes = 0;//上一次cpu总时间
// double previdletimes = 0;//上一次cpu空闲时间
CpuUsage prevStats, currStats;//cpu使用情况

double totalMemory = 0;//总内存
double availableMemory = 0;//可用内存

double diskIO = 0;//磁盘IO利用率
bool isBegin = true;

double networkReceive = 0;//网络接收
double networkTransmit = 0;//网络发送
double prevNetworkReceive = 0;//上一次网络接收
double prevNetworkTransmit = 0;//上一次网络发送
//正则匹配的模式定义
//cpu相关
std::regex pattern_idle_time("node_cpu_seconds_total\\{cpu=\"(\\d+)\",mode=\"idle\"\\}.*");
std::regex pattern_cpu_time("node_cpu_seconds_total.*");
//内存相关
std::regex pattern_memory_total("node_memory_MemTotal_bytes.*");
std::regex pattern_memory_avail("node_memory_MemAvailable_bytes.*");
//磁盘IO相关
std::regex pattern_disk_io("node_disk_io_time_seconds_total.*");
//网络相关
std::regex pattern_network_receive("node_network_receive_bytes_total\\{device=\"ens33\"\\}.*");
std::regex pattern_network_transmit("node_network_transmit_bytes_total\\{device=\"ens33\"\\}.*");


std::string getUnixTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch());
    return std::to_string(duration.count());
}

//给数据加上自定义标签
std::string addLabel(const std::string& original, const std::string& newLabelKey, const std::string& newLabelValue) {
    // 找到标签部分的开始位置
    size_t start = original.find('{');
    size_t end = original.find('}', start);

    // 如果找到了标签部分，插入新标签
    if (start != std::string::npos && end != std::string::npos) {
        std::string newLabel = newLabelKey + "=\"" + newLabelValue + "\"";
        std::string updated = original;
        updated.insert(end, "," + newLabel);
        return updated;
    }

    // 如果没有标签部分，则在数据名后添加整个标签
    size_t spacePos = original.find(' '); // 查找第一个空格
    if (spacePos != std::string::npos) {
        // 在空格前插入标签
        return original.substr(0, spacePos) + "{" + newLabelKey + "=\"" + newLabelValue + "\"}" +
               original.substr(spacePos);
    }

    // 如果没有空格，则假设原始数据只有名字
    return original + "{" + newLabelKey + "=\"" + newLabelValue + "\"}";
}

bool containsSubstring(const std::string& mainStr, const std::string& subStr) {
    return mainStr.find(subStr) != std::string::npos;
}

double extractNumber(const std::string& input) {
    size_t lastSpace = input.find_last_of(' '); // 找到最后一个空格
    if (lastSpace == std::string::npos) {
        throw std::runtime_error("No space found in the string.");
    }

    std::string numberPart = input.substr(lastSpace + 1); // 提取数字部分
    return std::stod(numberPart); // 转换为浮点数
}

// //计算cpu使用率
// double get_cpu_usage()
// {
//     return 1.0 - (idletimes - previdletimes) / (totaltimes - prevtotaltimes);
// }

//计算内存使用率
double get_memory_usage()
{
    return 1.0 - availableMemory / totalMemory;
}

//计算磁盘IO使用率
double get_disk_io_usage()
{
    return diskIO;
}

//计算网络IO带宽，单位：Byte/s
double get_network_receive_bandwidth()
{
    return (networkReceive - prevNetworkReceive)/collectInterval;
}

double get_network_transmit_bandwidth()
{
    return (networkTransmit - prevNetworkTransmit)/collectInterval;
}

void update_proformance_data(){
    //更新cpu相关变量，用于计算cpu使用率
    prevStats = currStats;
    currStats = getCpuStats();

    // if(std::regex_match(line, pattern_cpu_time))
    // {
    //     double temp = extractNumber(line);
    //     totaltimes += temp;
    //     if(std::regex_match(line, pattern_idle_time))
    //     {
    //         idletimes += temp;
    //     }
    // }
    // else if(std::regex_match(line, pattern_memory_total))
    // {
    //     totalMemory = extractNumber(line);
    // }
    // else if(std::regex_match(line, pattern_memory_avail))
    // {
    //     availableMemory = extractNumber(line);
    // }
    // else if(std::regex_match(line, pattern_disk_io))
    // {
    //     diskIO = extractNumber(line);
    // }
    // else if(std::regex_match(line, pattern_network_receive))
    // {
    //     prevNetworkReceive = networkReceive;
    //     networkReceive = extractNumber(line);
    // }
    // else if(std::regex_match(line, pattern_network_transmit))
    // {
    //     prevNetworkTransmit = networkTransmit;
    //     networkTransmit = extractNumber(line);
    // }
    // else if(std::regex_match(line, pattern_network_receive))
    // {
    //     prevNetworkReceive = networkReceive;
    //     networkReceive = extractNumber(line);
    // }
}

std::string processMetrics(const std::string& rawMetrics) {
    std::string result;
    std::istringstream input(rawMetrics);
    std::string line;
    std::string timestamp = getUnixTimestamp();

    // prevtotaltimes = totaltimes;
    // previdletimes = idletimes;
    // totaltimes = 0;
    // idletimes = 0;
    update_proformance_data();
    while (std::getline(input, line)) {
        if (line.empty() || line[0] == '#') {
            // 忽略注释行和空行
            // result.append(line).append("\n");
            continue;
        }

        line.append(" ").append(timestamp);
        result.append(addLabel(line,"source",hostInfo)).append("\n");
    }
    return result;
}

// 回调函数，用于将拉取到的数据写入 std::string
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output)
{
    size_t totalSize = size * nmemb;
    output->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

//初始化curl句柄，建立url到curl句柄的映射，关系保存在exporterUrls2CURL
void initCollector(std::vector<std::string>& urls)
{
    for(auto url : urls)
    {
        CURL* curl = curl_easy_init();
        if (!curl) {
            throw std::runtime_error("Failed to initialize CURL");
        }
            // 设置 CURL 选项
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &metrics);
        curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
        exporterUrls2CURL[url] = curl;
    }
}

//拉取数据
int collect()
{
    for(auto it = exporterUrls2CURL.begin(); it != exporterUrls2CURL.end(); ++it)
    {
        CURLcode res = curl_easy_perform(it->second);
         //返回值非表示拉取数据失败
        if (res != CURLE_OK)
        {
            std::cout<<"拉取数据源："<<it->first<<"失败！"<<std::endl;
            return -1;
        }
        else{
            metrics = processMetrics(metrics);
            if(!isBegin)
            {
                std::cout<<"cpu使用率: "<<calculateCpuUsage(prevStats,currStats)<<std::endl;
                // std::cout<<"内存使用率: "<<get_memory_usage()<<std::endl;
                // std::cout<<"磁盘IO使用率: "<<get_disk_io_usage()<<std::endl;
                // std::cout<<"网络接收带宽: "<<get_network_receive_bandwidth()<<std::endl;
                // std::cout<<"网络发送带宽: "<<get_network_transmit_bandwidth()<<std::endl;
            }
            else{
                isBegin = false;
            }
        }
    }
    return 0;
}

// 释放资源
void clearCollector()
{
    for(auto it = exporterUrls2CURL.begin(); it != exporterUrls2CURL.end(); ++it)
    {
        curl_easy_cleanup(it->second);
    }
}