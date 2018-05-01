echo -n "Input a data :"

read data

if [ $data -ge 512 ]
then
	data=`expr $data / 2`
	echo $data
fi

if [ $data -lt 256 ]
then 
	data=`expr $data \* 3`
	echo $data
fi

if [ $data -ge 256 -a $data -lt 512 ]
then
	echo $data
fi

echo $data >> result.txt
