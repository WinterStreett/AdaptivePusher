#ifndef ADAPTIVE_PUSHER_COLLECTOR_H
#define ADAPTIVE_PUSHER_COLLECTOR_H

#include<vector>
#include<string>

void initCollector(std::vector<std::string>& urls);
void clearCollector();
int collect();

#endif