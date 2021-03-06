#include "connectfours.hpp"

void connectfours::cleartables()
{
    require_auth(_self);

    cleanTable<games_table>();
    cleanTable<accounts_table>();
}


void connectfours::deloldgames()
{
    require_auth(_self);

    uint32_t currentTime = now();

    // Erase garbage games that already finished
    auto gameGarbIdx = games.get_index<"byfinishtime"_n>();
    auto start = gameGarbIdx.lower_bound(1);
    auto finish = gameGarbIdx.upper_bound(now() - (7 * DAY));

    while (start != finish) {
        start = gameGarbIdx.erase(start);
    }
}

void connectfours::forcefinish(const uint64_t gameId)
{
    require_auth(_self);

    // Erase games
    auto gameItr = games.find(gameId);
    check(gameItr != games.end(), "game does not exist.");
    games.modify(gameItr, same_payer, [&](auto &g){
       g.finishTime = now() - (7 * DAY);
    });
}

void connectfours::creategame( const name player1,
                               const name player2,
                               const asset buyIn,
                               const bool isPublic  )
{
    require_auth(player1);

    check(player1 != player2, "cannot play against yourself.");
    check(isPublic || is_account(player2), "game must be public or the challenged player must exist");

    // Charge for game
    // sub_balance(player1, buyIn);

    // Create game
    games.emplace(_self, [&](auto &g){
        g.gameId = games.available_primary_key();
        g.player1 = player1;
        g.buyIn = buyIn;
        g.isPublic = isPublic;

        // Set player 2 if not public
        if (!isPublic) {
            g.player2 = player2;
        }

        // Board
        vector<vector<uint8_t>> board(6, vector<uint8_t>(7));
        g.board = board;
    });
}

void connectfours::joingame( const name player2,
                             const uint64_t gameId,
                             const name referral )
{
    require_auth(player2);

    // Find game
    auto gameItr = games.find(gameId);
    check(gameItr != games.end(), "game does not exist.");
    check(gameItr->isPublic || gameItr->player2 == player2, "not authorized to join game.");
    check(gameItr->player1 != player2, "cannot play against yourself.");
    check(!gameItr->hasStarted(), "game has already started.");

    // Charge for game
    sub_balance(player2, gameItr->buyIn);

    // Start game
    games.modify(gameItr, _self, [&](auto &g){
        g.startTime = now();

        if (g.isPublic) {
            g.player2 = player2;
        }

        g.lastPlayer = now() % 2 == 0 ? g.player1 : g.player2;
        g.lastMove = now();
        
        g.referral = referral;
    });
}

void connectfours::dropdisc( const name player,
                            const uint64_t gameId,
                            const uint8_t columnIndex )
{
    require_auth(player);

    // Find game
    auto gameItr = games.find(gameId);
    check(gameItr != games.end(), "game does not exist.");
    check(gameItr->hasStarted() && !gameItr->hasFinished(), "game must have started and not finished.");
    check(gameItr->discsDropped < 42, "game board is full.");
    check(player == gameItr->nextPlayer(), "it is not your turn.");

    // Find first empty row
    int8_t emptyRowIndex = 5;
    for (; emptyRowIndex >= 0; emptyRowIndex--) {
        if (gameItr->board[emptyRowIndex][columnIndex] == 0) {
            break;
        }
    }

    check(emptyRowIndex >= 0, "column is full.");

    // Change game state
    games.modify(gameItr, same_payer, [&](auto &g){

        // Add disc to that row
        uint8_t playerSymbol = g.lastPlayer == g.player2 ? 1 : 2;
        g.board[emptyRowIndex][columnIndex] = playerSymbol;

        // Update last time and player
        g.lastMove = now();
        g.lastPlayer = player;
        g.discsDropped++;

        // All discs dropped
        if (g.discsDropped == 42) {
           uint8_t winnerSymbol = _checkwin(g.board);
           g.finishTime = now();

           // No winner
           if (winnerSymbol == 0) {
               payout(g.player1, g.singlePayout(), string("Draw Connect 4 Game! Play now at connectfours.io"));
               payout(g.player2, g.singlePayout(), string("Draw Connect 4 Game! Play now at connectfours.io"));
               
               g.winner = "draw"_n;
           // Winner is found
           } else {
               name winnerName = winnerSymbol == 1 ? g.player1 : g.player2;
               payout(winnerName, g.doublePayout(), string("Won Connect 4 Game! Play now at connectfours.io"));

               if (is_account(g.referral)) {
                 payout(g.referral, g.refPayout(), string("Referral Cut for Connect 4! Play now at connectfours.io"));
               }
               
               g.winner = winnerName;
           }
        }
    });
}

void connectfours::checkwin( const uint64_t gameId )
{
    // Find game
    auto gameItr = games.find(gameId);
    check(gameItr != games.end(), "game does not exist.");
    check(gameItr->hasStarted() && !gameItr->hasFinished(), "game must have started and not finished.");
    check(gameItr->discsDropped > 7, "only 7 discs have been dropped, win is not possible.");

    games.modify(gameItr, same_payer, [&](auto &g){
        // Find winner
        uint8_t winnerSymbol = _checkwin(g.board);
        check(winnerSymbol != 0, "game has not been won yet.");

        // Finish game
        g.finishTime = now();

        // Payout winner
        name winnerName = winnerSymbol == 1 ? g.player1 : g.player2;
        payout(winnerName, g.doublePayout(), string("Won Connect 4 Game! Play now at connectfours.io"));

        g.winner = winnerName;
    });
}

