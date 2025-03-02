#include "hash_validation.h"
#include "eos_api.h"  // 用于 API 访问
#include <iostream>
#include <fstream>
#include <sstream>
#include <openssl/sha.h>
#include <mutex>
#include <json/json.h>
#include <curl/curl.h>
#include <iomanip>
#include <openssl/evp.h>
#include "http_utils.h" 
#include <future>
#include <vector>
#include <semaphore>    // C++20

using namespace std;

static once_flag curl_global_init_flag;
static mutex global_cout_mutex;  // 用于保护标准输出流
static mutex global_cerr_mutex;  // 用于保护错误输出流


// 发送 HTTP 请求并解析 JSON
Json::Value fetchJsonData(const string &url, const string &postData = "") {
    call_once(curl_global_init_flag, [] {
        curl_global_init(CURL_GLOBAL_DEFAULT);
    });

    CURL *curl = curl_easy_init();
    if (!curl) {
        lock_guard<mutex> lock(global_cerr_mutex);
        cerr << "[ERROR] CURL 初始化失败！" << endl;
        return Json::Value();
    }

    string response;
    struct curl_slist *headers = nullptr;
    CURLcode res = CURLE_OK;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);

    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    if (!postData.empty()) {
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());
    }

    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        lock_guard<mutex> lock(global_cerr_mutex);
        cerr << "[ERROR] HTTP 请求失败..." << endl;
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
        return Json::Value();  // 直接返回，避免后续解析
    }

    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);

    // 解析 JSON
    Json::Value jsonData;
    Json::Reader reader;
    if (!reader.parse(response, jsonData)) {
        lock_guard<mutex> lock(global_cerr_mutex);
        cerr << "[ERROR] JSON 解析失败" << endl;
        return Json::Value();
    }

    return jsonData;
}

// 从区块链 API 获取区块哈希值
std::string getBlockHashFromAPI(int blockNum) {

    // 线程安全输出开始信息
    string localBlockHash;
    {
        lock_guard<std::mutex> lock(global_cerr_mutex);
        cout << "[INFO] Fetching block hash from blockchain for block " << blockNum << "..." << std::endl;
    }

    const string url = "https://eos.greymass.com/v1/chain/get_block";
    const string postData = "{\"block_num_or_id\": " + std::to_string(blockNum) + "}";

    Json::Value jsonData = fetchJsonData(url, postData);
    
    if (jsonData.isMember("id")) {
        {
            lock_guard<mutex> lock(global_cout_mutex);
            cout << "[INFO] Received block hash: " << jsonData["id"].asString() << endl;
        }
        localBlockHash = jsonData["id"].asString();
    } else {
        {
            lock_guard<mutex> lock(global_cerr_mutex);
            cerr << "[ERROR] Blockchain API response does not contain block hash" << endl;
        }
        localBlockHash.clear();
    }

    return localBlockHash;
}

void validateBackupFile(const string &backupFilePath) {
    {
        lock_guard<mutex> lock(global_cout_mutex);
        cout << "[INFO] Opening backup file: " << backupFilePath << endl;
    }

    ifstream file(backupFilePath);
    if (!file) {
        lock_guard<mutex> lock(global_cerr_mutex);
        cerr << "[ERROR] Cannot open backup file: " << backupFilePath << endl;
        return;
    }

    Json::Reader reader;
    Json::Value root;
    if (!reader.parse(file, root)) {
        lock_guard<mutex> lock(global_cerr_mutex);
        cerr << "[ERROR] Failed to parse JSON file" << endl;
        return;
    }

    {
        lock_guard<mutex> lock(global_cout_mutex);
        cout << "[INFO] Processing " << root.size() << " blocks..." << endl;
    }

    constexpr int MAX_CONCURRENT_TASKS = 4;
    counting_semaphore<MAX_CONCURRENT_TASKS> semaphore(MAX_CONCURRENT_TASKS);

    vector<future<void>> futures;// 并行处理每个块
    for (const auto &block : root) {
        futures.emplace_back(async(launch::async, [block, &semaphore] {
            // RAII方式获取信号量，确保任务结束后自动释放
            semaphore.acquire();
            // 使用 unique_ptr 模拟简单的 RAII（任务结束时自动调用释放）
            auto guard = unique_ptr<void, function<void(void*)>>(nullptr, [&semaphore](void*) {
                semaphore.release();
            });
            try{
                    const int blockNum = block["block_num"].asInt();
                    const string blockIdFromJson = block["id"].asString(); // 直接获取 JSON 文件中的 hash 值

                    {
                        lock_guard<mutex> lock(global_cout_mutex);
                        cout << "[INFO] Processing block number: " << blockNum << endl;
                        cout << "[DEBUG] Local block ID from JSON: " << blockIdFromJson << endl;
                    }

                    // 获取 API 返回的哈希值
                    const string blockchainHash = getBlockHashFromAPI(blockNum);

                    if (blockchainHash.empty()) {
                        lock_guard<mutex> lock(global_cerr_mutex);
                        cerr << "[ERROR] Failed to fetch block hash from blockchain for block " << blockNum << endl;
                        return;
                    }

                    {
                        lock_guard<mutex> lock(global_cout_mutex);
                        cout << "[INFO] Received block hash: " << blockchainHash << endl;
                    }

                    if (blockIdFromJson != blockchainHash) {
                        lock_guard<mutex> lock(global_cerr_mutex);
                        cerr << "[ERROR] Block " << blockNum << " hash mismatch!\n"
                                << "[ERROR] Expected: " << blockchainHash << "\n"
                                << "[ERROR] Local ID: " << blockIdFromJson << endl;
                    } else {
                        lock_guard<mutex> lock(global_cout_mutex);
                        cout << "[INFO] Block " << blockNum << " is valid." << endl;
                    }
            }catch (const exception &e) {
                lock_guard<mutex> lock(global_cerr_mutex);
                cerr << "[EXCEPTION] Error processing block: " << e.what() << endl;
            }
        }));
    }

    for (auto &future : futures) {
        future.get();
    }
    
    {
        lock_guard<mutex> lock(global_cout_mutex);
        cout << "[INFO] Backup validation completed!" << endl;
    }
}