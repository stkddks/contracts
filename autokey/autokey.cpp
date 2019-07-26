
/*이건 무슨 문제일까..? */

#include <eosio/eosio.hpp>

using namespace eosio;

CONTRACT autokey: public contract {
    public:
        using contract::contract;

        ACTION insertkey() {
// require_auth(get_self());

            Autokey myTable(get_self(), get_self().value);
            myTable.emplace(get_self(), [&](auto& row) {
                row.mykey = myTable.available_primary_key();
            }); //mykey 1씩 증가한 값이 저장
        }

    private:
        TABLE example_struct {
            uint64_t mykey;
            uint64_t primary_key() const { return mykey; }
        };

    typedef multi_index<"autokeys"_n, example_struct> Autokey;
};
