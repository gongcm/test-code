echo -n "Input username : "

read username

line=`cat /etc/passwd | cut -d ":" -f 1 | grep "^$username$" -n | cut -d ":" -f 1`

head -$line /etc/passwd | tail -1 | cut -d ":" -f 1,3,4

#userinfo=`head -$line /etc/passwd | tail -1 | cut -d ":" -f 1,3,4`
#echo $userinfo
