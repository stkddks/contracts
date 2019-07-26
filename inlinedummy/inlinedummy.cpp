#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>

using namespace eosio;

CONTRACT inlinedummy: public contract {
    public:
    using contract::contract;

    ACTION dummy(name user) {
        require_auth (user);
        print("dummy");
        action(
            permission_level(user, "active"_n ),  //user 대신 get_self()하면 컨트렉트가 수행한다는 거지?
            get_self(),
            "dummytwo"_n,
            std::make_tuple(user)
        ).send();        
    }
    ACTION dummytwo(name user) {
        require_auth(user);
        print("dummy two,", user);  

    private:
};