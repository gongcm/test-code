echo -n "Input a data :"

read data

if [ $data -ge 512 ]
then
	data=`expr $data / 2`
	echo $data
elif [ $data -lt 256 ]
then 
	data=`expr $data \* 3`
	echo $data
elif [ $data -ge 256 -a $data -lt 512 ]
then
	echo $data
fi

echo $data >> result.txt
