#include <fstream>
#include <string>
#include"proformance.h"

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

double calculateCpuUsage(const CpuUsage& prev, const CpuUsage& curr) {
    unsigned long long prevIdle = prev.idle + prev.iowait;
    unsigned long long currIdle = curr.idle + curr.iowait;

    unsigned long long prevNonIdle = prev.user + prev.nice + prev.system + prev.irq + prev.softirq + prev.steal;
    unsigned long long currNonIdle = curr.user + curr.nice + curr.system + curr.irq + curr.softirq + curr.steal;

    unsigned long long prevTotal = prevIdle + prevNonIdle;
    unsigned long long currTotal = currIdle + currNonIdle;

    unsigned long long totalDelta = currTotal - prevTotal;
    unsigned long long idleDelta = currIdle - prevIdle;

    if (totalDelta == 0) {
        return 0.0;
    }

    return (double)(totalDelta - idleDelta) / totalDelta * 100.0;
}