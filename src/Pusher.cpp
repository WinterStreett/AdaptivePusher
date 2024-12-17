#include"../include/Pusher.h"
#include"../include/File.h"
#include"../include/global.h"
#include<curl/curl.h>
#include<iostream>
#include<thread>
#include<fstream>
#include <sstream>

//一些辅助函数
double getNetworkPakectLossRate(const std::string& interface) {
    std::ifstream file("/proc/net/dev");
    if (!file.is_open()) {
        std::cout<<"Failed to open /proc/net/dev"<<std::endl;
        return 0;
    }

    std::string line;
    size_t rxPackets;
    size_t rxDropped;
    size_t txPackets;
    size_t txDropped;
    while (std::getline(file, line)) {
        if (line.find(interface) != std::string::npos) {
            std::istringstream iss(line);
            std::string iface;
            // std::string rxPackets;
            // std::string rxDropped;
            // std::string txPackets;
            // std::string txDropped;
            unsigned long dummy; // 占位变量

            iss >> iface;                     // 网络接口名称
            iss >> dummy >> rxPackets;  // 跳过bytes，获取 RX packets
            iss >> dummy >> rxDropped; // 跳过其他字段到 RX dropped
            

            for (int i = 0; i < 5; ++i) iss >> dummy; // 跳过 TX bytes 到 TX packets
            iss >> txPackets;          
            iss >> dummy >> txDropped; // 跳过其他字段到 TX dropped
            // std::cout<< "rxPackets: " << rxPackets<<std::endl;
            // std::cout<< "rxDropped: " << rxDropped<<std::endl;
            // std::cout<< "txPackets: " << txPackets<<std::endl;
            // std::cout<< "txDropped: " << txDropped<<std::endl;
            break;
        }
    }

    file.close();
    return (rxDropped+txDropped)/double(rxPackets+txPackets);
}

Pusher::Pusher(const std::string& url){
    curl = curl_easy_init();
    if (!curl) {
        throw std::runtime_error("Failed to initialize CURL");
    }
    // 设置目标 URL
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    // 设置 HTTP POST 方法
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: text/plain");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
}

Pusher::~Pusher(){
    // 清理资源
    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);
}

void Pusher::push(){
    if(metricsInMemory.size()==0)//如果数据收集出现问题，则不发起推送请求
        return;
    //申请metricsInMemory的独占权
    std::lock_guard<std::mutex> lock(metricsInMemoryMtx);

    std::ostringstream oss;
    for(const auto& str: metricsInMemory)
    {
        oss << str;
    }
    std::string metricsInMemoryStr = oss.str();

    //如果丢包率大于5%，则放弃本次推送，将metricsInMemory内容追加到文件中。
    if(getNetworkPakectLossRate(interface)>0.05)
    {
        try{
            writeStringToBinaryFile(fileSaveMetricsName,metricsInMemoryStr,1);
        }
        catch (const std::exception& e)
        {
            std::cerr << "Exception caught when write data to file: " << e.what() << std::endl;
        }
        metricsInMemory.clear();
        RSIndex = 0;
        std::cout<<"Abandoning pushing data due to high network packet loss rate detected"<<std::endl;
        return;
    }

    //   推送数据时，要先后读取文件，和metricsInMemory的内容合并后推送
    if(fileSaveMetricSize != 0)
    {
        try{   
            std::string data;
            readBinaryFileToString(fileSaveMetricsName,data);
            metricsInMemoryStr += std::move(data);
        }
        catch (const std::exception& e)
        {
            std::cerr << "Exception caught in push(): " << e.what() << std::endl;
        }
    }
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, metricsInMemoryStr.c_str());
        // 执行请求

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        //      当推送失败时，将metricsInMemory的内容存入文件
        //      最后清空metricsInMemory的内容

        try{
            writeStringToBinaryFile(fileSaveMetricsName,metricsInMemoryStr,0);
        }
        catch (const std::exception& e)
        {
            std::cerr << "Exception caught when write data to file: " << e.what() << std::endl;
        }

        metricsInMemory.clear();
        RSIndex = 0;
        std::string error(curl_easy_strerror(res));
        throw std::runtime_error("CURL request failed: " + error);
    } else {
        //      推送成功
        std::cout << "Data successfully pushed to VictoriaMetrics!" << std::endl;
        metricsInMemory.clear();
        RSIndex = 0;
    }
}

void Pusher::push_timer(){
    while(true)
    {
        try{
            push();
        }
        catch (const std::exception& e)
        {
            std::cerr << "Exception caught in push_timer: " << e.what() << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::seconds(periodOfPushMetrices));  // 暂停1秒

    }
}