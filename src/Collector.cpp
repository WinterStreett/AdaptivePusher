#include"../include/Collector.h"
#include"../include/global.h"
#include<iostream>
#include<curl/curl.h>
#include<chrono>
#include<thread>
#include <mutex>
#include <regex>
#include <ctime>

//一些辅助函数
std::string getUnixTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch());
    return std::to_string(duration.count());
}

std::string addTimestampToMetrics(const std::string& rawMetrics) {
    std::string result;
    std::istringstream input(rawMetrics);
    std::string line;
    std::string timestamp = getUnixTimestamp();

    while (std::getline(input, line)) {
        if (line.empty() || line[0] == '#') {
            // 忽略注释行和空行
            result.append(line).append("\n");
            continue;
        }
        line.append(" ").append(timestamp);
        result.append(line).append("\n");
    }
    return result;
}

// 回调函数，用于将拉取到的数据写入 std::string
size_t Collector::WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output)
{
    size_t totalSize = size * nmemb;
    output->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

//构造函数，初始化curl变量
Collector::Collector(const std::string& url)
{
    ebpfExporterUrl = url;
    curl = curl_easy_init();
    if (!curl) {
        throw std::runtime_error("Failed to initialize CURL");
    }

    // 设置 CURL 选项
    curl_easy_setopt(curl, CURLOPT_URL, ebpfExporterUrl.c_str());
    // curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:9435/metrics");

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buff);
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
}

Collector::~Collector()
{
    curl_easy_cleanup(curl);
}

//执行一次数据拉取
void Collector::collect()
{
    // if(curl == nullptr)
    //     std::cout<<"1"<<std::endl;
    //回调函数将metrics追加到metricsInMemory，需要加锁以确保线程安全
    std::lock_guard<std::mutex> lock(metricsInMemoryMtx);
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        std::string error(curl_easy_strerror(res));
        throw std::runtime_error("CURL request failed: " + error);
    } else {
        buff = addTimestampToMetrics(buff);
        metricsInMemory.append(buff);
        buff.clear();
        std::cout << "Metrics fetched successfully:" << std::endl;
        // std::cout << metricsInMemory << std::endl; // 输出数据
    }
}

//定时任务线程函数
void Collector::collect_timer()
{
    while(true)
    {
        try{
            collect();
        }
        catch (const std::exception& e){
            std::cerr << "Exception caught in collect_timer: " << e.what() << std::endl;
        }
        
        std::this_thread::sleep_for(std::chrono::seconds(periodOfCollectMetrics));  // 暂停1秒
    }
}
