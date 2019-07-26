#include <eosio/eosio.hpp>

using namespace eosio;

CONTRACT addressbook_3: public contract {
    public:
	using contract::contract;

    ACTION hi(name user) {
        require_auth(user);
        address_index forHi(get_self(), get_self().value);
        auto itr = forHi.find(user.value);
        if( itr == forHi.end() ){
            print("Get OUT!!!");
		}else {
            print("Hello, ", user);
        }
    }

    ACTION upsert(name user)
	{
		require_auth(user);
		
		address_index forUpsert(get_self(), get_self().value);
	
		auto itr = forUpsert.find(user.value);

		if( itr == forUpsert.end() ){
			forUpsert.emplace(get_self(), [&]( auto& row ) {
				row.user = user;
				});
			print("Insert success");
			send_summary(user, " successfully emplaced record to addressbook");
		}else {
			forUpsert.modify(itr, get_self(), [&]( auto& row ) {	
				row.user = user;
				});
                print("Modify success");
				send_summary(user, " successfully modified record to addressbook");
		}
	}


    ACTION remove(name user) {
		require_auth(user);          
		address_index forRemove(get_self(), get_self().value);	
		auto itr = forRemove.find(user.value);										
		check(itr != forRemove.end(), "Record does not exist");				
		forRemove.erase(itr);
		print("Remove success");
	}
	
	ACTION notify(name user, std::string msg) {
		require_auth(get_self());
		require_recipient(user);
		}

    private:
	struct [[eosio::table]] allowance{	        
		name user;	
		uint64_t primary_key() const { return user.value; } 
	};	

	void send_summary(name user, std::string message) {
    action(
		permission_level{get_self(),"active"_n},
		get_self(),
		"notify"_n,
		std::make_tuple(user, name{user}.to_string() + message)
    ).send();
	};

	typedef multi_index<"allowance"_n, allowance> address_index;

};