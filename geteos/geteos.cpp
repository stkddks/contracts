#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>

using namespace eosio;

CONTRACT geteos: public contract{
    public:
        using contract::contract;

        ACTION getbalance(name user, asset token){  //우리가 찾고싶은 토큰의 심볼을 입력해주면되고 
            //테이블내용을 바뀌는 것이 아니기에 require는 하지 않아야해
            accounts mytoken("eosio.token"_n, user.value);    //우리의 잔액을 가지고 있는것은 eosio-token이니까
            symbol sym = token.symbol;
            auto itr = mytoken.require_find(sym.code().raw(), "no balance");

            print(itr->balance);    //balance를 출력!
        }
    private:
    TABLE account {
            asset    balance;

            uint64_t primary_key()const { return balance.symbol.code().raw(); }
        };
TABLE currency_stats {
            asset    supply;
            asset    max_supply;
            name     issuer;

            uint64_t primary_key()const { return supply.symbol.code().raw(); }
        };
        
        typedef multi_index< "accounts"_n, account > accounts;   //balance를 의미. 잔액!! //accounts로 사용하겠다는 의미
        typedef multi_index< "stat"_n, currency_stats > stats;   //stats으로 사용하겠다
};