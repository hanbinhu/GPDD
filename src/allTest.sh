#!/bin/sh
rm -f TR.res
srcpath=$(cd "$(dirname "$0")"; pwd)
basicpath=$srcpath/../test/basic
testpath=$srcpath/../test
sh $testpath/clean.sh
find $basicpath -name "*.cir" -exec ./GPDD {} \; >> TR.res
sh $basicpath/compare.sh >> TR.res
echo "Result has been written to TR.res."
diff -s TR.res TR.mdl
