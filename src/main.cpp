#include<iostream>
#include"global.h"
#include"collector.h"

int main()
{
    exporterUrls.push_back("http://localhost:9435/metrics");//17886
    exporterUrls.push_back("http://localhost:9100/metrics");//61171

    initCollector(exporterUrls);
    if(collect()==0)
    {
        // std::cout<<"拉取数据成功"<<std::endl;
        std::cout<<metrics<<std::endl;
    }
    clearCollector();

    // std::cout<<"fuck fd"<<std::endl;
    return 0;
}