#ifndef ADAPTIVE_PUSHER_GLOBAL_H
#define ADAPTIVE_PUSHER_GLOBAL_H

#include<string>
#include<vector>
extern std::string metrics;
extern std::vector<std::string> exporterUrls;
extern std::string hostInfo;//标识数据来源的信息，注意不同数据源不要同名
extern int fileMaxSize;//单个文件的大小
extern int fileMaxNum;//文件的最大数量
extern std::string filePath;//文件路径
#endif // ADAPTIVE_PUSHER_GLOBAL_H