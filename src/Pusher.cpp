#include"../include/Pusher.h"
#include"../include/File.h"
#include"../include/global.h"
#include<curl/curl.h>
#include<iostream>
#include<thread>
#include<fstream>

//一些辅助函数





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

    //   推送数据时，要先后读取文件，和metricsInMemory的内容合并后推送
    if(fileSaveMetricSize != 0)
    {
        try{   
            std::string data;
            readBinaryFileToString(fileSaveMetricsName,data);
            metricsInMemory += std::move(data);
        }
        catch (const std::exception& e)
        {
            std::cerr << "Exception caught in push(): " << e.what() << std::endl;
        }
    }
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, metricsInMemory.c_str());
        // 执行请求
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        //      当推送失败时，将metricsInMemory的内容存入文件
        //      最后清空metricsInMemory的内容
        try{
            writeStringToBinaryFile(fileSaveMetricsName,metricsInMemory,0);
        }
        catch (const std::exception& e)
        {
            std::cerr << "Exception caught when write data to file: " << e.what() << std::endl;
        }

        metricsInMemory.clear();
        std::string error(curl_easy_strerror(res));
        throw std::runtime_error("CURL request failed: " + error);
    } else {
        std::cout << "Data successfully pushed to VictoriaMetrics!" << std::endl;
        metricsInMemory.clear();
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