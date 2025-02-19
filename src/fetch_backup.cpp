#include <iostream>
#include <fstream>
#include <curl/curl.h>
#include <json/json.h>
#include <filesystem>

using namespace std;
namespace fs = filesystem;  // 处理文件路径

const string DATA_DIR = "data";  // 你的数据存放目录
const string OUTPUT_FILE = DATA_DIR + "/backup.json";  // JSON 文件路径

// 回调函数：用于接收 HTTP 响应数据
size_t WriteCallback(void *contents, size_t size, size_t nmemb, string *output) {
    size_t totalSize = size * nmemb;
    output->append((char *)contents, totalSize);
    return totalSize;
}

// 发送 HTTP 请求并返回 JSON 解析结果
Json::Value fetchJsonData(const string &url, const string &postData = "") {
    CURL *curl = curl_easy_init();
    if (!curl) {
        cerr << "CURL 初始化失败！" << endl;
        return Json::Value();
    }

    string response;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    if (!postData.empty()) {
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());
    }

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        cerr << "HTTP 请求失败: " << curl_easy_strerror(res) << endl;
        curl_easy_cleanup(curl);
        return Json::Value();
    }

    curl_easy_cleanup(curl);

    // 解析 JSON 数据
    Json::Value jsonData;
    Json::Reader reader;
    if (!reader.parse(response, jsonData)) {
        cerr << "JSON 解析失败" << endl;
        return Json::Value();
    }

    return jsonData;
}

// 获取最新区块号
int getLatestBlockNum() {
    string url = "https://eos.greymass.com/v1/chain/get_info";
    Json::Value jsonData = fetchJsonData(url);

    if (jsonData.isMember("head_block_num")) {
        return jsonData["head_block_num"].asInt();
    } else {
        cerr << "获取最新区块号失败！" << endl;
        return -1;
    }
}

// 获取指定区块的数据
Json::Value getBlockData(int blockNum) {
    string url = "https://eos.greymass.com/v1/chain/get_block";
    string postData = "{\"block_num_or_id\": " + to_string(blockNum) + "}";

    return fetchJsonData(url, postData);
}

int main() {
    try {
        // 设置全局 UTF-8 语言环境
        std::locale::global(std::locale("en_US.utf8"));

        // 确保 cin / cout 使用 UTF-8
        std::wcout.imbue(std::locale("en_US.utf8"));
        std::wcin.imbue(std::locale("en_US.utf8"));

        // 测试中文输出
        std::wcout << L"Test UTF-8 output\n\n";
    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    cout << "fetching latest block num..." << endl;
    int latestBlockNum = getLatestBlockNum();
    if (latestBlockNum == -1) return 1;

    cout << "latest block num: " << latestBlockNum << endl;
    int startBlock = latestBlockNum - 999;

    // 确保 `data` 目录存在
    if (!fs::exists(DATA_DIR)) {
        fs::create_directories(DATA_DIR);
    }

    ofstream outFile(OUTPUT_FILE);
    if (!outFile) {
        cerr << "can not create JSON file!" << endl;
        return 1;
    }

    Json::Value allBlocks(Json::arrayValue);

    for (int i = startBlock; i <= latestBlockNum; i++) {
        cout << "fetching data: " << i << endl;
        Json::Value blockData = getBlockData(i);
        if (!blockData.isNull()) {
            allBlocks.append(blockData);  // 添加所有区块数据
        }
    }

    outFile << allBlocks.toStyledString();  // 写入 JSON 文件
    outFile.close();

    cout << "finish fetching data and save as" << OUTPUT_FILE << endl;
    return 0;
}