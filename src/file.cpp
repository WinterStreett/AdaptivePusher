#include"file.h"
#include"global.h"
#include<fstream>
#include<deque>
#include<string>
#include<iostream>

std::deque<std::string> fileNameStack;//存储文件名的栈，最新的文件会优先被弹出
int fileIndex = 0;//文件名的索引
int fileSize = 0;//当前文件的大小
std::fstream file;//文件流
//存储数据到文件
void saveMetrics2File(std::string& metrics)
{

    if(fileNameStack.size() == 0)//栈中没有文件，说明file没有打开
    {
        //打开文件
        std::string fileName = filePath + "metrics" + std::to_string(fileIndex) + ".txt";
        file.open(fileName, std::ios::out);
        if(!file.is_open())
        {
            std::cout<<"open file failed!"<<std::endl;
            return;
        }
        fileNameStack.push_back(fileName);//将文件名压入栈
        fileIndex++;
        file<<metrics;
        fileSize = metrics.size();
        file.close();
        return;
    }
    //栈中有文件，说明file已经打开
    if(fileSize + metrics.size() > fileMaxSize)//当前文件大小加上新数据大小超过了最大文件大小
    {
        file.close();
        if(fileNameStack.size() >= fileMaxNum)//栈中文件数量超过了最大文件数量
        {
            //删除最早的文件
            std::string fileName = fileNameStack.front();
            fileNameStack.pop_front();
            remove(fileName.c_str());//删除最早的文件
        }
        std::string fileName = filePath + "metrics" + std::to_string(fileIndex) + ".txt";
        file.open(fileName,std::ios::out);
        if(!file.is_open())
        {
            std::cout<<"open file failed!"<<std::endl;
            return;
        }
        fileIndex++;
        fileNameStack.push_back(fileName);
        file<<metrics;
        fileSize = metrics.size();
        file.close();
        return;
    }
    //当前文件大小加上新数据大小未超过最大文件大小
    std::string fileName = fileNameStack.back();
    file.open(fileName,std::ios::out | std::ios::app);  
    if(!file.is_open())
    {
        std::cout<<"open file failed!"<<std::endl;
        return;
    }
    file<<metrics;
    fileSize += metrics.size();
    file.close();
}
//判断是否还有存储数据的文件
bool hasMetricsFiles()
{
    return fileNameStack.size() > 0;
}

void removeCurrentMetricsFile()
{
    if(fileNameStack.size() == 0)
    {
        return;
    }
    std::string fileName = fileNameStack.back();
    fileNameStack.pop_back();
    remove(fileName.c_str());
}

//从文件读取数据
//返回值：0读取成功，-1读取失败
int readMetricsFromFile(std::string& metrics)
{   
    if(fileNameStack.size() == 0)
    {
        metrics.clear();//既然文件中没有内容，那么就清空metrics
        return 0;
    }
    std::string fileName = fileNameStack.back();
    file.open(fileName, std::ios::binary | std::ios::in | std::ios::ate);
    if(!file.is_open())
    {
        std::cout<<"open file failed!"<<std::endl;
        return -1;
    }
    size_t size = file.tellg(); // 获取文件大小
    file.seekg(0, std::ios::beg);       // 移动到文件开始

    metrics.resize(fileMaxSize, '\0');
    file.read(&metrics[0], fileMaxSize);
    file.close();
    return 0;
}

void clearFile()
{
    if(file.is_open())
    {
        file.close();
    }
}