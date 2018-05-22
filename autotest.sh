#!/bin/bash

set -e

rm test.db
valgrind ./ex17 test.db c 100 100
./ex17 test.db s 1 joe joe@gmail.com 23
./ex17 test.db s 2 tim tim@whim.lol 30
./ex17 test.db s 3 jack jack@black.com 45
./ex17 test.db s 4 anon anonymous@legion.com 100
echo "Get 1st record"
./ex17 test.db g 1
echo "List all records"
./ex17 test.db l
echo "Deleting last record and listing again"
valgrind ./ex17 test.db d 4
./ex17 test.db l
echo "Find a guy named joe"
./ex17 test.db f name joe
echo "Find a guy 30 yrs old"
./ex17 test.db f age 30
echo "Find email jack@black.com"
./ex17 test.db f email jack@black.com
#./ex17 test.db f email nosuchmail@gmail.com
#./ex17 test.db f gender male
