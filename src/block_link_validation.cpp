#include "block_link_validation.h"
#include <iostream>
#include <fstream>

BlockLinkValidator::BlockLinkValidator(const std::string& snapshotPath) : snapshotPath(snapshotPath) {}

std::vector<Block> BlockLinkValidator::parseBlocks() {
    std::vector<Block> blocks;
    std::ifstream file(snapshotPath);
    Json::Value root;
    file >> root;

    for (const auto& blk : root["blocks"]) {
        blocks.push_back({
            blk["block_id"].asInt(),
            blk["hash"].asString(),
            blk["previous_hash"].asString()
        });
    }
    return blocks;
}

void BlockLinkValidator::validateBlockLinks() {
    auto blocks = parseBlocks();
    for (size_t i = 1; i < blocks.size(); ++i) {
        if (blocks[i].previous_hash != blocks[i - 1].hash) {
            std::cerr << "Block link broken at ID: " << blocks[i].block_id << std::endl;
        }
    }
}