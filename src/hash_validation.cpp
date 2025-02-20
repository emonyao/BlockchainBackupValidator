#include "hash_validation.h"
#include "eos_api.h"  // 用于 API 访问
#include <iostream>
#include <fstream>
#include <sstream>
#include <openssl/sha.h>
#include <json/json.h>
#include <curl/curl.h>
#include <iomanip>
#include <openssl/evp.h>
#include "http_utils.h" 

using namespace std;

//  回调函数：用于接收 HTTP 响应数据
// static size_t WriteCallback(void *contents, size_t size, size_t nmemb, string *output) {
//     size_t totalSize = size * nmemb;
//     output->append((char *)contents, totalSize);
//     return totalSize;
// }


// 发送 HTTP 请求并解析 JSON
Json::Value fetchJsonData(const string &url, const string &postData = "") {
    CURL *curl = curl_easy_init();
    if (!curl) {
        cerr << "[ERROR] CURL 初始化失败！" << endl;
        return Json::Value();
    }

    string response;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    if (!postData.empty()) {
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());
    }

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        cerr << "[ERROR] HTTP 请求失败: " << curl_easy_strerror(res) << endl;
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
        return Json::Value();
    }

    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);

    // 解析 JSON
    Json::Value jsonData;
    Json::Reader reader;
    if (!reader.parse(response, jsonData)) {
        cerr << "[ERROR] JSON 解析失败" << endl;
        return Json::Value();
    }

    return jsonData;
}

// 计算 SHA-256 哈希值
// std::string computeSHA256(const std::string &data) {
//     EVP_MD_CTX *ctx = EVP_MD_CTX_new();
//     if (!ctx) {
//         cerr << "[ERROR] Failed to create EVP_MD_CTX" << endl;
//         return "";
//     }

//     unsigned char hash[SHA256_DIGEST_LENGTH];
//     unsigned int hashLen;

//     if (EVP_DigestInit_ex(ctx, EVP_sha256(), NULL) != 1 ||
//         EVP_DigestUpdate(ctx, data.c_str(), data.size()) != 1 ||
//         EVP_DigestFinal_ex(ctx, hash, &hashLen) != 1) {
//         cerr << "[ERROR] SHA-256 computation failed" << endl;
//         EVP_MD_CTX_free(ctx);
//         return "";
//     }

//     EVP_MD_CTX_free(ctx);

//     stringstream hashString;
//     for (unsigned int i = 0; i < hashLen; i++) {
//         hashString << hex << setw(2) << setfill('0') << (int)hash[i];
//     }

//     return hashString.str();
// }
// std::string computeSHA256(const std::string &data) {// block_num + previous + block_header
//     EVP_MD_CTX *ctx = EVP_MD_CTX_new();  // 创建 EVP 上下文
//     if (!ctx) {
//         cerr << "[ERROR] Failed to create EVP_MD_CTX" << endl;
//         return "";
//     }

//     unsigned char hash[SHA256_DIGEST_LENGTH];
//     unsigned int hashLen;

//     // 初始化 SHA-256 计算
//     if (EVP_DigestInit_ex(ctx, EVP_sha256(), NULL) != 1 ||
//         EVP_DigestUpdate(ctx, data.c_str(), data.size()) != 1 ||
//         EVP_DigestFinal_ex(ctx, hash, &hashLen) != 1) {
//         cerr << "[ERROR] SHA256 computation failed" << endl;
//         EVP_MD_CTX_free(ctx);
//         return "";
//     }

//     EVP_MD_CTX_free(ctx); // 释放资源

//     // 转换 hash 值为十六进制字符串
//     stringstream hashString;
//     for (unsigned int i = 0; i < hashLen; i++) {
//         hashString << hex << setw(2) << setfill('0') << (int)hash[i];
//     }

//     return hashString.str();
// }

// 从区块链 API 获取区块哈希值
std::string getBlockHashFromAPI(int blockNum) {
    cout << "[INFO] Fetching block hash from blockchain for block " << blockNum << "..." << endl;

    string url = "https://eos.greymass.com/v1/chain/get_block";
    string postData = "{\"block_num_or_id\": " + to_string(blockNum) + "}";

    Json::Value jsonData = fetchJsonData(url, postData);
    
    if (jsonData.isMember("id")) {
        cout << "[INFO] Received block hash: " << jsonData["id"].asString() << endl;
        return jsonData["id"].asString();
    } else {
        cerr << "[ERROR] Blockchain API response does not contain block hash" << endl;
        return "";
    }
}

// 验证 JSON 备份文件中的区块
// void validateBackupFile(const std::string &backupFilePath) {
//     cout << "[INFO] Opening backup file: " << backupFilePath << endl;

