#include"pusher.h"
#include"global.h"
#include<curl/curl.h>
#include<iostream>

CURL* curl;
struct curl_slist* headers;

void initPusher(const std::string& url)
{
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

void clearPusher()
{
    // 清理资源
    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);
}

int push(const std::string& data)
{
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        std::cerr << "Failed to push metrics: " << curl_easy_strerror(res) << std::endl;
        return -1;
    }
    // std::cout << "Data successfully pushed to VictoriaMetrics!" << std::endl;
    return 0;
}

//lz4压缩版本的推送流程
void initLZ4Pusher(const std::string& url){
    curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Failed to initialize CURL" << std::endl;
        return;
    }
    // 设置目标 URL
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        // 设置 HTTP POST 方法
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
}

int pushLZ4(const std::vector<char> &compressedData, const int& compressedSize){
    //debug
    // std::cout<<"compressedData.size(): "<<compressedData.size()<<std::endl;

    headers = nullptr;
    // 设置 Content-Type 为 application/octet-stream
    headers = curl_slist_append(headers, "Content-Type: application/octet-stream");
    headers = curl_slist_append(headers, ("LZ4-Compressed-Length: " + std::to_string(compressedSize)).c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    // 设置 POST 数据和数据长度
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, compressedData.data());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, compressedData.size());
    // 执行请求
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        std::cerr << "CURL error: " << curl_easy_strerror(res) << std::endl;
        return -1;
    }
    return 0;
}