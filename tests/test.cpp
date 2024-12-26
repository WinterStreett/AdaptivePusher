#include"global.h"
#include<iostream>
#include<deque>
#include<fstream>

size_t getFileSize(const std::string& fileName) {
    std::fstream file(fileName, std::ios::binary | std::ios::in | std::ios::ate); // 打开文件并将指针移到末尾
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + fileName);
    }
    return file.tellg(); // 获取文件末尾的指针位置，即文件大小
}

int main()
{
    
    std::string fileName = "/home/winter/AdaptivePusher/data/metrics0.txt";
    try {
        size_t fileSize = getFileSize(fileName);
        std::cout << "File size: " << fileSize << " bytes" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}