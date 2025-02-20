#ifndef HASH_VALIDATION_H
#define HASH_VALIDATION_H

#include <string>
#include <json/json.h>

// Compute the SHA-256 hash of a given string
std::string computeSHA256(const std::string &data);

// Validate the integrity of the blocks in a backup JSON file
void validateBackupFile(const std::string &backupFilePath);

// Fetch the block hash from the blockchain API
std::string getBlockHashFromAPI(int blockNum);

#endif // HASH_VALIDATION_H