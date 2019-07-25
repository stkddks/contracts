#include <eosiolib/eosio.hpp>

using namespace eosio;

CONTRACT addressbook: public contract {
    public:
	using contract::contract;
	ACTION findage( uint64_t age ) {
            address_index addresses( get_self(), get_self().value ); //get_self().value --> 스코프에 접근하는 방법
			//스코프는 굉장히 유동적인 의미
			//get_self().value 는 컨트랙트 생성자의 스코프에 접근하겠다는 의미
			//만약 저 부분이 user라고 되있으면 user 변수로 들어오는 값, 계정마다의 스코프가 생성되고 그 각각의 스코프로 접근한다는 의미이구!
			//만약 저 부분이 sym.code().raw() 이면 우리가 생성한 eos,lsa,baek 등의 각각의 심볼마다 스코프(범위)가 생성되는 것이지!

            auto forSecondary = addresses.get_index< "byage"_n >();

            auto itr = forSecondary.require_find( age, "no age");

            print( itr->user, " ", itr->age );
    }
	ACTION erase(name user) {
		require_auth(user);          
		address_index forErase(get_self(), get_self().value);	//get_self는 이 컨트렉트를 배포한 사람을 의미하며 
																//얘대신 "baekseokinit"_n으로 지칭해줘도 되
		auto itr = forErase.require_find(user.value, "no account");	//찾고난 결과물은 itr에 넣을것				
		forErase.erase(itr);
		print("Erase success");
	}


	ACTION upsert(name user, std::string first_name, std::string last_name, uint32_t age )
	{
		require_auth(user);	//일단 다른사람이 접근하지 못하게 막은다음
		address_index forUpsert(get_self(), get_self().value);	//scope 한곳에 모든 정보가 저장이 된다.
		auto itr = forUpsert.find(user.value);

		if( itr == forUpsert.end() ){
			forUpsert.emplace(user, [&]( auto& row ) {
				row.user = user;
				row.first_name = first_name;
				row.last_name = last_name;
				row.age = age;
				});
				print("Insert success");
		}else {
			forUpsert.modify(itr, user, [&]( auto& row ) {		//정보를 수정하는데에도 ram비용이 든다.
				row.user = user;
				row.first_name = first_name;
				row.last_name = last_name;
				row.age = age;
				});
			print("Modify success");
		}
	}

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
	}

    private:
	//TABLE person{			//DB의 형태를 정의하는 것
	struct [[eosio::table]] person{	 //위 코드랑 같은 의미
		name user;	//계정이름-- 계정은 이용자당 단 하나의 값이므로 key값으로 적합
		std::string first_name;
		std::string last_name;
		uint64_t age;
		
		uint64_t primary_key() const { return user.value; }
         uint64_t by_age() const { return age; }  // secondary_key_age()를 by_age() 도 가능
        };	//여기까지가 정보라고 생각하면 된다. 
	
//테이블의 이름은 people이고 필드명을 정의해준것!

	typedef multi_index< "peopletwo"_n, person, // 
	indexed_by< "byage"_n, const_mem_fun < person, uint64_t, &person::by_age>> > address_index;
	//매번 multi_index<"people"_n, person> 써야하는 것이 불편하니까 address_index로 바꿔쓰겠다 하는것을 정의하는 것!
};

