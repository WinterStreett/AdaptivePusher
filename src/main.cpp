#include<iostream>
#include"global.h"
#include"collector.h"
#include <thread>
#include <signal.h>
#include"file.h"

void handleSigint(int signal) {
    std::cout << "Caught signal " << signal << ", cleaning up..." << std::endl;
    // 执行清理操作，例如关闭文件、释放资源等
    clearCollector();
    clearFile();
    exit(0); // 正常退出程序
}

int main()
{

    exporterUrls.push_back("http://localhost:9435/metrics");//17886
    // exporterUrls.push_back("http://localhost:9100/metrics");//61171

    hostInfo = "192.168.88.139";
    fileMaxSize = 1024 * 1024 * 1;//单文件大小10M
    fileMaxNum = 2;//最多5个文件
    filePath = "/home/winter/AdaptivePusher/data/";

    // 注册信号处理器
    signal(SIGINT, handleSigint);

    std::cout << "Running... Press Ctrl+C to terminate." << std::endl;

    initCollector(exporterUrls);
    while(true)
    {
        if(collect() == 0)
        {
            saveMetrics2File(metrics);
            metrics.clear();
            std::cout<<"save metrics!"<<std::endl;
        }

        if(hasMetricsFiles())
        {
            readMetricsFromFile(metrics);
            removeCurrentMetricsFile();
            // std::cout<<metrics<<std::endl;
            metrics.clear();
        }
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    return 0;
}