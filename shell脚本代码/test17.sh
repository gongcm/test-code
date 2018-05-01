if [ -d $HOME/file-dir ]
then
	echo -n "$HOME/file-dir exist!,doo you want to delete file-dir? [Y/N]"
	read ans
	
	case $ans in 
		"y"|"Y"|"yes"|"YES")
			rm -rf $HOME/file-dir 
			;;
		*)
			exit
	esac
fi


if [ -d $HOME/dir-dir ]
then
	echo -n "$HOME/dir-dir is exist ,do you want to delete dir-dir? [Y/N]"
	read ans
	
	case $ans in 
		"y"|"Y"|"yes"|"YES")
			rm -rf $HOME/dir-dir 
			;;
	*)
		exit
	esac
fi


mkdir $HOME/file-dir 
mkdir $HOME/dir-dir 


echo -n "Input a directory with path  : "
read  path_dir

file_list=`ls $path_dir`
file_count=0
dir_count=0

#for file in $file_list
#do
#	if [ -d $file ]
#	then
#		cp $file $HOME/dir-dir -a
#		dir_count=`expr $dir_count + 1`
#	else
#		cp $file $HOME/file-dir
#		file_count=`expr $file_count + 1`
#	fi
#done

for file in $file_list
do
	if [ -d $path_dir/$file ]
	then
		cp $path_dir/$file $HOME/dir-dir -a
		dir_count=`expr $dir_count + 1`
	else
		cp $path_dir/$file $HOME/file-dir
		file_count=`expr $file_count + 1`
	fi
done

echo "copy $file_count file!"
echo "copy $dir_count  directory!"
