#ifndef PUSHER_H
#define PUSHER_H
#include <string>

void initPusher(const std::string& url);
void clearPusher();
int push(const std::string& data);

#endif // PUSHER_H