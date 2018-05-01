echo -n "Input a score : "

read score

if [ $score -lt 0 -o $score -gt 100 ]
then
	echo "Input a invalid score:$score"
else
	score=`expr $score / 10`
	case $score in 
		8 | 9 | 10)
			echo "A"
			;;
		6 | 7 )
			echo "B"
			;;
		*)
			echo "C"
	esac
fi
