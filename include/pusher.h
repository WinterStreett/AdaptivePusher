#ifndef PUSHER_H
#define PUSHER_H
#include <string>
#include<vector>

void initPusher(const std::string& url);
void clearPusher();
int push(const std::string& data);

void initLZ4Pusher(const std::string& url);
int pushLZ4(const std::vector<char> &compressedData, const int& compressedSize);
#endif // PUSHER_H