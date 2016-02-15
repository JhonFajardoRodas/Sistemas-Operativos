#!/bin/bash


array_files=( $1 $2 $3 $4 )
file_notas="notas.txt"
rm $file_notas
for file in ${array_files[@]}
do
  cat $file | awk '{print $1"	"$2}' >> $file_notas
done

nombres=`awk '{print $1}' $file_notas | sort | uniq`


for alumno in $nombres
do
  num_aprobado=`awk '$1 ~ /^'$alumno'$/ && $2 ~ /si/' $file_notas | wc -l`
  if [ $num_aprobado -gt 1 ]
  then
    echo $alumno "si"
  else
    echo $alumno "no"
  fi
done
