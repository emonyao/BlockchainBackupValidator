#include "account_validation.h"
#include <iostream>
#include <fstream>
#include <json/json.h>
#include <set>
#include <curl/curl.h>
#include "http_utils.h"

using namespace std;

// 构造函数，接收 JSON 备份文件路径
AccountValidator::AccountValidator(const std::string& snapshotPath) : snapshotPath(snapshotPath) {}

// HTTP 回调函数：用于接收 API 响应数据
// size_t WriteCallback(void *contents, size_t size, size_t nmemb, string *output) {
//     size_t totalSize = size * nmemb;
//     output->append((char *)contents, totalSize);
//     return totalSize;
// }

// 发送 HTTP 请求并返回 JSON 数据
Json::Value fetchBlockchainAccounts() {
    string apiUrl = "https://eos.greymass.com/v1/chain/get_table_rows";
    string jsonPayload = R"({
        "json": true,
        "code": "eosio",
        "scope": "eosio",
        "table": "accounts",
        "limit": 1000
    })";

    CURL *curl = curl_easy_init();
    if (!curl) {
        cerr << "[ERROR] CURL 初始化失败!" << endl;
        return Json::Value();
    }

    string response;
    curl_easy_setopt(curl, CURLOPT_URL, apiUrl.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonPayload.c_str());

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        cerr << "[ERROR] HTTP 请求失败: " << curl_easy_strerror(res) << endl;
        curl_easy_cleanup(curl);
        return Json::Value();
    }

    curl_easy_cleanup(curl);

    // 解析 JSON 数据
    Json::Value jsonData;
    Json::Reader reader;
    if (!reader.parse(response, jsonData)) {
        cerr << "[ERROR] JSON 解析失败" << endl;
        return Json::Value();
    }

    return jsonData;
}

// 解析 JSON 文件并提取本地账户数据
set<string> AccountValidator::parseAccounts() {
    set<string> accounts;
    ifstream file(snapshotPath);
    
    if (!file) {
        cerr << "[ERROR] Cannot open snapshot file: " << snapshotPath << endl;
        return accounts;
    }

    Json::Value root;
    Json::Reader reader;
    if (!reader.parse(file, root)) {
        cerr << "[ERROR] Failed to parse JSON file: " << snapshotPath << endl;
        return accounts;
    }

    if (!root.isArray()) {
        cerr << "[ERROR] JSON format error: root is not an array" << endl;
        return accounts;
    }

    cout << "[INFO] Processing " << root.size() << " blocks..." << endl;

    // 遍历所有区块
    for (const auto& block : root) {
        if (!block.isMember("transactions") || !block["transactions"].isArray()) {
            cerr << "[WARNING] Block " << block["block_num"].asUInt() << " has no transactions." << endl;
            continue;
        }

        for (const auto& trx : block["transactions"]) {
            if (!trx.isMember("trx") || !trx["trx"].isMember("transaction") ||
                !trx["trx"]["transaction"].isMember("actions") ||
                !trx["trx"]["transaction"]["actions"].isArray()) {
                cerr << "[WARNING] Block " << block["block_num"].asUInt() << " has transactions without actions." << endl;
                continue;
            }

            for (const auto& action : trx["trx"]["transaction"]["actions"]) {
                if (action.isMember("account")) {
                    string account = action["account"].asString();
                    accounts.insert(account);
                    cout << "[DEBUG] Found account: " << account << " in block " << block["block_num"].asUInt() << endl;
                } else {
                    cerr << "[WARNING] Found action without 'account' field in block " << block["block_num"].asUInt() << endl;
                }
            }
        }
    }

    cout << "[INFO] Successfully extracted " << accounts.size() << " unique accounts from the snapshot." << endl;
    return accounts;
}

// 验证本地账户数据是否与区块链数据匹配
void AccountValidator::validateAccounts() {
    set<string> localAccounts = parseAccounts();
    
    if (localAccounts.empty()) {
        cerr << "[ERROR] No accounts found in snapshot." << endl;
        return;
    }

    cout << "[INFO] Fetching blockchain accounts from API..." << endl;
    Json::Value blockchainData = fetchBlockchainAccounts();
    if (!blockchainData.isMember("rows") || !blockchainData["rows"].isArray()) {
        cerr << "[ERROR] Blockchain API response is invalid!" << endl;
        return;
    }

    set<string> blockchainAccounts;
    for (const auto& row : blockchainData["rows"]) {
        if (row.isMember("account_name")) {
            blockchainAccounts.insert(row["account_name"].asString());
        }
    }

    cout << "[INFO] Blockchain accounts count: " << blockchainAccounts.size() << endl;

    // 开始比对
    for (const auto& account : localAccounts) {
        if (blockchainAccounts.count(account)) {
            cout << "[INFO] Account verified: " << account << endl;
        } else {
            cerr << "[ERROR] Account missing on blockchain: " << account << endl;
        }
    }

    cout << "[INFO] Account validation completed!" << endl;
}