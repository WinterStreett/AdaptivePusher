#include<iostream>
#include"global.h"
#include"collector.h"
#include <thread>
#include <signal.h>

void handleSigint(int signal) {
    std::cout << "Caught signal " << signal << ", cleaning up..." << std::endl;
    // 执行清理操作，例如关闭文件、释放资源等
    clearCollector();
    exit(0); // 正常退出程序
}

int main()
{
    // 注册信号处理器
    signal(SIGINT, handleSigint);

    std::cout << "Running... Press Ctrl+C to terminate." << std::endl;

    exporterUrls.push_back("http://localhost:9435/metrics");//17886
    exporterUrls.push_back("http://localhost:9100/metrics");//61171

    initCollector(exporterUrls);
    while(collect()==0)
    {
        // std::cout<<"拉取数据成功"<<std::endl;
        std::cout<<metrics.size()<<std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    return 0;
}