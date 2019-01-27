cleos -u  https://api-kylin.eosasia.one set account permission connectfours active '{
  "threshold": 1,
  "keys":  [
      {"key": "EOS7bjn952JhKY4qGf6gcjjgXtgQjvpw9qAtfeNoUup1gtnFzwX1X","weight": 1}
  ],
  "accounts": [
    {
      "permission": {
        "actor": "connectfours",
        "permission": "eosio.code"
      },
      "weight": 1
    }
  ]
}' owner -p connectfours@owner

# Create account
cleos -u  https://api.jungle.alohaeos.com system newaccount mariaeyeroll eveeveeveeve EOS7efr2rCEL22P62LNkg5qhj1Mxh5ZeGkRGCM1u4p2Zs5fXnfssA EOS79JGDaiHog1x8VmuaxMLpfEYaK4EmMEvWRF5unwazAoi7Wd1FE --stake-cpu "100.0000 EOS" --stake-net "100.0000 EOS" --buy-ram-kbytes 20
# Transfer
cleos -u  https://api.jungle.alohaeos.com transfer haleythomsow eveeveeveeve "50.0000 EOS" ""

# Check game tables
cleos -u  https://api.jungle.alohaeos.com get table connectfours connectfours games

# Deploy contract
cleos  -u https://api.jungle.alohaeos.com  set contract connectfours /Users/jafri/eosio.contracts/connectfour

# Clear tables
cleos -u  https://api.jungle.alohaeos.com push action connectfours cleartables '[]' -p connectfours@owner

# Force finish (DEV)
cleos -u  https://api.jungle.alohaeos.com push action connectfours forcefinish '[0]' -p connectfours@owner
cleos -u  https://api.jungle.alohaeos.com push action connectfours deloldgames '[]' -p connectfours@owner

# Create game
cleos -u  https://api.jungle.alohaeos.com transfer mariaeyeroll connectfours "10.0000 EOS" ""
cleos -u  https://api.jungle.alohaeos.com push action connectfours creategame '["mariaeyeroll", "", "10.0000 EOS", 1]' -p mariaeyeroll@owner

cleos -u  https://api.jungle.alohaeos.com transfer mariaeyeroll connectfours "1.0000 EOS" ""
cleos -u  https://api.jungle.alohaeos.com push action connectfours creategame '["mariaeyeroll", "", "1.0000 EOS", 1]' -p mariaeyeroll@owner

cleos -u  https://api.jungle.alohaeos.com transfer mariaeyeroll connectfours "2.0000 EOS" ""
cleos -u  https://api.jungle.alohaeos.com push action connectfours creategame '["mariaeyeroll", "", "2.0000 EOS", 1]' -p mariaeyeroll@owner

# Join game
cleos -u  https://api.jungle.alohaeos.com push action connectfours joingame '["haleythomsow", 0]' -p haleythomsow@owner

# Drop disc (horizontal check)
cleos -u  https://api.jungle.alohaeos.com push action connectfours dropdisc '["haleythomsow", 0, 6]' -p haleythomsow@owner
cleos -u  https://api.jungle.alohaeos.com push action connectfours dropdisc '["eoscafeblock", 0, 6]' -p eoscafeblock@owner
cleos -u  https://api.jungle.alohaeos.com push action connectfours dropdisc '["haleythomsow", 0, 5]' -p haleythomsow@owner
cleos -u  https://api.jungle.alohaeos.com push action connectfours dropdisc '["eoscafeblock", 0, 6]' -p eoscafeblock@owner
cleos -u  https://api.jungle.alohaeos.com push action connectfours dropdisc '["haleythomsow", 0, 4]' -p haleythomsow@owner
cleos -u  https://api.jungle.alohaeos.com push action connectfours dropdisc '["eoscafeblock", 0, 6]' -p eoscafeblock@owner
cleos -u  https://api.jungle.alohaeos.com push action connectfours dropdisc '["haleythomsow", 0, 3]' -p haleythomsow@owner

cleos -u  https://api.jungle.alohaeos.com push action connectfours checkwin '[0]' -p haleythomsow@owner

# Drop disc (vertical check)
cleos -u  https://api.jungle.alohaeos.com push action connectfours dropdisc '["haleythomsow", 1, 4]' -p haleythomsow@owner
cleos -u  https://api.jungle.alohaeos.com push action connectfours dropdisc '["eoscafeblock", 1, 5]' -p eoscafeblock@owner
cleos -u  https://api.jungle.alohaeos.com push action connectfours dropdisc '["haleythomsow", 1, 4]' -p haleythomsow@owner
cleos -u  https://api.jungle.alohaeos.com push action connectfours dropdisc '["eoscafeblock", 1, 5]' -p eoscafeblock@owner
cleos -u  https://api.jungle.alohaeos.com push action connectfours dropdisc '["haleythomsow", 1, 4]' -p haleythomsow@owner
cleos -u  https://api.jungle.alohaeos.com push action connectfours dropdisc '["eoscafeblock", 1, 5]' -p eoscafeblock@owner
cleos -u  https://api.jungle.alohaeos.com push action connectfours dropdisc '["haleythomsow", 1, 4]' -p haleythomsow@owner

cleos -u  https://api.jungle.alohaeos.com push action connectfours checkwin '[0]' -p haleythomsow@owner

# Drop disc (diagonal check)
cleos -u  https://api.jungle.alohaeos.com push action connectfours dropdisc '["haleythomsow", 0, 6]' -p haleythomsow@owner

cleos -u  https://api.jungle.alohaeos.com push action connectfours dropdisc '["eoscafeblock", 0, 5]' -p eoscafeblock@owner
cleos -u  https://api.jungle.alohaeos.com push action connectfours dropdisc '["haleythomsow", 0, 5]' -p haleythomsow@owner

cleos -u  https://api.jungle.alohaeos.com push action connectfours dropdisc '["eoscafeblock", 0, 4]' -p eoscafeblock@owner
cleos -u  https://api.jungle.alohaeos.com push action connectfours dropdisc '["haleythomsow", 0, 4]' -p haleythomsow@owner

cleos -u  https://api.jungle.alohaeos.com push action connectfours dropdisc '["eoscafeblock", 0, 3]' -p eoscafeblock@owner;
cleos -u  https://api.jungle.alohaeos.com push action connectfours dropdisc '["haleythomsow", 0, 4]' -p haleythomsow@owner;
cleos -u  https://api.jungle.alohaeos.com push action connectfours dropdisc '["eoscafeblock", 0, 3]' -p eoscafeblock@owner;
cleos -u  https://api.jungle.alohaeos.com push action connectfours dropdisc '["haleythomsow", 0, 3]' -p haleythomsow@owner;
cleos -u  https://api.jungle.alohaeos.com push action connectfours dropdisc '["eoscafeblock", 0, 2]' -p eoscafeblock@owner;
cleos -u  https://api.jungle.alohaeos.com push action connectfours dropdisc '["haleythomsow", 0, 3]' -p haleythomsow@owner;

cleos -u  https://api.jungle.alohaeos.com push action connectfours checkwin '[0]' -p haleythomsow@owner
