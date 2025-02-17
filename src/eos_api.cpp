#include "eos_api.h"
#include <iostream>
#include <curl/curl.h>
#include <json/json.h>

// 用于接收 HTTP 响应数据
size_t WriteCallback(void *contents, size_t size, size_t nmemb, std::string *output) {
    size_t totalSize = size * nmemb;
    output->append((char *)contents, totalSize);
    return totalSize;
}

// 发送 HTTP 请求到 EOS API，获取区块链上的文件哈希
std::string getOriginalFileHash(const std::string &contract, const std::string &table, const std::string &scope) {
    std::string apiUrl = "https://eos.greymass.com/v1/chain/get_table_rows";  // EOS 公共 API
    std::string requestData = R"({
        "json": true,
        "code": ")" + contract + R"(",
        "scope": ")" + scope + R"(",
        "table": ")" + table + R"(",
        "limit": 1
    })";

    CURL *curl = curl_easy_init();
    if (!curl) {
        std::cerr << "CURL 初始化失败！" << std::endl;
        return "";
    }

    std::string response;
    curl_easy_setopt(curl, CURLOPT_URL, apiUrl.c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, requestData.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        std::cerr << "HTTP 请求失败：" << curl_easy_strerror(res) << std::endl;
        curl_easy_cleanup(curl);
        return "";
    }

    curl_easy_cleanup(curl);

    // 解析 JSON 数据
    Json::Value jsonData;
    Json::Reader reader;
    if (!reader.parse(response, jsonData)) {
        std::cerr << "JSON 解析失败" << std::endl;
        return "";
    }

    // 获取哈希值
    if (jsonData["rows"].size() > 0) {
        std::string originalHash = jsonData["rows"][0]["file_hash"].asString();
        return originalHash;
    }

    return "";
}