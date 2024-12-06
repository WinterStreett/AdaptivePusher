#pragma once
#include<string>
#include<mutex>

extern std::string metricsInMemory;
extern std::mutex metricsInMemoryMtx;
extern int periodOfCollectMetrics;
extern int periodOfPushMetrices;
extern std::string exporterUrl;
extern std::string vmUrl;
