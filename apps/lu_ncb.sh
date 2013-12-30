#!/bin/bash
source bench.sh
EXEC=lu_ncb
DIR=splash2
INPUT="-n1024 -b16 -p4"
REPEATS=$2
MODEL=$1
FILENAME=$0
FILENAME=${FILENAME}.$1


pushd $DIR
rm -f ${FILENAME}
for i in `seq $REPEATS`
do
	/usr/bin/time -p ./$MODEL/$EXEC $INPUT 2>&1 | egrep "^real [0-9]+\.[0-9]{2}" | sed "s/^real //g" | tee -a ${FILENAME}
done
after_run ${FILENAME}
popd
