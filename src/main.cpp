#include<iostream>
#include"global.h"
#include"collector.h"
#include <thread>
#include <signal.h>
#include"file.h"
#include"pusher.h"
#include <cmath>
#include"proformance.h"
#include"data_reduction.h"

void handleSigint(int signal) {
    std::cout << "Caught signal " << signal << ", cleaning up..." << std::endl;
    // 执行清理操作，例如关闭文件、释放资源等
    std::string deleteMessage = deleteDataPattrnFromServer();
    pushLZ4(compressLZ4(deleteMessage), deleteMessage.size());
    std::cout<<"删除数据模式"<<std::endl;
    clearCollector();
    clearPusher();
    clearFile();
    exit(0); // 正常退出程序
}

void updatePushPeriod(){
    double load = calculateLoad();
    if(load < performanceThreshold)
    {
        pushPeriod = 1;
    }
    else
    {
        pushPeriod = 1 + std::ceil(static_cast<double>(maxPushPeriod-1)*(load-performanceThreshold)/(100 - performanceThreshold));
    }
}

int main()
{
    // exporterUrls.push_back("http://localhost:9435/metrics");//17886
    exporterUrls.push_back("http://localhost:9100/metrics");//61171
    // serverUrl = "http://192.168.88.140:8428/api/v1/import/prometheus";
    serverUrl = "http://192.168.88.140:9900/metrics";
    hostInfo = "192.168.88.139";
    fileMaxSize = 1024 * 1024 * 1;//单文件大小10M
    fileMaxNum = 10;//最多5个文件
    filePath = "/home/winter/AdaptivePusher/data/";
    collectInterval = 2;//单位：秒
    pushPeriod = 1;//单位：采集间隔数
    periodCounter = 0;//单位：采集间隔数
    maxPushPeriod = 50;//单位：采集间隔数
    networkInterface = "ens33";
    maxNetworkThroughput = 125 * 1000 * 1000;//1 Gbps = 125 MB/s
    performanceThreshold = 20;
    // 注册信号处理器
    signal(SIGINT, handleSigint);

    std::cout << "Running... Press Ctrl+C to terminate." << std::endl;
    bool isFirstPeriod = true;
    initCollector(exporterUrls);
    // initPusher(serverUrl);
    initLZ4Pusher(serverUrl);
    while(true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(collectInterval));
        periodCounter++;

        if(collect() != 0)
        {
            metrics.clear();
            continue;
        }

        //判断当前是否处于推送周期
        if(periodCounter < pushPeriod){
            //未到推送周期
            saveMetrics2File(metrics);
            metrics.clear();
            continue;
        }

        //到达推送周期
        periodCounter = 0;
        //更新推送周期
        if(isFirstPeriod)
        {
            pushPeriod = 1; 
            isFirstPeriod = false;  
        }
        else
        {
            updatePushPeriod();
        }
        pushPeriod = 5;
        // std::cout<<"推送周期："<<pushPeriod<<std::endl;
        if(pushLZ4(compressLZ4(metrics), metrics.size()) != 0)//推送失败则将数据保存到文件，并进行下一次收集
        {
            if(hasDataPatternSend)//如果数据模式尚未推送给服务器，那么也不会将历史数据保存到文件
                saveMetrics2File(metrics);
            metrics.clear();
            continue;
        }   
        hasDataPatternSend = true;
        //存在临时文件，循环读取每个临时文件并推送
        while(hasMetricsFiles())
        {
            if(readMetricsFromFile(metrics) == 0)
            {
                // push(metrics);//不判断是否推送成功
                pushLZ4(compressLZ4(metrics), metrics.size());
            }
            removeCurrentMetricsFile();//就算推送失败默认该文件已经没有存储价值
                                        //将空间留给更新的数据
        }
        metrics.clear();
    }

    return 0;
}