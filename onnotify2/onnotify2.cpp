/* 
문제2. 각테이블에 name type 변수를 선언해서 EOS 보낸 사람또는 EOS를 받은 사람 그리고 얼마를 보내고 받았는지 기록하는 테이블을 만들고 on_notify 액션을 수정하시오.
예시: 입출금 횟수마다 따로 저장하는게 아니고 각 계정마다 누적금액을 저장할 수 있도록 해주세요 화이팅..!
*/

#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>

using namespace eosio;

CONTRACT onnotify2: public contract {
    public:
        using contract::contract;
        ACTION dummy() {        //쓰레기 액션을 정의해준다. ACtion dummy
                //왜? 밑에 액션만 써주면 컴파일이 안되기에
        [[eosio::on_notify("eosio.token::transfer")]]   //이액션이 실행될떄에만
        void ontransfer(name from, name to, asset quantity, std::string memo) { //이 액션이 실행되는 것이다. 요것이 바로 notify!!
            if(from == get_self()) {
                Receiver receiver(get_self(), get_self().value);    //스코프는 get_self이기에 다 통일되서 이안에 저장되겠찌?
                auto itr = receiver.find(to.value); //이 find 함수가 안에 to.value 값이 없으면 end 즉, 맨 끝값을 반환한다.
                if(itr == receiver.end()) {         //만약 반환값이 end라면, 즉, 안에 to.value 값이 없다면
                    receiver.emplace(from, [&](auto& row) {          // ( 새로운 객체를 위해 ram을 지불할 계정, 콜백함수 )
                        row.user = to;
                        row.balance = quantity;
                    });
                } else {
                    receiver.modify(itr, from, [&](auto& row) {
                        row.balance += quantity;
                    });
                }
            } else {
                Sender sender(get_self(), get_self().value);
                auto itr = sender.find(from.value);
                if(itr == sender.end()) {
                    sender.emplace(to, [&](auto& row) {
                        row.user = from;        // 받는건데 from이 필요할까?
                        row.balance = quantity;
                    });
                } else {
                    sender.modify(itr, to, [&](auto& row) {
                    row.balance += quantity;
                });
                }
            }
        }
    private:
        TABLE trxlist {
            name user;
            asset balance;      // 총 보낸, 총 받은 값 저장

            uint64_t primary_key() const { return user.value; }
        };  // return user.value; 는 어떤 정보별로 나뉘어야 하는지를 나타낸다
            //(여기선 문제가 계정별이기에!) //프라이머리 키는 사용자 정보마다로!

        typedef multi_index<"eossender"_n, trxlist> Sender;
        typedef multi_index<"eosreceiver"_n, trxlist> Receiver;
};
};