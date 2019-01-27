#include "config.hpp"
#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>

#include <string>
#include <vector>

using namespace eosio;
using namespace std;

struct transfer_args {
    name  from;
    name  to;
    asset         quantity;
    string        memo;
};


CONTRACT connectfour : contract {
    public:

        connectfour( name self, name code,  datastream<const char*> ds )
                   : contract(self, code, ds),
                     games(self, self.value),
                     accounts(self, self.value) {}

        template <typename T>
        void cleanTable(){
                T db(_self, _self.value);
                while(db.begin() != db.end()){
                        auto itr = --db.end();
                        db.erase(itr);
                }
        }

        ACTION creategame( const name player1,
                         const name player2,
                         const asset buyIn,
                         const bool isPublic );

        ACTION joingame( const name player2,
                       const uint64_t gameId );

        ACTION dropdisc( const name player,
                       const uint64_t gameId,
                       const uint8_t columnIndex );

        ACTION checkwin( const uint64_t gameId );

        ACTION cancelgame( const uint64_t gameId );

        ACTION forfeitgame( const uint64_t gameId,
                          const name player );

        ACTION claimtimeout( const uint64_t gameId );

        ACTION cleartables();

        ACTION forcefinish( const uint64_t gameId );

        ACTION deloldgames();

        void transfer( const name& from, const name& to, const asset& quantity, string& memo );

private:
        uint8_t _checkwin( const vector<vector<uint8_t>> board );

        void add_balance( const name owner,
                          const asset quantity);
        void sub_balance( const name owner,
                          const asset quantity);

        void payout (const name    receiver,
                     const asset   quantity,
                     const string  memo );

        struct [[eosio::table, eosio::contract("connectfour")]] game {
            uint64_t       gameId;
            bool           isPublic;
            vector<vector<uint8_t>>  board;
            uint8_t        discsDropped = 0;

            name           player1;
            name           player2;
            name           winner;

            asset          buyIn;
            asset singlePayout () const { return asset{static_cast<int64_t>(buyIn.amount * WINNER_CUT), DEFAULT_SYMBOL}; };
            asset doublePayout () const { return singlePayout() * 2; };

            name           lastPlayer;
            name nextPlayer () const { return lastPlayer == player2 ? player1 : player2; };

            uint32_t       lastMove;
            bool timedOut () const { return now() > lastMove + DAY; };

            uint32_t       startTime;
            uint32_t       finishTime;
            bool hasStarted () const { return startTime > 0; };
            bool hasFinished () const { return finishTime > 0; };

            uint64_t primary_key()const     { return gameId; }     // 1
            uint64_t get_started_by()const  { return player1.value; }    // 2
            uint64_t get_joined_by()const   { return player2.value; }    // 3
            uint64_t get_start_time()const  { return startTime; }  // 4
            uint64_t get_finish_time()const { return finishTime; } // 5
        };

        struct [[eosio::table, eosio::contract("connectfour")]] account {
            name         owner;
            asset        balance = asset{0, DEFAULT_SYMBOL};

            uint64_t primary_key()const { return owner.value; }
        };

        typedef eosio::multi_index<"games"_n, game,
                    indexed_by<"startedby"_n,    const_mem_fun<game, uint64_t, &game::get_started_by>>,
                    indexed_by<"joinedby"_n,     const_mem_fun<game, uint64_t, &game::get_joined_by>>,
                    indexed_by<"bystarttime"_n,  const_mem_fun<game, uint64_t, &game::get_start_time>>,
                    indexed_by<"byfinishtime"_n, const_mem_fun<game, uint64_t, &game::get_finish_time>> > games_table;

        typedef eosio::multi_index<"accounts"_n, account> accounts_table;

        games_table games;
        accounts_table accounts;
};
