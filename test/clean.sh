#!/bin/bash
cleanpath=$(cd "$(dirname "$0")"; pwd)
find $cleanpath/basic -name "*.out*" | xargs rm -vf
find $cleanpath/tmp -name "*" | xargs rm -vf
find $cleanpath/RCtest -name "*.out*" | xargs rm -vf
find $cleanpath/two_stage -name "*.out*" | xargs rm -vf
find $cleanpath/two_stage_vbias -name "*.out*" | xargs rm -vf
