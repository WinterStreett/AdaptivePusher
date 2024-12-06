#pragma once
#include<string>
#include<curl/curl.h>
class Pusher
{
private:
    CURL* curl;
    struct curl_slist* headers;
    
public:
    void push();
    Pusher(const std::string& url);
    ~Pusher();
    void push_timer();
};