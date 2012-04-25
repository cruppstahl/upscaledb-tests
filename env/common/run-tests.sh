#!/bin/sh
#
# hamsterdb test script
#
# Copyright (C) 2005-2011, Christoph Rupp (chris@crupp.de). 
# All rights reserved. See file LICENSE for license and copyright 
# information.
#

total=0
errors=0
success=0

if [ -e ./test ]
    then executable=./test
    else executable=./test.exe
fi

testdir=../../testfiles

echo "----------------------------------------------------------------"
echo "[CONFIGURATION] $2 $3 $4 $5 $6 $7 "

maxdir=$1; shift

for dir in `echo "1 2 3 4 5 6 7 8 9 10"`
do
    for file in `ls $testdir/$dir/*.tst`
    do
        # call $file  $1 $2 $3 $4 $5 $6 $7 
        echo "starting" $executable $1 $2 $3 $4 $5 $6 $7 $8 $9 $file
        $executable $1 $2 $3 $4 $5 $6 $7 $8 $9 $file

        if [ $? = 0 ]
        then 
            success=$(($success+1))
        else
            errors=$(($errors+1))
            echo "[FAIL]"
        fi
        total=$(($total+1))
    done

    if [ $dir = $maxdir ]; then
        #echo "$total tests; $errors error(s)"
        exit $errors
    fi
done

echo "[RESULT] $total tests; $errors error(s)"
exit $errors
