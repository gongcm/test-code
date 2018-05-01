VAR_STRING="hello word"
VAR1=100

VAR2=VAR_STRING
VAR3=$VAR1

#hello word
echo "VAR_STRING : ---$VAR_STRING---"

#100
echo "VAR1 : ---$VAR1---"

#VAR_STRING
echo "VAR2 : ---$VAR2---"

#100
echo "VAR3 : ---$VAR3---"

echo "=============================="

echo "VAR3 : ----${VAR3}CHINA******"


VAR4=$HOME
echo "VAR4 : $VAR4"
