#include "transaction_validation.h"
#include <iostream>
#include <fstream>
#include <curl/curl.h>

TransactionValidator::TransactionValidator(const std::string& snapshotPath, const std::string& eosApiUrl)
    : snapshotPath(snapshotPath), eosApiUrl(eosApiUrl) {}

std::vector<Transaction> TransactionValidator::parseTransactions() {
    std::vector<Transaction> transactions;
    std::ifstream file(snapshotPath);
    Json::Value root;
    file >> root;

    for (const auto& tx : root["transactions"]) {
        transactions.push_back({
            tx["tx_id"].asString(),
            tx["from"].asString(),
            tx["to"].asString(),
            tx["quantity"].asString(),
            tx["memo"].asString(),
            tx["is_inline"].asBool()
        });
    }
    return transactions;
}

std::string TransactionValidator::getTransactionHashFromChain(const std::string& tx_id) {
    CURL* curl = curl_easy_init();
    std::string response;

    if (curl) {
        std::string url = eosApiUrl + "/v1/history/get_transaction?id=" + tx_id;
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
    if (reader.parse(response, jsonData) && jsonData.isMember("id")) {
        return jsonData["id"].asString();
    }
    return "";
}

void TransactionValidator::validateTransactions() {
    auto transactions = parseTransactions();
    for (const auto& tx : transactions) {
        std::string hashFromChain = getTransactionHashFromChain(tx.tx_id);
        if (hashFromChain.empty() || hashFromChain != tx.tx_id) {
            std::cerr << "Transaction mismatch: " << tx.tx_id << std::endl;
        }
    }
}