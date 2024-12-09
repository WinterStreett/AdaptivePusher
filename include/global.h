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
extern std::fstream fileSaveMetrics;
extern bool isFileSaveMetricsEmpty;
extern std::string fileSaveMetricsName;