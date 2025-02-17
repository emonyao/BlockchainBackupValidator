#ifndef ACCOUNT_VALIDATION_H
#define ACCOUNT_VALIDATION_H

#include <string>
#include <vector>
#include <json/json.h>

struct Account {
    std::string name;
    double balance;
};

class AccountValidator {
public:
    AccountValidator(const std::string& snapshotPath, const std::string& eosApiUrl);
    void validateAccounts();

private:
    std::string snapshotPath;
    std::string eosApiUrl;
    std::vector<Account> parseAccounts();
    double getAccountBalanceFromChain(const std::string& account_name);
};

#endif // ACCOUNT_VALIDATION_H