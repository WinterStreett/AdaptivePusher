#include"../include/global.h"

std::string metricsInMemory;
std::mutex metricsInMemoryMtx;

std::string exporterUrl;
std::string vmUrl;
int periodOfCollectMetrics;
int periodOfPushMetrices;

bool isFileSaveMetricsEmpty;
std::string fileSaveMetricsName;
size_t MAX_FILE_SIZE;