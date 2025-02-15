#include "token.hpp"

void token::create(name issuer, asset maximum_supply) {
    require_auth(get_self());

    stats statstable(get_self(), maximum_supply.symbol.code().raw());
    auto existing = statstable.find(maximum_supply.symbol.code().raw());
    check(existing == statstable.end(), "Token already exists");

    statstable.emplace(get_self(), [&](auto& s) {
        s.supply = asset(0, maximum_supply.symbol);
        s.max_supply = maximum_supply;
        s.issuer = issuer;
    });
}

void token::issue(name to, asset quantity, string memo) {
    require_auth(get_self());

    stats statstable(get_self(), quantity.symbol.code().raw());
    auto existing = statstable.find(quantity.symbol.code().raw());
    check(existing != statstable.end(), "Token does not exist");

    statstable.modify(existing, same_payer, [&](auto& s) {
        s.supply += quantity;
    });

    add_balance(to, quantity, get_self());
}

void token::transfer(name from, name to, asset quantity, string memo) {
    require_auth(from);
    check(from != to, "Cannot transfer to self");

    sub_balance(from, quantity);
    add_balance(to, quantity, from);
}

void token::sub_balance(name owner, asset value) {
    accounts from_acnts(get_self(), owner.value);
    auto it = from_acnts.find(value.symbol.code().raw());
    check(it != from_acnts.end(), "Insufficient balance");
    from_acnts.modify(it, owner, [&](auto& a) {
        a.balance -= value;
    });
}

void token::add_balance(name owner, asset value, name ram_payer) {
    accounts to_acnts(get_self(), owner.value);
    auto it = to_acnts.find(value.symbol.code().raw());
    if (it == to_acnts.end()) {
        to_acnts.emplace(ram_payer, [&](auto& a) {
            a.balance = value;
        });
    } else {
        to_acnts.modify(it, same_payer, [&](auto& a) {
            a.balance += value;
        });
    }
}
