#!/bin/sh
#
# hamsterdb test script
#
# Copyright (C) 2005-2007, Christoph Rupp (chris@crupp.de). 
# All rights reserved. See file LICENSE for license and copyright 
# information.
#

total=0
errors=0

if [ -e ./test ]
    then executable=./test
    else executable=./test.exe
fi

testdir=../../testfiles

echo "----------------------------------------------------------------"
echo -n "new test starts on "
date
echo "parameters: ($1) $2 $3 $4 $5 $6 $7 "

maxdir=$1; shift

for dir in `echo "1 2 3 4"`
do
    for file in `ls $testdir/$dir/*.tst`
    do
        #rm -rf test-ham.db

        # call $file  $1 $2 $3 $4 $5 $6 $7 
        $executable $1 $2 $3 $4 $5 $6 $7 $8 $9 $file

        if [ $? = 0 ]
            then echo "[OK]   $1 $2 $3 $4 $5 $6 $7 $8 $9 $file"
            else echo "[FAIL] $1 $2 $3 $4 $5 $6 $7 $8 $9 $file"; errors=$(($errors+1))
        fi
        total=$(($total+1))
    done

    if [ $dir = $maxdir ]; then
        echo "$total tests; $errors error(s)"
        exit $errors
    fi
done

echo "$total tests; $errors error(s)"
exit $errors
