#include <eosiolib/eosio.hpp>

using namespace eosio;

CONTRACT count: public contract {
    public:
	using contract::contract;

    ACTION countuser(name user)
    {
        require_auth(user);
        count forcountuser(get_self(), get_self().value);	//scope 한곳에 모든 정보가 저장이 된다. get_self().value하면 모두 다 같은 scope에 정보가 저장이된다.
		auto itr = forcountuser.find(user.value);
        if( itr == forcountuser.end() ){
            forcountuser.emplace(user, [&]( auto& row ) {
                row.user = user;
                row.count = 1;  //처음 카운터는 1부터!!
                });
                print("Insert success");
        }else {
            forcountuser.modify(itr, user, [&]( auto& row ) {		//정보를 수정하는데에도 ram비용이 든다.
                row.user = user;
                row.count ++;
                });
                print(itr->user, " ", itr->count);
        }
    }
	ACTION findcount( uint64_t count ) {
        count addresses( get_self(), get_self().value ); //get_self().value --> 스코프에 접근하는 방법
		auto forSecondary = addresses.get_index< "bycount"_n >();
        auto itr = forSecondary.require_find(count, "no count");    //itr은 해당 카운터의 그사람을 가르킨다.
        print( itr->count, " ", itr->user );
    }
    
	ACTION eraseall() {
		require_auth(get_self());          
		count forErase(get_self(), get_self().value);	
		auto itr = forErase.begin();

        while(itr != forErase.end()){
            itr = forErase.erase(itr);
        }	
		//forErase.erase(itr);
		//print("Erase success");
	}

    private:
    
    TABLE person{			//DB의 형태를 정의하는 것
	//struct [[eosio::table]] person{	 
		name user;
        uint64_t count;
		
		uint64_t primary_key() const { return user.value; }
        uint64_t by_count() const { return count; }  // secondary_key_age()를 by_age() 도 가능
        };
	

	typedef multi_index< "counttable"_n, person, 
	indexed_by< "bycount"_n, const_mem_fun< person, uint64_t, &person::by_count>> > count;
    //테이블이름은 counttable, 구조체이름 person
	//블록체인 상에서 쓰이는 이름 "bycount"_n 
    //매번 multi_index<"people"_n, person>---쭈욱--- 써야하는 것이 불편하니까 count로 바꿔쓰겠다 하는것을 정의하는 것!
};

//by_age가 세컨더리 인덱스였어..!