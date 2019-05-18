#!/bin/bash

array=(0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 \
      21 22 23 24 25 26 27 28 29 30 31)

printf "Usage: \n  o All IN  : 1\n  o All OUT : 2\n  o All 1   : 3\n  o All 0   : 4\n"

OPTION=1

while true
do
	read OPTION
	for i in "${array[@]}"
	do
		if [ $OPTION -eq 1 ]; then
			gpio mode $i in
		elif [ $OPTION -eq 2 ]; then
			gpio mode $i out
		elif [ $OPTION -eq 3 ]; then
			gpio write $i 1
		else
			gpio write $i 0
		fi	

	done
done
