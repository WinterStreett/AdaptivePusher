#include"file.h"
#include"global.h"
#include<fstream>
#include<deque>
#include<string>

std::deque<std::string> fileNameStack;//存储文件名的栈，最新的文件会优先被弹出
int fileIndex = 0;//文件名的索引
std::fstream file;//最新文件的文件流
int fileSize = 0;//当前文件的大小

//存储数据到文件
void saveMetrics2File(std::string& metrics)
{
    if(fileNameStack.size() == 0)//栈中没有文件，说明file没有打开
    {
        //打开文件
        std::string fileName = "metrics" + std::to_string(fileIndex) + ".txt";
        file.open(fileName);
        fileNameStack.push_back(fileName);//将文件名压入栈
        fileIndex++;
        file<<metrics;


    }
    if(fileNameStack.size() > 0)//栈中有文件，说明file已经打开
    {
        //
        if(fileSize + metrics.size() > fileMaxSize)
        {
            file.close();
            
        }
        file<<metrics;
    }

    std::string fileName = "metrics" + std::to_string(fileIndex) + ".txt";
    std::ofstream out(fileName);
    out<<metrics;
    out.close();
    fileNameStack.push_back(fileName);
    fileIndex++;
}

//从文件读取数据
//返回值：0读取成功且无剩余文件，1读取成功但有剩余文件，-1读取失败
int readMetricsFromFile(std::string& metrics)
{
    return 0;
}