echo -n "Are you sure ?[y|n]"
read ans

if [ $ans == "y" ]
then
	echo "yes"
fi


if [ $ans != "y" ]
then
	echo "no"
fi
