#pragma once
#include<string>
void readBinaryFileToString(const std::string& fileName, std::string& buffer);
void writeStringToBinaryFile(const std::string& fileName, std::string& data, const int& flag);
