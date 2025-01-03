#include <fstream>
#include <string>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <chrono>
#include"proformance.h"
#include"global.h"
#include <regex>

CpuUsage preCpuStats, currCpuStats;//cpu使用情况
size_t totalMem, availableMem;//mem使用情况
NetworkStats preNetworkStats, currNetworkStats;//网络使用情况
double preDiskIOSecond, currDiskIOSecond;//磁盘IO利用率
std::regex pattern_disk_io("node_disk_io_time_seconds_total\\{device=\"sda\"\\}.*");

CpuUsage getCpuStats() {
    std::ifstream file("/proc/stat");
    std::string line;
    CpuUsage stats = {0};

    if (file.is_open()) {
        while (std::getline(file, line)) {
            if (line.find("cpu ") == 0) { // Read the aggregate CPU stats
                sscanf(
                    line.c_str(),
                    "cpu %llu %llu %llu %llu %llu %llu %llu %llu",
                    &stats.user,
                    &stats.nice,
                    &stats.system,
                    &stats.idle,
                    &stats.iowait,
                    &stats.irq,
                    &stats.softirq,
                    &stats.steal
                );
                break;
            }
        }
        file.close();
    }
    return stats;
}

void updateCpuStats() {
    preCpuStats = currCpuStats;
    currCpuStats = getCpuStats();
}

double calculateCpuUsage() {
    
    unsigned long long prevIdle = preCpuStats.idle + preCpuStats.iowait;
    unsigned long long currIdle = currCpuStats.idle + currCpuStats.iowait;

    unsigned long long prevNonIdle = preCpuStats.user + preCpuStats.nice + preCpuStats.system + preCpuStats.irq + preCpuStats.softirq + preCpuStats.steal;
    unsigned long long currNonIdle = currCpuStats.user + currCpuStats.nice + currCpuStats.system + currCpuStats.irq + currCpuStats.softirq + currCpuStats.steal;

    unsigned long long prevTotal = prevIdle + prevNonIdle;
    unsigned long long currTotal = currIdle + currNonIdle;

    unsigned long long totalDelta = currTotal - prevTotal;
    unsigned long long idleDelta = currIdle - prevIdle;

    if (totalDelta == 0) {
        return 0.0;
    }

    return (double)(totalDelta - idleDelta) / totalDelta * 100.0;
}

// 解析内存信息
bool parseMemInfo(size_t &totalMem, size_t &availableMem) {
    std::ifstream file("/proc/meminfo");
    std::string line;
    
    if (!file.is_open()) {
        std::cerr << "Unable to open file: " << "/proc/meminfo" << std::endl;
        return false;
    }

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string key;
        size_t value;

        // 解析每一行的键值对
        if (iss >> key >> value) {
            if (key == "MemTotal:") {
                totalMem = value;
            } else if (key == "MemAvailable:") {
                availableMem = value;
            }
        }
    }

    file.close();
    return true;
}

// 计算内存使用率
double calculateMemoryUsage() {
    // 内存使用率 = (总内存 - 可用内存) / 总内存
    return 100.0 * (totalMem - availableMem) / totalMem;
}

void updateMemoryStats() {
    
    if (!parseMemInfo(totalMem, availableMem)) {
        std::cout<<"内存使用率统计失败"<<std::endl;
    }
}

// 从 /proc/net/dev 中读取指定接口的统计数据
bool parseNetworkStats(const std::string& interface, NetworkStats& stats) {
    std::ifstream file("/proc/net/dev");
    if (!file.is_open()) {
        std::cerr << "Failed to open /proc/net/dev" << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.find(interface + ":") != std::string::npos) {
            std::istringstream iss(line.substr(line.find(":") + 1));
            iss >> stats.rxBytes; // 读取接收字节数
            size_t temp;
            for(int i = 0; i < 7; i++)
            {
                iss >> temp;
            }
            iss >> stats.txBytes; // 读取发送字节数
            return true;
        }
    }
    return false;
}

void updateNetworkStats() {
    preNetworkStats = currNetworkStats;
    if (!parseNetworkStats(networkInterface, currNetworkStats)) {
        std::cerr << "Failed to parse network stats" << std::endl;
    }
    // else{
    //     std::cout<<"网络接收带宽: "<<networkStats.rxBytes<<std::endl;
    //     std::cout<<"网络发送带宽: "<<networkStats.txBytes<<std::endl;
    // }
}

//计算网络带宽
NetworkBandwidth calculateNetworkBandwidth(){
    NetworkBandwidth networkBandwidth;//网络速率
    //接收带宽
    networkBandwidth.rxBps = (currNetworkStats.rxBytes - preNetworkStats.rxBytes)/collectInterval;//接收带宽
    //发送带宽
    networkBandwidth.txBps = (currNetworkStats.txBytes - preNetworkStats.txBytes)/collectInterval;//发送带宽
    return networkBandwidth;
}

//计算磁盘IO使用率
double getDiskIOTime(const std::string& device) {
    std::ifstream file("/proc/diskstats");
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string dev;
        unsigned long long ioTime;
        // Skipping irrelevant fields and extracting the relevant ones
        if (iss >> std::ws && line.find(device) != std::string::npos) {
            // std::cout << "line: " << line << std::endl;
            std::string temp;
            for (int i = 0; i < 12; i++) { // Skip first 11 columns
                iss >> temp;
            }
            iss >> ioTime; // Column 12
            // std::cout << "ioTime: " << ioTime << std::endl;
            return  ioTime/1000.0; // Convert ms to seconds
        }
    }
    return 0.0;
}

void updateDiskIOStats()
{
    preDiskIOSecond = currDiskIOSecond;
    currDiskIOSecond = getDiskIOTime("sda");
}

double calculateDiskIO()
{
    return (currDiskIOSecond - preDiskIOSecond)/collectInterval;
}