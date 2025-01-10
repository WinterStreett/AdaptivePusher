#ifndef ADAPTIVE_PUSHER_FILE_H
#define ADAPTIVE_PUSHER_FILE_H

#include<string>
void saveMetrics2File(std::string& metrics);
int readMetricsFromFile(std::string& metrics);
bool hasMetricsFiles();
void removeCurrentMetricsFile();
void clearFile();
void saveStringToFile(const std::string& filename, const std::string& content);
#endif // ADAPTIVE_PUSHER_FILE_H