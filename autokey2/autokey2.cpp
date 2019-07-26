#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>

using namespace eosio;

CONTRACT autokey: public contract {
    public:
        using contract::contract;

        ACTION dummy() {}
        [[eosio::on_notify("eosio.token::transfer")]]
        void ontransfer(name from, name to, asset quantity, std::string memo) {
        if(from == get_self()) {
            Withdraw receiver(get_self(), get_self().value);      
            receiver.emplace(from, [&](auto& row) {          
                row.mykey = receiver.available_primary_key();
                row.user = to;
                row.balance = quantity;
                });
            } else {
            Deposit sender(get_self(), get_self().value);
            sender.emplace(to, [&](auto& row) {
                row.mykey = sender.available_primary_key();
                row.user = from;       
                row.balance = quantity;
                });
            }
        }
        
        private:
        TABLE insert{              
            uint64_t mykey;
            name user;
            asset balance; 

            uint64_t primary_key() const { return mykey; }
            //uint64_t primary_key() const { return mykey; }
        };

    typedef multi_index<"deposit"_n, insert> Deposit;
    typedef multi_index<"withdraw"_n, insert> Withdraw;
};