uint8_t connectfours::_checkwin( const vector<vector<uint8_t>> board )
{
    uint8_t HEIGHT = 6;
    uint8_t WIDTH = 7;
    uint8_t EMPTY = 0;

    for (int r = 0; r < HEIGHT; r++) { // iterate rows, bottom to top
        for (int c = 0; c < WIDTH; c++) { // iterate columns, left to right
            int player = board[r][c];

            if (player == EMPTY)
                continue; // don't check empty slots

            if (c + 3 < WIDTH &&
                player == board[r][c+1] && // look right
                player == board[r][c+2] &&
                player == board[r][c+3])
                return player;

            if (r + 3 < HEIGHT) {
                if (player == board[r+1][c] && // look down
                    player == board[r+2][c] &&
                    player == board[r+3][c])
                    return player;

                if (c + 3 < WIDTH &&
                    player == board[r+1][c+1] && // look down & right
                    player == board[r+2][c+2] &&
                    player == board[r+3][c+3])
                    return player;

                if (c - 3 >= 0 &&
                    player == board[r+1][c-1] && // look down & left
                    player == board[r+2][c-2] &&
                    player == board[r+3][c-3])
                    return player;
            }
        }
    }

    return EMPTY; // no winner found
}

void connectfours::cancelgame( const uint64_t gameId )
{
    // Find game
    auto gameItr = games.find(gameId);
    check(gameItr != games.end(), "game does not exist.");
    check(!gameItr->hasStarted(), "game has already started.");

    // Require auth of player 1
    require_auth(gameItr->player1);

    // Return initial cost to player 1
    payout(gameItr->player1, gameItr->buyIn, string("Cancelled Connect 4 Game! Play now at connectfours.io"));

    // Finish game
    games.erase(gameItr);
}

void connectfours::claimtimeout(const uint64_t gameId)
{
    // Find game
    auto gameItr = games.find(gameId);
    check(gameItr != games.end(), "game does not exist.");
    check(gameItr->hasStarted(), "game has not started yet.");
    check(!gameItr->hasFinished(), "game has already finished.");
    check(gameItr->timedOut(), "game has not timed out.");

    // Require auth of last player
    require_auth(gameItr->lastPlayer);

    // Payout
    payout(gameItr->lastPlayer, gameItr->doublePayout(), string("Won Connect 4 Game by timeout! Play now at connectfours.io"));

    // Finish game
    games.modify(gameItr, same_payer,[&](auto &g){
        g.finishTime = now();
        g.winner = g.lastPlayer;
    });
}

void connectfours::forfeitgame( const uint64_t gameId,
                                const name player )
{
    // Require auth of user
    require_auth(player);

    // Find game
    auto gameItr = games.find(gameId);
    check(gameItr != games.end(), "game does not exist.");
    check(gameItr->hasStarted(), "game has not started yet.");
    check(!gameItr->hasFinished(), "game has already finished.");
    check(player == gameItr->player1 || player == gameItr->player2, "not a player to the game.");

    // Payout other player
    name otherPlayer = player == gameItr->player1 ? gameItr->player2 : gameItr->player1;
    payout(otherPlayer, gameItr->doublePayout(), string("Other player forfeit Connect 4 Game! Play now at connectfours.io"));

    // Finish game
    games.modify(gameItr, same_payer,[&](auto &g){
        g.finishTime = now();
        g.winner = otherPlayer;
    });
}

void connectfours::payout (const name    receiver,
                           const asset   quantity,
                           const string  memo )
{
    // Inline transfer
    connectfours::transfer_action t_action( TOKEN_CONTRACT, {_self, "active"_n} );
    t_action.send(_self, receiver, quantity, memo);
}

// **TODO add withdraw function, and set a designated developer account
void connectfours::add_balance( const name     owner,
                               const asset    quantity)
{
    auto itr = accounts.find(owner.value);

    if( itr == accounts.end() ) {
        accounts.emplace( _self, [&]( auto& a ){
            a.owner = owner;
            a.balance = quantity;
        });
    } else {
        accounts.modify( itr, same_payer, [&]( auto& a ) {
            a.balance += quantity;
        });
    }
}

void connectfours::sub_balance ( const name     owner,
                                const asset    quantity )
{
    auto accItr = accounts.find(owner.value);
    check( accItr != accounts.end(), "account does not exist" );
    check( accItr->balance.amount >= quantity.amount, "overdrawn balance");

    accounts.modify(accItr, same_payer, [&](auto &a){
        a.balance -= quantity;
    });

    if(accItr->balance.amount == 0) {
        accounts.erase(accItr);
    }
}

void connectfours::deposit(name from, name to, asset quantity, const std::string& memo){
    if(to != _self) return;
    
    check(quantity.symbol != DEFAULT_SYMBOL, "must deposit EOS.");
    check(quantity.amount >= MINIMUM_BUYIN, "must buy in with a minimum of 0.5 EOS to cover RAM.");

    // Deposit money
    add_balance(from, quantity);
}