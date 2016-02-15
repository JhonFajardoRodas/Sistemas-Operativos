#!/bin/bash

num_ficheros=$1
dir=$PWD

fich_size=$(du -a -b | sort -n -r | awk '{print $2}')

#echo $fich_size
if [ $# -gt 1 ]
then 
	echo "Error: usage error"
	exit 1
fi

for x in $fich_size 
do 
	if(test -f $x)
	then 
		du -b $x | awk '{print $2"	"$1}' 
	fi
done | sed $1q
