#include"global.h"
#include<iostream>

std::string metrics;
std::vector<std::string> exporterUrls;
std::string serverUrl;//远程服务器ip
std::string hostInfo;
int fileMaxSize;//单个文件的大小
int fileMaxNum;//文件的最大数量
std::string filePath;//文件路径
std::string networkInterface;
int collectInterval;//采集间隔
int pushPeriod;//推送间隔
int periodCounter;//周期计数器
int maxPushPeriod;
uint64_t maxNetworkThroughput;
int performanceThreshold;

double extractNumber(const std::string& input) {
    size_t lastSpace = input.find_last_of(' '); // 找到最后一个空格
    if (lastSpace == std::string::npos) {
        throw std::runtime_error("No space found in the string.");
    }

    std::string numberPart = input.substr(lastSpace + 1); // 提取数字部分
    return std::stod(numberPart); // 转换为浮点数
}