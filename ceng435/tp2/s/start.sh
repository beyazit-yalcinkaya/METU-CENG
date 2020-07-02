#!/bin/bash

./reset.sh

./init.sh

./config.sh $2

python3 main.py $1 $3 $4
