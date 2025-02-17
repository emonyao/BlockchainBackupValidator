#ifndef TRANSACTION_VALIDATION_H
#define TRANSACTION_VALIDATION_H

#include <string>
#include <vector>
#include <json/json.h>

struct Transaction {
    std::string tx_id;
    std::string from;
    std::string to;
    std::string quantity;
    std::string memo;
    bool is_inline;
};

class TransactionValidator {
public:
    TransactionValidator(const std::string& snapshotPath, const std::string& eosApiUrl);
    void validateTransactions();

private:
    std::string snapshotPath;
    std::string eosApiUrl;
    std::vector<Transaction> parseTransactions();
    std::string getTransactionHashFromChain(const std::string& tx_id);
};

#endif // TRANSACTION_VALIDATION_H