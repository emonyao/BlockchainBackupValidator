#include "hash_validation.h"
#include "eos_api.h"
#include <iostream>
#include <fstream>
#include <openssl/sha.h>

using namespace std;

// 计算本地文件的 SHA-256 哈希
std::string computeFileHash(const std::string &filepath) {
    ifstream file(filepath, ios::binary);
    if (!file) {
        cerr << "无法打开文件: " << filepath << endl;
        return "";
    }

    SHA256_CTX sha256;
    SHA256_Init(&sha256);

    char buffer[4096];
    while (file.read(buffer, sizeof(buffer))) {
        SHA256_Update(&sha256, buffer, file.gcount());
    }

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_Final(hash, &sha256);

    char hashStr[SHA256_DIGEST_LENGTH * 2 + 1];
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(&hashStr[i * 2], "%02x", hash[i]);
    }

    return std::string(hashStr);
}

// 验证本地文件完整性（与区块链数据对比）
bool validateFileIntegrityWithBlockchain(const std::string &backupFile, const std::string &contract, const std::string &table, const std::string &scope) {
    cout << "获取区块链上的文件哈希..." << endl;
    string originalHash = getOriginalFileHash(contract, table, scope);
    if (originalHash.empty()) {
        cerr << "无法获取原始文件哈希!" << endl;
        return false;
    }

    cout << "计算本地备份文件哈希..." << endl;
    string backupHash = computeFileHash(backupFile);
    if (backupHash.empty()) return false;

    cout << "进行哈希值比对..." << endl;
    if (originalHash == backupHash) {
        cout << "备份文件完整!" << endl;
        return true;
    } else {
        cerr << "备份文件可能已损坏或被篡改!" << endl;
        cerr << "区块链哈希: " << originalHash << endl;
        cerr << "本地文件哈希: " << backupHash << endl;
        return false;
    }
}