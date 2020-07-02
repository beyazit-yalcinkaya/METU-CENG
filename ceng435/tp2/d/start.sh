#!/bin/bash

./reset.sh

./init.sh

./config.sh $1

python3 main.py $2 $3
