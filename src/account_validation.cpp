#include "account_validation.h"
#include <iostream>
#include <fstream>
#include <curl/curl.h>

AccountValidator::AccountValidator(const std::string& snapshotPath, const std::string& eosApiUrl)
    : snapshotPath(snapshotPath), eosApiUrl(eosApiUrl) {}

std::vector<Account> AccountValidator::parseAccounts() {
    std::vector<Account> accounts;
    std::ifstream file(snapshotPath);
    Json::Value root;
    file >> root;

    for (const auto& acc : root["accounts"]) {
        accounts.push_back({
            acc["name"].asString(),
            acc["balance"].asDouble()
        });
    }
    return accounts;
}

double AccountValidator::getAccountBalanceFromChain(const std::string& account_name) {
    CURL* curl = curl_easy_init();
    std::string response;

    if (curl) {
        std::string url = eosApiUrl + "/v1/chain/get_currency_balance?account=" + account_name;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, +[](void* contents, size_t size, size_t nmemb, void* userp) -> size_t {
            ((std::string*)userp)->append((char*)contents, size * nmemb);
            return size * nmemb;
        });
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }

    Json::Value jsonData;
    Json::Reader reader;
    if (reader.parse(response, jsonData) && jsonData.size() > 0) {
        return std::stod(jsonData[0].asString());
    }
    return -1;
}

void AccountValidator::validateAccounts() {
    auto accounts = parseAccounts();
    for (const auto& acc : accounts) {
        double chainBalance = getAccountBalanceFromChain(acc.name);
        if (chainBalance < 0 || chainBalance != acc.balance) {
            std::cerr << "Account mismatch: " << acc.name << std::endl;
        }
    }
}