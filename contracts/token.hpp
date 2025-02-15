#pragma once

#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>

using namespace eosio;
using namespace std;

CONTRACT token : public contract {
public:
    using contract::contract;

    [[eosio::action]]
    void create(name issuer, asset maximum_supply);

    [[eosio::action]]
    void issue(name to, asset quantity, string memo);

    [[eosio::action]]
    void transfer(name from, name to, asset quantity, string memo);

private:
    TABLE account {
        asset balance;
        uint64_t primary_key() const { return balance.symbol.code().raw(); }
    };

    TABLE currency_stats {
        asset supply;
        asset max_supply;
        name issuer;
        uint64_t primary_key() const { return supply.symbol.code().raw(); }
    };

    typedef multi_index<"accounts"_n, account> accounts;
    typedef multi_index<"stat"_n, currency_stats> stats;

    void sub_balance(name owner, asset value);
    void add_balance(name owner, asset value, name ram_payer);
};
