!/bin/bash

#eosio-cpp --abigen --contract=connectfour connectfour.cpp -o ~/contracts/connectfour.wasm
#sed -i 's/1.1/1.0/g' ~/contracts/connectfour.abi
#eosio-cpp --contract=connectfour connectfour.cpp -o ~/contracts/connectfour.wasm
cleos -u https://nodes.get-scatter.com set contract connectfours ~/contracts/ connectfour.wasm connectfour.abi -p connectfours@active
