#ifndef HASH_VALIDATION_H
#define HASH_VALIDATION_H

#include <string>

std::string computeFileHash(const std::string &filepath);
bool validateFileIntegrityWithBlockchain(const std::string &backupFile, const std::string &contract, const std::string &table, const std::string &scope);

#endif