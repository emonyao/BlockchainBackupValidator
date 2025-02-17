#ifndef MAIN_H
#define MAIN_H

#include <string>
#include "transaction_validation.h"
#include "block_link_validation.h"
#include "account_validation.h"

class BlockchainBackupValidator {
public:
    BlockchainBackupValidator(const std::string& snapshotPath, const std::string& eosApiUrl);
    void runValidation();

private:
    std::string snapshotPath;
    std::string eosApiUrl;
};

#endif // MAIN_H