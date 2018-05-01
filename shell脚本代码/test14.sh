
echo "Input  : "

read var

case $var in 
	100)
		echo "100"
		;;
	200)
		echo "200"
		;;

	"yes" | "y" | "YES")
		echo "yes"
		;;
	 *)
		 echo "default"
		 ;;
esac
