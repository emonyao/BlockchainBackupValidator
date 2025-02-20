#ifndef ACCOUNT_VALIDATION_H
#define ACCOUNT_VALIDATION_H

#include <string>
#include <set>
#include <json/json.h>

class AccountValidator {
private:
    std::string snapshotPath;

public:
    // 构造函数声明
    AccountValidator(const std::string& snapshotPath);

    // 方法声明
    std::set<std::string> parseAccounts();
    void validateAccounts();
};

#endif // ACCOUNT_VALIDATION_H