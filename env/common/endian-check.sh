#!/bin/sh

if [ -e ./test ]
    then executable=./test
    else executable=./test.exe
fi

mv berk-45.db test-berk.db
mv ham-45.db test-ham.db
$executable ../../testfiles/fullcheck.tst --numeric=1

mv berk-220.db test-berk.db
mv ham-220.db test-ham.db
$executable ../../testfiles/fullcheck.tst --numeric=1

mv berk-ext020.db test-berk.db
mv ham-ext020.db test-ham.db
$executable ../../testfiles/fullcheck.tst

mv berk-blb002.db test-berk.db
mv ham-blb002.db test-ham.db
$executable ../../testfiles/fullcheck.tst --numeric=1

mv berk-45-ps1024-ks12.db test-berk.db
mv ham-45-ps1024-ks12.db test-ham.db
$executable ../../testfiles/fullcheck.tst --numeric=1

mv berk-220-ps1024-ks12.db test-berk.db
mv ham-220-ps1024-ks12.db test-ham.db
$executable ../../testfiles/fullcheck.tst --numeric=1

mv berk-ext020-ps1024-ks12.db test-berk.db
mv ham-ext020-ps1024-ks12.db test-ham.db
$executable ../../testfiles/fullcheck.tst

mv berk-blb002-ps1024-ks12.db test-berk.db
mv ham-blb002-ps1024-ks24.db test-ham.db
$executable ../../testfiles/fullcheck.tst --numeric=1

mv berk-ext061-ps8192-over.db test-berk.db 
mv ham-ext061-ps8192-over.db test-ham.db 
$executable ../../testfiles/fullcheck.tst

mv berk-220-dupe.db test-berk.db
mv ham-220-dupe.db test-ham.db
$executable ../../testfiles/fullcheck.tst --numeric=1

echo "------------ fertig! --------------------"

