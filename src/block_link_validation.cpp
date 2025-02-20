#include "block_link_validation.h"
#include <iostream>
#include <fstream>
#include <json/json.h>

using namespace std;

BlockLinkValidator::BlockLinkValidator(const string& snapshotPath) : snapshotPath(snapshotPath) {}

// 解析 JSON 文件，获取所有区块
vector<Block> BlockLinkValidator::parseBlocks() {
    vector<Block> blocks;
    ifstream file(snapshotPath);
    if (!file) {
        cerr << "[ERROR] Cannot open snapshot file: " << snapshotPath << endl;
        return blocks;  // 避免崩溃
    }

    Json::Value root;
    Json::Reader reader;
    if (!reader.parse(file, root)) {
        cerr << "[ERROR] Failed to parse JSON file: " << snapshotPath << endl;
        return blocks;
    }

    if (!root.isArray()) {  //JSON 以数组形式存储多个区块
        cerr << "[ERROR] JSON format error: Root is not an array" << endl;
        return blocks;
    }

    cout << "[INFO] JSON file parsed successfully. Total blocks found: " << root.size() << endl;

    for (const auto& blk : root) {
        if (!blk.isMember("id") || !blk.isMember("previous") || !blk.isMember("block_num")) {
            cerr << "[WARNING] Skipping block with missing fields" << endl;
            continue;
        }

        Block block;
        block.block_id = blk["block_num"].asUInt();
        block.hash = blk["id"].asString();
        block.previous_hash = blk["previous"].asString();

        blocks.push_back(block);
        cout << "[DEBUG] Parsed block: " << block.block_id << " | Hash: " << block.hash
             << " | Previous: " << block.previous_hash << endl;
    }

    return blocks;
}

// 验证区块是否正确连接
void BlockLinkValidator::validateBlockLinks() {
    auto blocks = parseBlocks();

    if (blocks.empty()) {
        cerr << "[ERROR] No blocks found in snapshot." << endl;
        return;
    }

    cout << "[INFO] Validating " << blocks.size() << " blocks..." << endl;

    for (size_t i = 1; i < blocks.size(); ++i) {
        cout << "[DEBUG] Checking link: Block " << blocks[i - 1].block_id
             << " Block " << blocks[i].block_id << endl;
        cout << "[DEBUG] Expected previous hash: " << blocks[i - 1].hash << endl;
        cout << "[DEBUG] Found previous hash: " << blocks[i].previous_hash << endl;

        if (blocks[i].previous_hash != blocks[i - 1].hash) {
            cerr << "[ERROR] Block link broken at block " << blocks[i].block_id << endl;
            cerr << "[DEBUG] Expected previous hash: " << blocks[i - 1].hash << endl;
            cerr << "[DEBUG] Found: " << blocks[i].previous_hash << endl;
        } else {
            cout << "[INFO] Block " << blocks[i].block_id << " linked correctly." << endl;
        }
    }

    cout << "[INFO] Block link validation completed!" << endl;
}