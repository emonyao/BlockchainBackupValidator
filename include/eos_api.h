#ifndef EOS_API_H
#define EOS_API_H

#include <string>

// 从 EOS 区块链获取原始文件的哈希值
std::string getOriginalFileHash(const std::string &contract, const std::string &table, const std::string &scope);

#endif // EOS_API_H
