#pragma once
#include<curl/curl.h>
#include<string>

class Collector{
private:
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output);
    std::string ebpfExporterUrl;
    CURL* curl;
    
    
public:
    void collect();
    void collect_timer();
    Collector(const std::string& url);
    ~Collector();
};