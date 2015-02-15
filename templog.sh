#!/bin/bash

cat /dev/ttyACM0 | while read line
do
# echo $line
 size=${#line}
# echo $size
 if [ $size != 18 ] && [ $size != 19 ] ; then
#  echo "retry"
  :
 else
#  echo "writing"
  t=`date '+%Y-%m-%d %H:%M:%S'`
  echo ${line},$t > /media/IMATION\ USB/rttmp.txt
 fi
done
