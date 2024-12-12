#include"../include/global.h"

std::string metricsInMemory;
std::mutex metricsInMemoryMtx;

std::string exporterUrl;
std::string vmUrl;
int periodOfCollectMetrics;
int periodOfPushMetrices;

std::string fileSaveMetricsName;
size_t MAX_FILE_SIZE;
size_t fileSaveMetricSize;

std::string hostInfo;