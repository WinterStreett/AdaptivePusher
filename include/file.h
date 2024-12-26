#ifndef FILE_H
#define FILE_H

#include<string>
void saveMetrics2File(std::string& metrics);
int readMetricsFromFile(std::string& metrics);
bool hasMetricsFiles();
void removeCurrentMetricsFile();
void clearFile();
#endif // FILE_H