#!/bin/bash

num=$1
dir=$PWD

fich_size=$(du -a -b | sort -n -r | awk '{print $2}')

#echo $fich_size
if [ $# -gt 1 ]
then 
	echo "Usage error: incorrect number of arguments"
	exit 1
fi

if [ $# -eq 1 ]
then 

	if [ $1 -eq 0 ]
	then
		exit 1
	fi
fi

for x in $fich_size 
do
	if(test -f $x)
	then 
		du -b $x | awk '{print $2"	"$1}' 
	fi
done | sed $1q
