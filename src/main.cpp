#include<iostream>
#include<string>
#include"Collector.h"
#include"Pusher.h"
#include<thread>
#include<time.h>
#include"../include/global.h"
#include<fstream>

int main(){
    periodOfCollectMetrics = 1;
    periodOfPushMetrices = 10;
    exporterUrl = "http://localhost:9435/metrics";
    vmUrl = "http://192.168.88.140:8428/api/v1/import/prometheus";
    fileSaveMetricsName = "../bin/data.bin";

    MAX_FILE_SIZE = 1024 * 1024 * 2;//最大文件大小：5MB
    fileSaveMetricSize = 0;

    Collector* collector= new Collector(exporterUrl);
    Pusher* pusher = new Pusher(vmUrl);

    std::thread push_timer_thread(&Pusher::push_timer,pusher);
    std::thread collect_timer_thread(&Collector::collect_timer, collector);
    collect_timer_thread.join();
    push_timer_thread.join();

    return 0;
}


