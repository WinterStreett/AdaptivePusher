#include <yaml-cpp/yaml.h>
#include<string>
#include<vector>
#include<fstream>
#include"global.h"

void readConfig(const std::string& configFile){
    YAML::Node config = YAML::LoadFile(configFile);
    // std::cout<<config["exporterUrls"].size()<<std::endl;
    for(auto url : config["exporter_urls"]){
        exporterUrls.push_back(url.as<std::string>());
    }
    serverUrl = config["server_url"].as<std::string>();
    hostInfo = config["host_info"].as<std::string>();
    fileMaxSize = config["file_max_size"].as<int>();
    fileMaxNum = config["file_max_num"].as<int>();
    filePath = config["store_file_path"].as<std::string>();
    collectInterval = config["collect_interval"].as<int>();

    maxPushPeriod = config["max_push_period"].as<int>();
    networkInterface = config["network_interface"].as<std::string>();
    maxNetworkThroughput = config["max_network_throughput"].as<uint64_t>();
    performanceThreshold = config["performance_threshold"].as<int>();
}