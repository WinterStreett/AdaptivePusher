#pragma once
#include<string>
#include<mutex>
#include<fstream>

extern std::string metricsInMemory;//指标数据保存在内存中的容器
extern std::mutex metricsInMemoryMtx;//metricsInMemory的独占锁，访问数据前先获取锁
extern int periodOfCollectMetrics;//收集数据的时间间隔
extern int periodOfPushMetrices;//推送数据的时间间隔
extern std::string exporterUrl;//exporter的IP
extern std::string vmUrl;//远程数据库的ip

extern std::string fileSaveMetricsName;//临时文件的文件名
extern size_t MAX_FILE_SIZE;//临时文件的最大字节数
extern size_t fileSaveMetricSize;//临时文件中数据的长度