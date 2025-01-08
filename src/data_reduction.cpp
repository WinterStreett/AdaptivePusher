#include"data_reduction.h"
#include"global.h"
#include<chrono>
#include<string>
#include<sstream>

bool hasDataPatternSend = false;
std::map<std::string, std::string> metricsValueHistory;

std::string getUnixTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch());
    return std::to_string(duration.count());
}

std::string deleteDataPattrnFromServer()//删除远程服务器上的数据模式
{
    std::string result;
    result.append(std::to_string(op_delete)).append(" ")
          .append(hostInfo).append(" ")
          .append(getUnixTimestamp()).append("\n");
    return result;
}

std::string generatePushContent(const std::string& rawMetrics)//根据指标数据生成要被推送的最终内容
{
    std::string result;
    std::istringstream input(rawMetrics);
    std::string line;
    std::string timestamp = getUnixTimestamp();
    std::istringstream lineStream;
    std::string metrics_name_part, metrics_value_part;
    // 尚未向服务器告知边缘收集的数据模式
    if(!hasDataPatternSend)
    {
        result.append(std::to_string(op_create)).append(" ")
              .append(hostInfo).append(" ")
              .append(timestamp).append("\n");

        metricsValueHistory.clear();
        while (std::getline(input, line)) {
            if (line.empty() || line[0] == '#') {
                // 忽略注释行和空行
                continue;
            }

            lineStream.clear();
            lineStream.str(line);
            lineStream >> metrics_name_part >> metrics_value_part;
            metricsValueHistory.insert(std::make_pair(metrics_name_part, metrics_value_part));
            result.append(line).append("\n");
        }
        return result;
    }
    // 已经向服务器告知了边缘收集的数据模式
    result.append(std::to_string(op_update)).append(" ")
          .append(hostInfo).append(" ")
          .append(timestamp).append("\n");
    //更新metricsValueHistory
    while (std::getline(input, line)) {
        if (line.empty() || line[0] == '#') {
            // 忽略注释行和空行
            continue;
        }

        lineStream.clear();
        lineStream.str(line);
        lineStream >> metrics_name_part >> metrics_value_part;
        // 如果历史数据中有这个指标
        if(metricsValueHistory.count((metrics_name_part)) > 0)
        {
            metricsValueHistory[metrics_name_part] = metrics_value_part;
        }
    }
    //根据新的metricsValueHistory生成推送内容
    for(const auto& pair : metricsValueHistory)
    {
        result.append(pair.second).append("\n");
    }
    return result;
}