#include"../include/Pusher.h"
#include"../include/global.h"
#include<curl/curl.h>
#include<iostream>
#include<thread>
#include<fstream>

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
    std::lock_guard<std::mutex> lock(metricsInMemoryMtx);
    //   推送数据时，要先后读取文件和metricsInMemory的内容，合并后推送
    if(!isFileSaveMetricsEmpty)
    {
        fileSaveMetrics.open(fileSaveMetricsName,std::ios::in);
        if (!fileSaveMetrics.is_open()) {
            std::cerr << "fileSaveMetrics: Failed to open the file." << std::endl;
        }
        else
        {
            metricsInMemory += std::string((std::istreambuf_iterator<char>(fileSaveMetrics)), std::istreambuf_iterator<char>());
        }
        fileSaveMetrics.close();
    }

    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, metricsInMemory.c_str());
        // 执行请求
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        //      当推送失败时，将metricsInMemory的内容存入文件
        //      最后清空metricsInMemory的内容
        fileSaveMetrics.open(fileSaveMetricsName,std::ios::in | std::ios::out | std::ios::trunc);
        if (!fileSaveMetrics.is_open()) {
            metricsInMemory.clear();
            throw std::runtime_error("push(): Failed to open the file.");
        }
        fileSaveMetrics << metricsInMemory;
        fileSaveMetrics.close();
        metricsInMemory.clear();
        isFileSaveMetricsEmpty = false;
        std::string error(curl_easy_strerror(res));
        throw std::runtime_error("CURL request failed: " + error);
    } else {
        std::cout << "Data successfully pushed to VictoriaMetrics!" << std::endl;
        metricsInMemory.clear();
        isFileSaveMetricsEmpty = true;
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