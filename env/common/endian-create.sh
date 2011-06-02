#!/bin/sh

if [ -e ./test ]
    then executable=./test
    else executable=./test.exe
fi

$executable ../../testfiles/1/45.tst
mv test-berk.db berk-45.db
mv test-ham.db ham-45.db

$executable ../../testfiles/1/220.tst
mv test-berk.db berk-220.db
mv test-ham.db ham-220.db

$executable ../../testfiles/1/ext_020.tst
mv test-berk.db berk-ext020.db
mv test-ham.db ham-ext020.db

$executable ../../testfiles/1/blb-002.tst
mv test-berk.db berk-blb002.db
mv test-ham.db ham-blb002.db

$executable ../../testfiles/1/45.tst --pagesize=1024 --keysize=12
mv test-berk.db berk-45-ps1024-ks12.db
mv test-ham.db ham-45-ps1024-ks12.db

$executable ../../testfiles/1/220.tst --pagesize=1024 --keysize=12
mv test-berk.db berk-220-ps1024-ks12.db
mv test-ham.db ham-220-ps1024-ks12.db

$executable ../../testfiles/1/ext_020.tst --pagesize=1024 --keysize=12
mv test-berk.db berk-ext020-ps1024-ks12.db
mv test-ham.db ham-ext020-ps1024-ks12.db

$executable ../../testfiles/1/blb-002.tst --pagesize=1024 --keysize=24
mv test-berk.db berk-blb002-ps1024-ks12.db
mv test-ham.db ham-blb002-ps1024-ks24.db

$executable ../../testfiles/2/ext_061.tst --pagesize=8192 
mv test-berk.db berk-ext061-ps8192.db
mv test-ham.db ham-ext061-ps8192.db

$executable ../../testfiles/2/ext_061.tst --pagesize=8192 --overwrite=1
mv test-berk.db berk-ext061-ps8192-over.db
mv test-ham.db ham-ext061-ps8192-over.db

$executable ../../testfiles/1/220.tst --duplicate=1
mv test-berk.db berk-220-dupe.db
mv test-ham.db ham-220-dupe.db

tar -cvf endian.tar *.db
gzip endian.tar
rm *.db

echo "------------ fertig! --------------------"
ls -l endian.tar.gz

