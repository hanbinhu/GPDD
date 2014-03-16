#!/bin/sh
cleanpath=$(cd "$(dirname "$0")"; pwd)
cd $cleanpath/basic
rm -f *~ *.out
