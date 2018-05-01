wordlist="A B C D E F"

for word in $wordlist
do
	echo $word
done

filelist=`ls`

for file in $filelist
do
	ls -l $file
done

for arg
do
	echo $arg
done