//     ifstream file(backupFilePath);
//     if (!file) {
//         cerr << "[ERROR] Cannot open backup file: " << backupFilePath << endl;
//         return;
//     }

//     Json::Reader reader;
//     Json::Value root;
//     if (!reader.parse(file, root)) {
//         cerr << "[ERROR] Failed to parse JSON file" << endl;
//         return;
//     }

//     cout << "[INFO] Processing " << root.size() << " blocks..." << endl;

//     for (const auto &block : root) {
//         int blockNum = block["block_num"].asInt();
//         string blockIdFromJson = block["id"].asString();
        
//         cout << "[INFO] Processing block number: " << blockNum << endl;
//         cout << "[DEBUG] Local block ID from JSON: " << blockIdFromJson << endl;

//         // 获取区块链上的哈希值
//         string blockchainHash = getBlockHashFromAPI(blockNum);
//         if (blockchainHash.empty()) {
//             cerr << "[WARNING] Failed to fetch block hash from blockchain for block " << blockNum << endl;
//             continue;
//         }

//         // 计算本地哈希值
//         string blockData = block.toStyledString();
//         string computedHash = computeSHA256(blockData);
//         cout << "[DEBUG] Computed hash: " << computedHash << endl;

//         // 进行比对
//         if (computedHash == blockchainHash) {
//             cout << "[INFO] Block " << blockNum << " is valid." << endl;
//         } else {
//             cerr << "[ERROR] Block " << blockNum << " hash mismatch!" << endl;
//             cerr << "[ERROR] Expected: " << blockchainHash << endl;
//             cerr << "[ERROR] Computed: " << computedHash << endl;
//         }
//     }

//     cout << "[INFO] Backup validation completed!" << endl;
// }
void validateBackupFile(const std::string &backupFilePath) {
    cout << "[INFO] Opening backup file: " << backupFilePath << endl;

    ifstream file(backupFilePath);
    if (!file) {
        cerr << "[ERROR] Cannot open backup file: " << backupFilePath << endl;
        return;
    }

    Json::Reader reader;
    Json::Value root;
    if (!reader.parse(file, root)) {
        cerr << "[ERROR] Failed to parse JSON file" << endl;
        return;
    }

    cout << "[INFO] Processing " << root.size() << " blocks..." << endl;

    for (const auto &block : root) {
        int blockNum = block["block_num"].asInt();
        // string blockIdFromJson = block["id"].asString();
        string blockIdFromJson = block["id"].asString(); // 直接获取 JSON 文件中的 hash 值

        cout << "[INFO] Processing block number: " << blockNum << endl;
        cout << "[DEBUG] Local block ID from JSON: " << blockIdFromJson << endl;

        // 计算哈希值：仅使用 previous + transaction_mroot + action_mroot
        // string blockHeaderData = block["previous"].asString() +
        //                          block["transaction_mroot"].asString() +
        //                          block["action_mroot"].asString();

        // cout << "[DEBUG] Constructing block hash input: " << blockHeaderData << endl;
        // string computedHash = computeSHA256(blockHeaderData);
        // cout << "[DEBUG] Computed hash: " << computedHash << endl;

        // 获取 API 返回的哈希值
        string blockchainHash = getBlockHashFromAPI(blockNum);
        if (blockchainHash.empty()) {
            cerr << "[ERROR] Failed to fetch block hash from blockchain for block " << blockNum << endl;
            continue;
        }
        cout << "[INFO] Received block hash: " << blockchainHash << endl;

        if (blockchainHash.empty()) {
            cerr << "[WARNING] Failed to fetch block hash from blockchain for block " << blockNum << endl;
            continue;
        }

        // 进行哈希对比
    //     if (computedHash == blockchainHash) {
    //         cout << "[INFO] Block " << blockNum << " is valid." << endl;
    //     } else {
    //         cerr << "[ERROR] Block " << blockNum << " hash mismatch!" << endl;
    //         cerr << "[ERROR] Expected: " << blockchainHash << endl;
    //         cerr << "[ERROR] Computed: " << computedHash << endl;
    //     }
    // }
        // 直接比对 JSON 文件中的 `id` 和 API 获取的 `id`
        if (blockIdFromJson == blockchainHash) {
            cout << "[INFO] Block " << blockNum << " is valid." << endl;
        } else {
            cerr << "[ERROR] Block " << blockNum << " hash mismatch!" << endl;
            cerr << "[ERROR] Expected: " << blockchainHash << endl;
            cerr << "[ERROR] Local ID: " << blockIdFromJson << endl;
        }
    }

    cout << "[INFO] Backup validation completed!" << endl;
}