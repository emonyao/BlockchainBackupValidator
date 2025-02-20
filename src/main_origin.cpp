#include "main.h"
#include <iostream>
#include <fstream>

BlockchainBackupValidator::BlockchainBackupValidator(const std::string& snapshotPath, const std::string& eosApiUrl)
    : snapshotPath(snapshotPath), eosApiUrl(eosApiUrl) {}

void BlockchainBackupValidator::runValidation() {
    std::cout << "Starting blockchain backup validation...\n";

    // 交易数据校验
    std::cout << "Validating transactions...\n";
    TransactionValidator txValidator(snapshotPath, eosApiUrl);
    txValidator.validateTransactions();

    // 区块链接关系验证
    std::cout << "Validating block links...\n";
    BlockLinkValidator blockValidator(snapshotPath);
    blockValidator.validateBlockLinks();

    // 关键账户数据抽样验证
    std::cout << "Validating account data...\n";
    AccountValidator accountValidator(snapshotPath, eosApiUrl);
    accountValidator.validateAccounts();

    std::cout << "Validation completed.\n";
}

// 主函数
int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <snapshot.json> <eos_api_url>\n";
        return 1;
    }

    std::string snapshotPath = argv[1];
    std::string eosApiUrl = argv[2];

    BlockchainBackupValidator validator(snapshotPath, eosApiUrl);
    validator.runValidation();

    return 0;
}