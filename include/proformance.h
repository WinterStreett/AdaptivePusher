#ifndef ADAPTIVE_PUSHER_PROFORMANCE_H
#define ADAPTIVE_PUSHER_PROFORMANCE_H

struct CpuUsage {
    unsigned long long user, nice, system, idle, iowait, irq, softirq, steal;
};

struct NetworkStats {//网络使用情况
    uint64_t rxBytes; // 接收的字节数
    uint64_t txBytes; // 发送的字节数
};

struct NetworkBandwidth {//网络带宽
    uint64_t rxBps; // 接收速率
    uint64_t txBps; // 发送速率
};
//获取cpu使用情况
double calculateCpuUsage();
void updateCpuStats();
//获取内存使用情况
double calculateMemoryUsage();
void updateMemoryStats();
//获取网络使用情况
NetworkBandwidth calculateNetworkBandwidth();
void updateNetworkStats();
//获取磁盘IO使用情况
void updateDiskIOStats();
double calculateDiskIO();
#endif // ADAPTIVE_PUSHER_PROFORMANCE_H