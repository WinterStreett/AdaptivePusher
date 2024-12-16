#include<iostream>
#include<string>
#include"Collector.h"
#include"Pusher.h"
#include<thread>
#include<time.h>
#include"../include/global.h"
#include"../include/Config.h"
#include<fstream>
#include <cmath>
#include <sstream>
#include <unordered_map>

int main(){

    std::unordered_map<std::string, std::string> config;
    try {
        config = readConfig("config.txt");
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    //需要手动配置的参数
    periodOfCollectMetrics = std::stoi(config["periodOfCollectMetrics"]);
    periodOfPushMetrices = std::stoi(config["periodOfPushMetrices"]);
    exporterUrl = config["exporterUrl"];
    vmUrl = config["vmUrl"];
    fileSaveMetricsName = config["fileSaveMetricsName"];
    hostInfo = config["hostInfo"];
    MAX_FILE_SIZE = std::stoi(config["MAX_FILE_SIZE"]);//最大文件大小：5MB
    samplingRatio = std::stod(config["samplingRatio"]);
    
    //不需要手动初始化的参数
    fileSaveMetricSize = 0;
    reservoirSize = static_cast<int>(std::round(
    static_cast<double>(periodOfPushMetrices) / periodOfCollectMetrics * samplingRatio));
    RSIndex = 0;

    Collector* collector= new Collector(exporterUrl);
    Pusher* pusher = new Pusher(vmUrl);

    std::thread push_timer_thread(&Pusher::push_timer,pusher);
    std::thread collect_timer_thread(&Collector::collect_timer, collector);
    collect_timer_thread.join();
    push_timer_thread.join();

    return 0;
}


