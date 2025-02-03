#include <iostream>
#include <fstream>
#include <vector>
#include <json/json.h>
#include <openssl/evp.h>  // 替换 SHA.h，使用 EVP API 计算 SHA-256

const size_t BUFFER_SIZE = 4096;  // 每次读取 4KB
const size_t MAX_BLOCKS = 10;     // 只解析前 10 个区块

std::string sha256(const std::vector<unsigned char> &data) {
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hash_len = 0;
    
    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    if (!mdctx) {
        std::cerr << "EVP_MD_CTX_new() 失败！" << std::endl;
        return "";
    }

    if (EVP_DigestInit_ex(mdctx, EVP_sha256(), nullptr) != 1 ||
        EVP_DigestUpdate(mdctx, data.data(), data.size()) != 1 ||
        EVP_DigestFinal_ex(mdctx, hash, &hash_len) != 1) {
        std::cerr << "EVP API 计算 SHA-256 失败！" << std::endl;
        EVP_MD_CTX_free(mdctx);
        return "";
    }

    EVP_MD_CTX_free(mdctx);

    char outputBuffer[65];  // 64 字符 + 终止符
    for (unsigned int i = 0; i < hash_len; i++) {
        sprintf(outputBuffer + (i * 2), "%02x", hash[i]);
    }
    outputBuffer[64] = 0;
    return std::string(outputBuffer);
}

void parse_bin_file(const std::string &filepath, const std::string &output_json) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file) {
        std::cerr << "❌ 无法打开文件: " << filepath << std::endl;
        return;
    }

    std::ofstream json_file(output_json);
    if (!json_file) {
        std::cerr << "❌ 无法创建 JSON 输出文件: " << output_json << std::endl;
        return;
    }

    Json::Value root;
    Json::StreamWriterBuilder writer;
    writer["indentation"] = "    ";

    std::vector<unsigned char> buffer(BUFFER_SIZE);
    size_t block_count = 0;

    while (file.read(reinterpret_cast<char *>(buffer.data()), BUFFER_SIZE) && block_count < MAX_BLOCKS) {
        Json::Value block;
        block["block_id"] = static_cast<int>(block_count);
        block["hash"] = sha256(buffer);  // 计算 SHA-256 哈希值
        block["data"] = Json::Value(Json::arrayValue);  // 先留空，后续填充具体数据

        root["blocks"].append(block);
        block_count++;
    }

    json_file << Json::writeString(writer, root);
    file.close();
    json_file.close();

    std::cout << "✅ 解析完成！已输出 JSON: " << output_json << std::endl;
}

int main() {
    std::string bin_path = "data/snapshot.bin";   // 固定 .bin 文件路径
    std::string json_path = "data/output.json";   // 固定 JSON 输出路径

    parse_bin_file(bin_path, json_path);

    return 0;
}
