#ifndef BLOCK_LINK_VALIDATION_H
#define BLOCK_LINK_VALIDATION_H

#include <string>
#include <vector>
#include <json/json.h>

struct Block {
    int block_id;
    std::string hash;
    std::string previous_hash;
};

class BlockLinkValidator {
public:
    BlockLinkValidator(const std::string& snapshotPath);
    void validateBlockLinks();

private:
    std::string snapshotPath;
    std::vector<Block> parseBlocks();
};

#endif // BLOCK_LINK_VALIDATION_H