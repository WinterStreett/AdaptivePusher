#ifndef ADAPTIVE_PUSHER_FILE_H
#define ADAPTIVE_PUSHER_FILE_H

#include<string>
void saveMetrics2File(std::string& metrics);
int readMetricsFromFile(std::string& metrics);
bool hasMetricsFiles();
void removeCurrentMetricsFile();
void clearFile();
#endif // ADAPTIVE_PUSHER_FILE_H