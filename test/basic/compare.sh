#!/bin/sh

basicpath=$(cd "$(dirname "$0")"; pwd)
answerpath=$basicpath/../answer
cd $answerpath
ls *.out > $basicpath/tmp
cd $basicpath
cat tmp | while read line
do
	echo "Comparing $line..."
	if [ -f $line ]; then
		diff -s $line ../answer/$line
	else
		echo "$line doesn't exist."
	fi
done
rm tmp
