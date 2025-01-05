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