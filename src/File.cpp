#include"../include/File.h"
#include"../include/global.h"
#include<string>
#include<fstream>
#include <sys/stat.h> // struct stat
#include <cstdio>
#include<iostream>

void readBinaryFileToString(const std::string& fileName, std::string& buffer)
{
    std::ifstream file;
    file.open(fileName,std::ios::binary | std::ios::in);
    if (!file.is_open()) {
        file.close();
        throw std::runtime_error("fun: readBinaryFileToString: Failed to open the file for reading.");
    }
    buffer.resize(fileSaveMetricSize, '\0');    // 为字符串分配足够的空间
                   
    file.read(&buffer[0], fileSaveMetricSize);             // 将文件内容读取到字符串中
    file.close();
    fileSaveMetricSize = 0;//当文件中的数据全部被读取后，文件中有效内容长度默认为0
}

void writeStringToBinaryFile(const std::string& fileName, std::string& data, const int& flag)
{
    std::ofstream file;
    if(flag == 0)//flag=0: 默认文件为空，清空内容再写入
    {
        file.open(fileName,std::ios::binary | std::ios::out);
    }
    else//默认情况下以追加模式写入
    {
        file.open(fileName,std::ios::binary | std::ios::out | std::ios::app);
    }
    if (!file.is_open()) {
        file.close();
        throw std::runtime_error("writeStringToBinaryFile: Failed to open the file for writing.");
    }

    size_t length = data.size();
    
    //for debug
    // std::cout<<"fileSize: "<<fileSaveMetricSize<<", "<<"length: "<<length<<", "<<"MAX_FILE_SIZE: "<<MAX_FILE_SIZE<<std::endl;
    
    //获取文件大小，如果文件大小即将超过阈值，则删除所有内容
    if(fileSaveMetricSize + length > MAX_FILE_SIZE)
    {
        file.close();
        if(std::remove(fileName.c_str()) != 0)
        {
            throw std::runtime_error("writeStringToBinaryFile: Error deleting file");
        }
        data.clear();//所有数据现在都在data中，直接删除
        return;
    }

    file.write(data.data(), data.size());
    file.close();
    fileSaveMetricSize += length;
}