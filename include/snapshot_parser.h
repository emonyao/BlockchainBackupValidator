#ifndef SNAPSHOT_PARSER_H
#define SNAPSHOT_PARSER_H

#include <vector>
#include <json/json.h>

std::vector<char> readBinaryFile(const std::string &filePath);
Json::Value parseSnapshot(const std::vector<char> &buffer);
void saveJsonToFile(const Json::Value &jsonData, const std::string &outputPath);

#endif