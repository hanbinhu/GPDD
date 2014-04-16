#!/bin/sh
cleanpath=$(cd "$(dirname "$0")"; pwd)
cd $cleanpath/basic
rm -rf *.out
cd $cleanpath/RCtest
rm -rf *.out
cd $cleanpath/two_stage
rm -rf *.out
