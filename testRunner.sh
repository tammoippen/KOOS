#! /usr/bin/env bash

function createDirs() {
	if [[ -d ./experiments/$1 ]]; then
		rm -fr ./experiments/$1	
	fi
	
	mkdir ./experiments/$1
	mkdir ./experiments/$1/Frequency
	mkdir ./experiments/$1/BlockFrequency
	mkdir ./experiments/$1/Runs
	mkdir ./experiments/$1/LongestRun
	mkdir ./experiments/$1/Rank
	mkdir ./experiments/$1/FFT
	mkdir ./experiments/$1/NonOverlappingTemplate
	mkdir ./experiments/$1/OverlappingTemplate
	mkdir ./experiments/$1/Universal
	mkdir ./experiments/$1/LinearComplexity
	mkdir ./experiments/$1/Serial
	mkdir ./experiments/$1/ApproximateEntropy
	mkdir ./experiments/$1/CumulativeSums
	mkdir ./experiments/$1/RandomExcursions
	mkdir ./experiments/$1/RandomExcursionsVariant
}

# generate data => to costy due to bbs
DATADIR=$HOME/dataOpOrderNew
#if [[ -d $DATADIR/data$1 ]]; then
#	rm -fr $DATADIR/data$1
#fi
#mkdir $DATADIR/data$1
#./prndGen $DATADIR/data$1 $1 >> ./experiments/console$1.txt

# key Avalance Row
filename=keyAvaRow
createDirs $filename$1
./assess 1048576 384 $DATADIR/data$1/$filename.data $filename$1 >> ./experiments/$filename$1/console.txt &
t1=$!

# plain Avalance Row
filename=plainAvaRow
createDirs $filename$1
./assess 1048576 384 $DATADIR/data$1/$filename.data $filename$1 >> ./experiments/$filename$1/console.txt &
t2=$!

# plain cypher correlation Row
filename=plainCyphCorrRow
createDirs $filename$1
./assess 1048576 128 $DATADIR/data$1/$filename.data $filename$1 >> ./experiments/$filename$1/console.txt &
t3=$!

# cbc Row
filename=cbcModeRow
createDirs $filename$1
./assess 1048576 300 $DATADIR/data$1/$filename.data $filename$1 >> ./experiments/$filename$1/console.txt &
t4=$!

# random plain key Row
#filename=randPlainKeyRow
#createDirs $filename$1
#./assess 1048576 128 $DATADIR/data$1/$filename.data $filename$1 >> ./experiments/$filename$1/console.txt &
#t5=$!

# low density plain Row
filename=lowDensPlainRow
createDirs $filename$1
./assess 1056896 128 $DATADIR/data$1/$filename.data $filename$1 >> ./experiments/$filename$1/console.txt &
t6=$!

# low density key Row
filename=lowDensKeyRow
createDirs $filename$1
./assess 1056896 128 $DATADIR/data$1/$filename.data $filename$1 >> ./experiments/$filename$1/console.txt &
t7=$!

# high density plain Row
filename=highDensPlainRow
createDirs $filename$1
./assess 1056896 128 $DATADIR/data$1/$filename.data $filename$1 >> ./experiments/$filename$1/console.txt &
t8=$!

# high density key Row
filename=highDensKeyRow
createDirs $filename$1
./assess 1056896 128 $DATADIR/data$1/$filename.data $filename$1 >> ./experiments/$filename$1/console.txt &
t9=$!

wait $t1
wait $t2
wait $t3
wait $t4
#wait $t5
wait $t6
wait $t7
wait $t8
wait $t9
