#include<iostream>
#include<string>
#include"Collector.h"
#include"Pusher.h"
#include<thread>
#include<time.h>
#include"../include/global.h"
#include<fstream>
#include <cmath>

int main(){
    //需要手动配置的参数
    periodOfCollectMetrics = 1;
    periodOfPushMetrices = 10;
    exporterUrl = "http://localhost:9435/metrics";
    vmUrl = "http://192.168.88.140:8428/api/v1/import/prometheus";
    fileSaveMetricsName = "../bin/data.bin";
    hostInfo = "192.168.88.139";
    MAX_FILE_SIZE = 1024 * 1024 * 2;//最大文件大小：5MB
    samplingRatio = 0.5;
    
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


