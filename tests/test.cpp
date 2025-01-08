#include"global.h"
#include<iostream>
#include<deque>
#include<fstream>
#include"proformance.h"
#include"file.h"
#include <sstream>
#include <cmath>
#include<regex>
size_t getFileSize(const std::string& fileName) {
    std::fstream file(fileName, std::ios::binary | std::ios::in | std::ios::ate); // 打开文件并将指针移到末尾
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + fileName);
    }
    return file.tellg(); // 获取文件末尾的指针位置，即文件大小
}

double getDiskIOTime(const std::string& device) {
    std::ifstream file("/proc/diskstats");
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string dev;
        unsigned long long ioTime;
        // Skipping irrelevant fields and extracting the relevant ones
        if (iss >> std::ws && line.find(device) != std::string::npos) {
            std::cout << "line: " << line << std::endl;
            std::string temp;
            for (int i = 0; i < 12; i++) { // Skip first 11 columns
                iss >> temp;
            }
            iss >> ioTime; // Column 12
            std::cout << "ioTime: " << ioTime << std::endl;
            return  ioTime/1000.0; // Convert ms to seconds
        }
    }
    return 0.0;
}

int checkStringFormat(const std::string& str) {
    // 正则表达式匹配三个由空格分隔的字符串
    std::istringstream stream(str);
    std::string tmp;
    int count = 0;
    while(std::getline(stream, tmp, ' ')){
        count++;
    }
    if(count != 3){
        return -1;
    }
    return 0;
}

int main()
{
    std::string str = "0 192.168.88.139";
    if(checkStringFormat(str) == 0)
        std::cout<<"matched"<<std::endl;
    return 0;
}