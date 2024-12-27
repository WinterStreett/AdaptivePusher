#include<collector.h>
#include<vector>
#include<string>
#include<curl/curl.h>
#include<map>
#include"global.h"
#include<iostream>
#include<chrono>
#include <regex>
#include <ctime>

std::map<std::string, CURL*> exporterUrls2CURL;

//一些辅助函数
int busytimes = 0;
int idletimes = 0;
int prevbusytimes = 0;
int previdletimes = 0;
bool isBegin = true;


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

int extractNumber(const std::string& input) {
    size_t lastSpace = input.find_last_of(' '); // 找到最后一个空格
    if (lastSpace == std::string::npos) {
        throw std::runtime_error("No space found in the string.");
    }

    std::string numberPart = input.substr(lastSpace + 1); // 提取数字部分
    return static_cast<int>(std::stod(numberPart)); // 转换为整数
}


std::string processMetrics(const std::string& rawMetrics) {
    std::string result;
    std::istringstream input(rawMetrics);
    std::string line;
    std::string timestamp = getUnixTimestamp();

    while (std::getline(input, line)) {
        if (line.empty() || line[0] == '#') {
            // 忽略注释行和空行
            // result.append(line).append("\n");
            continue;
        }
        if(containsSubstring(line,R"(ebpf_exporter_ebpf_cpu_seconds_total{type="busy"})"))
        {
            prevbusytimes = busytimes;
            busytimes = extractNumber(line);
        }
        if(containsSubstring(line,R"(ebpf_exporter_ebpf_cpu_seconds_total{type="idle"})"))
        {
            previdletimes = idletimes;
            idletimes = extractNumber(line);
        }

        // line = addLabel(line,"source",hostInfo);
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
                std::cout<<"busytimes:"<<busytimes<<std::endl;
                std::cout<<"idletimes:"<<idletimes<<std::endl;
                std::cout<<"cpu usage:"<<static_cast<double>(busytimes - prevbusytimes) / (busytimes - prevbusytimes + idletimes - previdletimes) <<std::endl;
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