#ifndef ADAPTIVE_PUSHER_GLOBAL_H
#define ADAPTIVE_PUSHER_GLOBAL_H

#include<string>
#include<vector>

extern std::string metrics;
// extern std::string additionalMetrics;//在推送数据中添加的新出现的额外指标
extern std::vector<std::string> exporterUrls;
extern std::string serverUrl;//远程服务器ip
extern std::string hostInfo;//标识数据来源的信息，注意不同数据源不要同名
extern int fileMaxSize;//单个文件的大小
extern int fileMaxNum;//文件的最大数量
extern std::string filePath;//文件路径
extern std::string networkInterface;//网络接口
extern int collectInterval;//采集间隔，单位：秒
extern int pushPeriod;//推送间隔，单位：采集间隔数
extern int maxPushPeriod;//最大推送间隔，单位：采集间隔数
extern int periodCounter;//周期计数器，单位：采集间隔数
extern std::string networkInterface;//网络接口
extern uint64_t maxNetworkThroughput;//最大网络吞吐量
extern int performanceThreshold;//性能阈值
extern double extractNumber(const std::string& input);
#endif // ADAPTIVE_PUSHER_GLOBAL_H