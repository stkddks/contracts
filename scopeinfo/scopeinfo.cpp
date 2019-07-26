
#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>

using namespace eosio;

CONTRACT scopeinfo: public contract {
    public:
        using contract::contract;

        [[eosio::on_notify("eosio.token::transfer")]]
        void ontransfer(name from, name to, asset quantity, std::string memo) {
        if(from == get_self()) {
            
            Withdraw sender(get_self(), to.value);       
            sender.emplace(get_self(), [&](auto& row) {          
                row.mykey = sender.available_primary_key();
                row.user = to;
                row.balance = quantity;
            });
            Scopesend scopsave(get_self(), get_self().value);
            auto itr = scopsave.find(to.value);
            if(itr == scopsave.end()) {
                scopsave.emplace(get_self(), [&](auto& row) { 
                    row.scope = to.value;
            });
        } 
        } else {
            Deposit receiver(get_self(), from.value); //입금테이블 //내가 받는다 receiver
            receiver.emplace(get_self(), [&](auto& row) {
                row.mykey = receiver.available_primary_key();
                row.user = from;       
                row.balance = quantity;
            });
            Scoperecv scopsave(get_self(), get_self().value);
            auto itr = scopsave.find(from.value);
            if(itr == scopsave.end()) {
            scopsave.emplace(get_self(), [&](auto& row) { 
                row.scope = from.value;
            });
        }
        }
        }
        ACTION eraseall() {
            require_auth(get_self());
            //Deposit forErase1(get_self(), get_self().value);
            //Withdraw forErase2(get_self(), get_self().value);
            Scopesend scopsend(get_self(), get_self().value);   //이진씨한테 돈을 보냈다면 내 정보는 이 scoprsend에 저장이 되어 있겠지?
            auto itr1 = scopsend.begin();
            while(itr1 != scopsend.end()) { //조건이 참인 경우에만 while문 실행

                Deposit forEraseAll(get_self(), itr1->scope); //내 정보가  forEraseAll에 들어있겠지?
                auto itr = forEraseAll.begin();

                while(itr != forEraseAll.end()) {
                    itr = forEraseAll.erase(itr);
            }

            itr1 = scopsend.erase(itr1);
            }
            Scoperecv scoperecv(get_self(), get_self().value); 
            auto itr2 = scoperecv.begin();
            while(itr1 != scopsend.end()) {
                Withdraw forEraseAll(get_self(), itr2->scope);
                auto itr = forEraseAll.begin();
                while(itr != forEraseAll.end()) {
                    itr = forEraseAll.erase(itr);
                }
                itr1=scopsend.erase(itr1);
            }

        print("Erase success!!");
    }

        
        private:
        TABLE insert{              
            uint64_t mykey;
            name user;
            asset balance; 

            uint64_t primary_key() const { return mykey; }
            //uint64_t primary_key() const { return mykey; }
        };
        TABLE scope{
            name user;
            uint64_t scope;
            uint64_t primary_key() const { return scope; }

        }; 

    typedef multi_index<"depositss"_n, insert> Deposit;
    typedef multi_index<"withdrawss"_n, insert> Withdraw;
    typedef multi_index<"scopesend"_n, scope> Scopesend;
    typedef multi_index<"scoperecv"_n, scope> Scoperecv;
};