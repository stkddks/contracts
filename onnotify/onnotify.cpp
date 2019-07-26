/* 문제1. 1개의 테이블 추가 : 입금액
이 테이블에 입금액이 누적될 수 있도록 바꾸시오출*/

#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>

using namespace eosio;

CONTRACT onnotify: public contract {
    public:
        using contract::contract;
        
        ACTION dummy() {} //쓰레기 액션을 정의해준다. ACtion dummy
                //왜? 밑에 액션만 써주면 컴파일이 안되기에
        [[eosio::on_notify("eosio.token::transfer")]] //이액션이 실행될떄에만
        void ontransfer(name from, name to, asset quantity, std::string memo) { //이 액션이 실행되는 것이다. 요것이 바로 notify!!
            if(from == get_self()) { //출금테이블
                outs myTable(get_self(), get_self().value); //스코프는 get_self이기에 다 통일되서 이안에 저장되겠찌?
            
                if(myTable.begin() == myTable.end()) { //아무것도 없는 경우--테이블을 만들어줘야해
                    myTable.emplace(from, [&](auto& row) { // &는 콜타입함수
                        row.balance = quantity;
                    });
                } else {
                    auto itr = myTable.begin();
                    myTable.modify(itr, from, [&](auto& row) {
                        row.balance += quantity;
                    });
                }
            } else if (to == get_self()){ //입금테이블
                ins myTable(get_self(), get_self().value);

                if(myTable.begin() == myTable.end()) { //아무것도 없는 경우--테이블을 만들어줘야해
                    myTable.emplace(to, [&](auto& row) { // &는 콜타입함수
                        row.balance = quantity;
                    });
                } else {
                    auto itr = myTable.begin();
                    myTable.modify(itr, to, [&](auto& row) {
                        row.balance += quantity;
                    });
                }
            }
        }
    private:
        TABLE outstruct {
            asset balance;
            uint64_t primary_key() const { return balance.symbol.code().raw(); }
        };     
        typedef multi_index<"out"_n, outstruct> outs;
        typedef multi_index<"in"_n, outstruct> ins;
};

