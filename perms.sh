#!/bin/bash

PKEY=EOS6cJqfJgH8dqp1zcFxsVo9szgTS4LEPKACuwZgtgjwPF7XEo9D1

cleos -u https://nodes.get-scatter.com set account permission connectfours active \
'{"threshold": 1,"keys": [{"key": "'$PKEY'","weight": 1}],"accounts": [{"permission":{"actor":"connectfours","permission":"eosio.code"},"weight":1}]}' owner
