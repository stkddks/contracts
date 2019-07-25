#include <eosio/eosio.hpp>

using namespace eosio;

CONTRACT address: public contract {
    public:
	using contract::contract;

    ACTION hi(name user) {
		require_auth(user);
        address_index forHi(get_self(), get_self().value);
        auto itr = forHi.find(user.value, "push!!");
		
		print("Hi, ", user);
		/* xx
		if( itr == forHi.end() ){
            print("Get OUT!!!");
        }else {
            print("Hello, ", user);
        }
		xx */
        //check(has_auth(itr != forHi.end()), "missing");
        //print("Hello, ", user);
        
    }
    
    ACTION upsert(name user)
	{
		require_auth(get_self());
		
		address_index forUpsert(get_self(), get_self().value);	//(컨트렉트주인, scope) //계정별로 scope확인하려면 //user.value
		
		auto itr = forUpsert.find(user.value);		//itr은 포인터와 같다
		//만약 itr을 객체로 쓰고프면
		//auto& obj = *itr
		//print(obj.user, obj.first_name);
		//auto itr2 = iterator_to(obj);
		//if(itr == itr2)
		//print(itr->user, itr->first_name, *itr.user);


		if( itr == forUpsert.end() ){
			forUpsert.emplace(get_self(), [&]( auto& row ) {
				row.user = user;
				});
				print("Insert success");
		} else {
			forUpsert.modify(itr, get_self(), [&]( auto& row ) {	
				row.user = user;
				});
                print("Modify success");
		}	
	}

    ACTION remove(name user) {
		require_auth(get_self());          
		address_index forRemove(get_self(), get_self().value);	
		auto itr = forRemove.find(user.value);										
		check(itr != forRemove.end(), "Record does not exist");				
		forRemove.erase(itr);
		print("Remove success");
	}

    private:
	struct [[eosio::table]] allowance{	        
		name user;	
		uint64_t primary_key() const { return user.value; } 
	};	

	typedef multi_index<"allowance"_n, allowance> address_index;

};


	


/* 
	ACTION insert(name user, std::string first_name, std::string last_name, uint32_t age )
	{
		require_auth(user);	//일단 다른사람이 접근하지 못하게 막은다음
		address_index forInsert(get_self(), get_self().value);
		auto itr = forInsert.find(user.value);
		//존재하는지 안하는 지를 알수 있어야 하므로

		check(itr == forInsert.end(), "already exists");
		//find라는 메소드를 통해서 forInsert테이블을 조회하게되
		//중간에서 정보를 찾았으면 itr이 end가 되지 않는다	
		//코드에서는 itr == end, itr이 end를 가르킨다고 하는데	end를 가르킨다면 이미 정보가 존제한다고 에러메세지 알려주고
		
		forInsert.emplace(user, [&](auto& row){	//사용자가 데이터를 지불하는데 있어서 이제 ram을 사용하겠다 하는 의미
			row.user = user;
			row.first_name = first_name;
			row.last_name = last_name;
			row.age = age;				//받아온 정보들을 넣어준다. 테이블에 정보들을 넣어준다.
	});
	print("Insert success");
}*/
