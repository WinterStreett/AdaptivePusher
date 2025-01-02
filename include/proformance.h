#ifndef ADAPTIVE_PUSHER_PROFORMANCE_H
#define ADAPTIVE_PUSHER_PROFORMANCE_H

struct CpuUsage {
    unsigned long long user, nice, system, idle, iowait, irq, softirq, steal;
};
CpuUsage getCpuStats();
double calculateCpuUsage(const CpuUsage& prev, const CpuUsage& curr);

#endif // ADAPTIVE_PUSHER_PROFORMANCE_H