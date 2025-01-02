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
    std::string networkInterface = "ens33";
    std::string temp1 = "node_network_receive_bytes_total{device=\""+networkInterface+"\"}.*";

    std::cout<<"node_cpu_seconds_total\\{cpu=\"(\\d+)\",mode=\"idle\"\\}.*"<<std::endl;
    std::cout<<temp1<<std::endl;
    return 0;
}