#include "hash_validation.h"
#include "block_link_validation.h"
// #include "account_validation.h"
#include "transaction_validation.h"
#include <iostream>

using namespace std;

void printUsage() {
    cout << "Usage: blockchain_validator <backup_json_file>" << endl;
}

int main() {
    // hash validation
    cout << "======================================" << endl;
    cout << "Blockchain Backup Hash Validator" << endl;
    cout << "======================================" << endl;

    // 选择要验证的备份文件
    string backupFile = "src/data/test_correct.json";  //
    cout << "[INFO] Selected backup file: " << backupFile << endl;

    // 运行前检查路径
    if (backupFile.empty()) {
        cerr << "[ERROR] Backup file path is empty!" << endl;
        return 1;
    }

    // 开始验证
    cout << "[INFO] Starting backup validation for: " << backupFile << endl;
    validateBackupFile(backupFile);

    // 结束信息
    cout << "[INFO] Backup validation completed!" << endl;
    cout << "======================================" << endl;


    // block link validation
    // 选择要验证的区块链快照文件
    // string backupFile = "src/data/test_correct.json";

    cout << "======================================" << endl;
    cout << " Blockchain Block Link Validator" << endl;
    cout << "======================================" << endl;

    // 创建区块验证器对象
    BlockLinkValidator validator(backupFile);

    // 执行区块链接验证
    cout << "[INFO] Starting block link validation for: " << backupFile << endl;
    validator.validateBlockLinks();
    
    cout << "[INFO] Block link validation finished!" << endl;
    cout << "======================================" << endl;



    // account validation
    // string snapshotFile = "src/data/test_correct.json";  // JSON 备份文件
    // cout << "======================================" << endl;
    // cout << " Blockchain account validation" << endl;
    // cout << "======================================" << endl;
    // cout << "[INFO] Starting account validation for: " << snapshotFile << endl;

    // AccountValidator validator(snapshotFile);
    // validator.validateAccounts();


    // transaction validation
    cout << "======================================" << endl;
    cout << " Blockchain transaction validation" << endl;
    cout << "======================================" << endl;
    // string backupFile = "src/data/test_correct.json";
    validateTransactions(backupFile);
    
    return 0;
}