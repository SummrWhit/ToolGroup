#! bin/sh

value1=`./main`
value2=$?
value3=2

echo  "value1 = $value1"
echo  "lucky number = $value2"
#echo  "value3 = $value3"

#INPUT=List.csv
#OLDIFS=$IFS
#IFS=','
#[ ! -f $INPUT ] && { echo "$INPUT file not found"; exit 99; }
#while read number name
#do
#	echo "Number : $number"
#	echo "Name : $name"
#done < $INPUT
#IFS=$OLDIFS
echo "lucky person is:"
cat List.csv |grep $value3 |awk  -F, '{print $2}'

