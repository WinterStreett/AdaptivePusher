#include"../include/global.h"

std::string metricsInMemory;
std::mutex metricsInMemoryMtx;

std::string exporterUrl;
std::string vmUrl;
int periodOfCollectMetrics;
int periodOfPushMetrices;

std::fstream fileSaveMetrics;
bool isFileSaveMetricsEmpty;
std::string fileSaveMetricsName;