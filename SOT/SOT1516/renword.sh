#!/bin/bash





a=$PWD
b=$(ls $a|grep -v .sh)

for fich in $b
do
	if (! test -d $fich)	
	then 
		while read linea
		do 		

			OIFS="$IFS"
			IFS=' '
			read -a Array <<< "${linea}"
			IFS="$OIFS"
			if [ "${Array[0]}" == "$1" ]
			then
				file_new=$fich
		  		file_new+="."
		  		file_new+=$1
				mv $fich $file_new 
				
			fi

		done < $fich

	fi
done