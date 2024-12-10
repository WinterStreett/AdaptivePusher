#pragma once
#include<string>
#include<mutex>
#include<fstream>

extern std::string metricsInMemory;
extern std::mutex metricsInMemoryMtx;
extern int periodOfCollectMetrics;
extern int periodOfPushMetrices;
extern std::string exporterUrl;
extern std::string vmUrl;

extern bool isFileSaveMetricsEmpty;
extern std::string fileSaveMetricsName;
extern size_t MAX_FILE_SIZE;