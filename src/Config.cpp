#include"../include/Config.h"
#include<fstream>
#include <sstream>
#include <unordered_map>

std::unordered_map<std::string, std::string> readConfig(const std::string& fileName) {
    std::unordered_map<std::string, std::string> config;
    std::ifstream file(fileName);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open configuration file.");
    }

    std::string line;
    while (std::getline(file, line)) {
        // 跳过空行或注释行
        if (line.empty() || line[0] == '#') continue;

        // 查找 '=' 分隔符
        size_t pos = line.find('=');
        if (pos == std::string::npos) continue; // 无效行，跳过

        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);

        // 去除键和值两端的空格
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);

        config[key] = value;
    }

    file.close();
    return config;
}