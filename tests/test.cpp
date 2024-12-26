#include"global.h"
#include<iostream>
#include<deque>

int main()
{
    std::deque<int> dq;
    dq.push_back(1);
    dq.push_back(2);
    dq.push_back(3);
    std::cout<<dq.back()<<std::endl;
    std::cout<<dq.front()<<std::endl;
    dq.pop_front();
    std::cout<<dq.back()<<std::endl;

    std::cout<<dq.front()<<std::endl;

    return 0;
}