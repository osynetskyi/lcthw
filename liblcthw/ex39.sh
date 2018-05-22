#!/bin/bash

rm times.log
echo "scan find binstr" >> times.log
for i  in 1 2 3 4 5 6 7 8 9 10
do
	echo `tests/string_algos_tests 2>&1 | grep COUNT | cut -d' ' -f6 | cut -d',' -f1` >> times.log	
done
Rscript ex39.r
