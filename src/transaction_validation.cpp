#include "transaction_validation.h"
#include <iostream>
#include <fstream>
#include <json/json.h>
#include <curl/curl.h>
#include <set>
#include <sstream>
#include "http_utils.h" 

using namespace std;

// 读取 JSON 文件
bool readJsonFile(const std::string &filePath, Json::Value &jsonData) {
    std::ifstream file(filePath, std::ifstream::binary);
    if (!file) {
        std::cerr << "[ERROR] Failed to open JSON file: " << filePath << std::endl;
        return false;
    }

    Json::CharReaderBuilder reader;
    std::string errs;
    if (!Json::parseFromStream(reader, file, &jsonData, &errs)) {
        std::cerr << "[ERROR] Failed to parse JSON file: " << errs << std::endl;
        return false;
    }

    std::cout << "[INFO] Successfully loaded JSON file: " << filePath << std::endl;
    return true;
}

// 提取 block_num
std::vector<int> extractBlockNumbers(const Json::Value& backupJson) {
    std::vector<int> blockNumbers;

    for (const auto& block : backupJson) {
        if (block.isMember("block_num")) {
            int blockNum = block["block_num"].asInt();
            blockNumbers.push_back(blockNum);
            std::cout << "[DEBUG] Found block number: " << blockNum << std::endl;
        }
    }

    return blockNumbers;
}

// 发送 HTTP 请求的回调函数
// size_t WriteCallback(void *contents, size_t size, size_t nmemb, std::string *output) {
//     size_t totalSize = size * nmemb;
//     output->append((char *)contents, totalSize);
//     return totalSize;
// }

// 通过 API 获取交易信息
Json::Value getTransactionsFromBlockchain(int blockNum) {
    std::string apiUrl = "https://eos.greymass.com/v1/chain/get_block";
    std::string jsonPayload = "{\"block_num_or_id\": " + std::to_string(blockNum) + "}";

    CURL *curl = curl_easy_init();
    if (!curl) {
        std::cerr << "[ERROR] Failed to initialize CURL" << std::endl;
        return Json::Value();
    }

    std::string response;
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, apiUrl.c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonPayload.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);

    if (res != CURLE_OK) {
        std::cerr << "[ERROR] Failed to fetch blockchain transactions: " << curl_easy_strerror(res) << std::endl;
        return Json::Value();
    }

    Json::Value jsonData;
    Json::CharReaderBuilder reader;
    std::string errs;
    std::istringstream s(response);
    if (!Json::parseFromStream(reader, s, &jsonData, &errs)) {
        std::cerr << "[ERROR] Failed to parse blockchain response: " << errs << std::endl;
        return Json::Value();
    }

    return jsonData;
}

// 从备份 JSON 文件提取交易 ID
std::set<std::string> extractTransactionsFromBackup(const Json::Value& backupJson) {
    std::set<std::string> transactions;

    for (const auto& block : backupJson) {
        if (block.isMember("transactions")) {
            for (const auto& txn : block["transactions"]) {
                if (txn.isMember("trx") && txn["trx"].isMember("id")) {
                    std::string txnId = txn["trx"]["id"].asString();
                    transactions.insert(txnId);
                }
            }
        }
    }

    std::cout << "[INFO] Extracted " << transactions.size() << " transactions from backup file." << std::endl;
    return transactions;
}

// 通过 API 查询单笔交易是否存在
bool isTransactionOnBlockchain(const std::string& transactionId) {
    std::string apiUrl = "https://eos.greymass.com/v1/history/get_transaction";
    std::string jsonPayload = "{\"id\": \"" + transactionId + "\"}";

    CURL *curl = curl_easy_init();
    if (!curl) {
        std::cerr << "[ERROR] Failed to initialize CURL" << std::endl;
        return false;
    }

    std::string response;
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, apiUrl.c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonPayload.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);

    if (res != CURLE_OK) {
        std::cerr << "[ERROR] Failed to fetch transaction " << transactionId << ": " << curl_easy_strerror(res) << std::endl;
        return false;
    }

    Json::Value jsonData;
    Json::CharReaderBuilder reader;
    std::string errs;
    std::istringstream s(response);
    if (!Json::parseFromStream(reader, s, &jsonData, &errs)) {
        std::cerr << "[ERROR] Failed to parse blockchain response for transaction " << transactionId << std::endl;
        return false;
    }

    return jsonData.isMember("id");  // API 返回 ID 说明交易存在
}

// 交易验证主函数
// void validateTransactions(const std::string& backupFilePath) {
//     std::cout << "[INFO] Starting transaction validation for: " << backupFilePath << std::endl;

//     // 读取备份 JSON
//     Json::Value backupJson;
//     if (!readJsonFile(backupFilePath, backupJson)) {
//         std::cerr << "[ERROR] Failed to open backup file: " << backupFilePath << std::endl;
//         return;
//     }

//     // 提取 block_num
//     std::vector<int> blockNumbers = extractBlockNumbers(backupJson);
//     if (blockNumbers.empty()) {
//         std::cerr << "[ERROR] No block numbers found in backup file." << std::endl;
//         return;
//     }

//     // 提取交易 ID
//     std::set<std::string> backupTransactions = extractTransactionsFromBackup(backupJson);

//     // 遍历每个 block_num 并获取区块链交易数据
//     for (int blockNum : blockNumbers) {
//         std::cout << "[INFO] Fetching transactions for block: " << blockNum << std::endl;
//         Json::Value blockchainData = getTransactionsFromBlockchain(blockNum);

//         if (blockchainData.isNull()) {
//             std::cerr << "[ERROR] Failed to fetch transactions for block: " << blockNum << std::endl;
//             continue;
//         }

//         std::set<std::string> blockchainTransactions;
//         if (blockchainData.isMember("transactions")) {
//             for (const auto& txn : blockchainData["transactions"]) {
//                 if (txn.isMember("trx") && txn["trx"].isMember("id")) {
//                     std::string txnId = txn["trx"]["id"].asString();
//                     blockchainTransactions.insert(txnId);
//                 }
//             }
//         }

//         // 比较交易
//         for (const auto& txnId : backupTransactions) {
//             if (blockchainTransactions.find(txnId) != blockchainTransactions.end()) {
//                 std::cout << "[INFO] Transaction matched: " << txnId << std::endl;
//             } else {
//                 std::cerr << "[ERROR] Transaction missing on blockchain: " << txnId << std::endl;
//             }
//         }
//     }

//     std::cout << "[INFO] Transaction validation completed!" << std::endl;
// }
void validateTransactions(const std::string& backupFilePath) {
    std::cout << "[INFO] Starting transaction validation for: " << backupFilePath << std::endl;

    // 读取 JSON 备份
    Json::Value backupJson;
    if (!readJsonFile(backupFilePath, backupJson)) {
        std::cerr << "[ERROR] Failed to open backup file: " << backupFilePath << std::endl;
        return;
    }

    // 提取交易 ID
    std::set<std::string> backupTransactions = extractTransactionsFromBackup(backupJson);

    std::cout << "[INFO] Fetching blockchain transactions from API..." << std::endl;

    // 逐个查询链上是否存在交易
    for (const auto& txnId : backupTransactions) {
        std::cout << "[INFO] Checking transaction: " << txnId << "..." << std::endl;

        if (isTransactionOnBlockchain(txnId)) {
            std::cout << "[INFO] Transaction matched: " << txnId << std::endl;
        } else {
            std::cerr << "[ERROR] Transaction missing on blockchain: " << txnId << std::endl;
        }
    }

    std::cout << "[INFO] Transaction validation completed!" << std::endl;
}