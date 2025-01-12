#ifndef DATA_REDUCTION_H
#define DATA_REDUCTION_H
#include<string>
#include<map>
#include<vector>

enum {//发送数据时，头部携带的操作符
    op_create,
    op_update,
    op_delete
};

extern bool hasDataPatternSend;//标志pusher有没有将监控数据的模式发送给远程服务器
extern std::map<std::string, std::string> metricsValueHistory;

std::string deleteDataPattrnFromServer();//删除远程服务器上的数据模式
std::string generatePushContent(const std::string& rawMetrics);//根据指标数据生成要被推送的最终内容

std::vector<char> compressLZ4(const std::string &data);//压缩函数
#endif