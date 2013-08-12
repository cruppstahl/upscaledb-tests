#!/bin/sh
#
# hamsterdb test script
#
# Copyright (C) 2005-2012, Christoph Rupp (chris@crupp.de). 
# All rights reserved. See file LICENSE for license and copyright 
# information.
#

t=../common/run-tests.sh

\rm -f *.db

# AES encryption
$t 3 --enable-encryption --reopen $1 $2 $3 $4 $5
$t 3 --enable-encryption --use-transactions=10 --reopen $1 $2 $3 $4 $5

# client/server
$t 3 --enable-remote --reopen $1 $2 $3 $4 $5
$t 3 --enable-remote --use-transactions=10 --reopen $1 $2 $3 $4 $5

$t 3 --reopen $1 $2 $3 $4 $5
$t 3 --fullcheck-find $1 $2 $3 $4 $5
$t 3 --overwrite --reopen $1 $2 $3 $4 $5
$t 3 --inmemorydb $1 $2 $3 $4 $5
$t 3 --overwrite --inmemorydb $1 $2 $3 $4 $5
$t 3 --use-cursors --reopen $1 $2 $3 $4 $5
$t 3 --use-cursors --overwrite --reopen $1 $2 $3 $4 $5
$t 3 --use-cursors --inmemorydb $1 $2 $3 $4 $5
$t 3 --use-cursors --overwrite --inmemorydb $1 $2 $3 $4 $5
$t 3 --use-cursors --duplicate-first --reopen $1 $2 $3 $4 $5

$t 3 --fullcheck-backwards --inmemorydb $1 $2 $3 $4 $5
$t 3 --fullcheck-backwards --reopen $1 $2 $3 $4 $5
$t 3 --fullcheck-backwards --duplicate --reopen $1 $2 $3 $4 $5
$t 3 --fullcheck-backwards --overwrite --reopen $1 $2 $3 $4 $5

$t 3 --duplicate --reopen $1 $2 $3 $4 $5
$t 3 --fullcheck-find $1 $2 $3 $4 $5
$t 3 --duplicate --inmemorydb $1 $2 $3 $4 $5
$t 3 --duplicate --use-cursors --reopen $1 $2 $3 $4 $5
$t 3 --duplicate --use-cursors --inmemorydb $1 $2 $3 $4 $5

$t 3 --disable-mmap --reopen $1 $2 $3 $4 $5
$t 3 --disable-mmap --overwrite --reopen $1 $2 $3 $4 $5

$t 3 --keysize=8 --reopen $1 $2 $3 $4 $5
$t 3 --keysize=8 --overwrite --reopen $1 $2 $3 $4 $5
$t 3 --keysize=8 --inmemorydb $1 $2 $3 $4 $5
$t 3 --keysize=8 --overwrite --inmemorydb $1 $2 $3 $4 $5
$t 3 --keysize=8 --use-cursors --overwrite --inmemorydb $1 $2 $3 $4 $5

$t 3 --keysize=12 --reopen $1 $2 $3 $4 $5
$t 3 --keysize=12 --overwrite --reopen $1 $2 $3 $4 $5
$t 3 --keysize=12 --inmemorydb $1 $2 $3 $4 $5
$t 3 --keysize=12 --overwrite --inmemorydb $1 $2 $3 $4 $5

$t 3 --keysize=33 --reopen $1 $2 $3 $4 $5
$t 3 --keysize=33 --overwrite --reopen $1 $2 $3 $4 $5
$t 3 --keysize=33 --inmemorydb $1 $2 $3 $4 $5
$t 3 --keysize=33 --overwrite --inmemorydb $1 $2 $3 $4 $5

$t 3 --keysize=680 --reopen $1 $2 $3 $4 $5
$t 3 --keysize=680 --overwrite --reopen $1 $2 $3 $4 $5
$t 3 --keysize=680 --inmemorydb $1 $2 $3 $4 $5
$t 3 --keysize=680 --overwrite --inmemorydb $1 $2 $3 $4 $5

$t 3 --pagesize=1024 --reopen $1 $2 $3 $4 $5
$t 3 --pagesize=1024 --overwrite --reopen $1 $2 $3 $4 $5
$t 3 --pagesize=1024 --inmemorydb $1 $2 $3 $4 $5
$t 3 --pagesize=1024 --overwrite --inmemorydb $1 $2 $3 $4 $5

$t 3 --pagesize=8192 --reopen $1 $2 $3 $4 $5
$t 3 --pagesize=8192 --overwrite --reopen $1 $2 $3 $4 $5
$t 3 --pagesize=8192 --inmemorydb $1 $2 $3 $4 $5
$t 3 --pagesize=8192 --overwrite --inmemorydb $1 $2 $3 $4 $5

$t 3 --pagesize=131072 --reopen $1 $2 $3 $4 $5
$t 3 --pagesize=131072 --overwrite --reopen $1 $2 $3 $4 $5
$t 3 --pagesize=131072 --inmemorydb $1 $2 $3 $4 $5
$t 3 --pagesize=131072 --overwrite --inmemorydb $1 $2 $3 $4 $5

$t 3 --cacheunlimited --reopen $1 $2 $3 $4 $5
$t 3 --cachesize=0 --reopen $1 $2 $3 $4 $5
$t 3 --cachesize=0 --overwrite --reopen $1 $2 $3 $4 $5
$t 3 --cachesize=0 --disable-mmap --reopen $1 $2 $3 $4 $5
$t 3 --cachesize=0 --overwrite --disable-mmap --reopen $1 $2 $3 $4 $5

$t 3 --cachesize --reopen $1 $2 $3 $4 $5
$t 3 --cachesize --overwrite --reopen $1 $2 $3 $4 $5
$t 3 --cachesize --disable-mmap --reopen $1 $2 $3 $4 $5
$t 3 --cachesize --overwrite --disable-mmap --reopen $1 $2 $3 $4 $5
$t 3 --cachesize --use-cursors --overwrite --disable-mmap --reopen $1 $2 $3 $4 $5

$t 3 --cachesize=50 --reopen $1 $2 $3 $4 $5
$t 3 --cachesize=50 --overwrite --reopen $1 $2 $3 $4 $5
$t 3 --cachesize=50 --disable-mmap --reopen $1 $2 $3 $4 $5
$t 3 --cachesize=50 --overwrite --disable-mmap --reopen $1 $2 $3 $4 $5
$t 3 --cachesize=50 --use-cursors --overwrite --disable-mmap --reopen $1 $2 $3 $4 $5

$t 3 --cachesize=1024 --reopen $1 $2 $3 $4 $5
$t 3 --cachesize=1024 --overwrite --reopen $1 $2 $3 $4 $5
$t 3 --cachesize=1024 --disable-mmap --reopen $1 $2 $3 $4 $5
$t 3 --cachesize=1024 --overwrite --disable-mmap --reopen $1 $2 $3 $4 $5

$t 3 --use-recovery --reopen $1 $2 $3 $4 $5
$t 3 --use-recovery --overwrite --reopen $1 $2 $3 $4 $5
$t 3 --use-recovery --pagesize=1024 --reopen $1 $2 $3 $4 $5
$t 3 --use-recovery --duplicate --reopen $1 $2 $3 $4 $5
$t 3 --use-recovery --overwrite --disable-mmap --reopen $1 $2 $3 $4 $5
$t 3 --use-recovery --keysize=16 --reopen $1 $2 $3 $4 $5

# HAM_HINT_APPEND
$t 3 --hints=HAM_HINT_APPEND --use-cursors --reopen $1 $2 $3 $4 $5
$t 3 --hints=HAM_HINT_APPEND --use-cursors --inmemorydb $1 $2 $3 $4 $5

# HAM_HINT_PREPEND
$t 3 --hints=HAM_HINT_PREPEND --use-cursors --reopen $1 $2 $3 $4 $5
$t 3 --hints=HAM_HINT_PREPEND --use-cursors --inmemorydb $1 $2 $3 $4 $5

# HAM_DIRECT_ACCESS
$t 3 --direct-access --inmemorydb $1 $2 $3 $4 $5
$t 3 --direct-access --inmemorydb --fullcheck-find $1 $2 $3 $4 $5
$t 3 --direct-access --inmemorydb --overwrite $1 $2 $3 $4 $5
$t 3 --direct-access --inmemorydb --use-cursors $1 $2 $3 $4 $5
$t 3 --direct-access --inmemorydb --use-cursors --overwrite $1 $2 $3
$t 3 --direct-access --inmemorydb --use-cursors --duplicate-first $1 $2 $3 $4 $5
$t 3 --direct-access --inmemorydb --fullcheck-backwards $1 $2 $3 $4 $5

# use temporary transactions
$t 2 --use-transactions=tmp $1 $2 $3 $4 $5
$t 2 --use-transactions=tmp --overwrite $1 $2 $3 $4 $5
$t 2 --use-transactions=tmp --duplicate $1 $2 $3 $4 $5
$t 2 --use-transactions=tmp --use-cursors $1 $2 $3 $4 $5
$t 2 --use-transactions=tmp --use-cursors --overwrite $1 $2 $3 $4 $5
$t 2 --use-transactions=tmp --use-cursors --duplicate $1 $2 $3 $4 $5
$t 2 --use-transactions=tmp --use-cursors --duplicate-first $1 $2 $3 $4 $5
$t 2 --use-transactions=tmp --disable-mmap $1 $2 $3 $4 $5
$t 2 --use-transactions=tmp --fullcheck-find $1 $2 $3 $4 $5
$t 2 --use-transactions=tmp --fullcheck-backwards $1 $2 $3 $4 $5
$t 2 --use-transactions=tmp --pagesize=1024 $1 $2 $3 $4 $5
$t 2 --use-transactions=tmp --pagesize=10240 $1 $2 $3 $4 $5
$t 2 --use-transactions=tmp --pagesize=102400 $1 $2 $3 $4 $5
$t 2 --use-transactions=tmp --cachesize=1024 $1 $2 $3 $4 $5
$t 2 --use-transactions=tmp --cachesize=102400 $1 $2 $3 $4 $5
$t 2 --use-transactions=tmp --cachesize=10240000 $1 $2 $3 $4 $5

# use transactions: group 5 ops in one txn
$t 2 --use-transactions=5 $1 $2 $3 $4 $5
$t 2 --use-transactions=5 --overwrite $1 $2 $3 $4 $5
$t 2 --use-transactions=5 --duplicate $1 $2 $3 $4 $5
$t 2 --use-transactions=5 --use-cursors $1 $2 $3 $4 $5
$t 2 --use-transactions=5 --use-cursors --overwrite $1 $2 $3 $4 $5
$t 2 --use-transactions=5 --use-cursors --duplicate $1 $2 $3 $4 $5
$t 2 --use-transactions=5 --use-cursors --duplicate-first $1 $2 $3 $4 $5
$t 2 --use-transactions=5 --fullcheck-find $1 $2 $3 $4 $5
$t 2 --use-transactions=5 --fullcheck-backwards $1 $2 $3 $4 $5

# use transactions: group 20 ops in one txn
$t 2 --use-transactions=20 $1 $2 $3 $4 $5
$t 2 --use-transactions=20 --overwrite $1 $2 $3 $4 $5
$t 2 --use-transactions=20 --duplicate $1 $2 $3 $4 $5
$t 2 --use-transactions=20 --use-cursors $1 $2 $3 $4 $5
$t 2 --use-transactions=20 --use-cursors --overwrite $1 $2 $3 $4 $5
$t 2 --use-transactions=20 --use-cursors --duplicate $1 $2 $3 $4 $5
$t 2 --use-transactions=20 --use-cursors --duplicate-first $1 $2 $3 $4 $5
$t 2 --use-transactions=20 --fullcheck-find $1 $2 $3 $4 $5
$t 2 --use-transactions=20 --fullcheck-backwards $1 $2 $3 $4 $5

# use transactions: group 200 ops in one txn
$t 2 --use-transactions=200 $1 $2 $3 $4 $5
$t 2 --use-transactions=200 --overwrite $1 $2 $3 $4 $5
$t 2 --use-transactions=200 --duplicate $1 $2 $3 $4 $5
$t 2 --use-transactions=200 --use-cursors $1 $2 $3 $4 $5
$t 2 --use-transactions=200 --use-cursors --overwrite $1 $2 $3 $4 $5
$t 2 --use-transactions=200 --use-cursors --duplicate $1 $2 $3 $4 $5
$t 2 --use-transactions=200 --use-cursors --duplicate-first $1 $2 $3 $4 $5
$t 2 --use-transactions=200 --fullcheck-find $1 $2 $3 $4 $5
$t 2 --use-transactions=200 --fullcheck-backwards $1 $2 $3 $4 $5

# use transactions: group ALL ops in one txn
$t 2 --use-transactions=all $1 $2 $3 $4 $5
$t 2 --use-transactions=all --overwrite $1 $2 $3 $4 $5
$t 2 --use-transactions=all --duplicate $1 $2 $3 $4 $5
$t 2 --use-transactions=all --use-cursors $1 $2 $3 $4 $5
$t 2 --use-transactions=all --use-cursors --overwrite $1 $2 $3 $4 $5
$t 2 --use-transactions=all --use-cursors --duplicate $1 $2 $3 $4 $5
exit
$t 2 --use-transactions=all --use-cursors --duplicate-first $1 $2 $3 $4 $5
$t 2 --use-transactions=all --fullcheck-find $1 $2 $3 $4 $5
$t 2 --use-transactions=all --fullcheck-backwards $1 $2 $3 $4 $5

# use 2 threads
$t 2 --num-threads=2 $1 $2 $3 $4 $5
$t 2 --num-threads=2 --overwrite $1 $2 $3 $4 $5
$t 2 --num-threads=2 --use-cursors $1 $2 $3 $4 $5
$t 2 --num-threads=2 --use-cursors --duplicate-first $1 $2 $3 $4 $5
$t 2 --num-threads=2 --fullcheck-backwards $1 $2 $3 $4 $5
$t 2 --num-threads=2 --use-transactions=20 $1 $2 $3 $4 $5
$t 2 --num-threads=2 --use-transactions=20 --overwrite $1 $2 $3 $4 $5
$t 2 --num-threads=2 --use-transactions=20 --duplicate $1 $2 $3 $4 $5
$t 2 --num-threads=2 --use-transactions=20 --use-cursors $1 $2 $3 $4 $5
$t 2 --num-threads=2 --use-transactions=20 --use-cursors --overwrite $1 $2 $3 $4 $5
$t 2 --num-threads=2 --use-transactions=20 --use-cursors --duplicate $1 $2 $3 $4 $5
$t 2 --num-threads=2 --use-cursors --duplicate-first $1 $2 $3 $4 $5

# use 10 threads
$t 2 --num-threads=10 $1 $2 $3 $4 $5
$t 1 --num-threads=10 --overwrite $1 $2 $3 $4 $5
$t 1 --num-threads=10 --use-cursors --duplicate $1 $2 $3 $4 $5
$t 1 --num-threads=10 --use-transactions=20 $1 $2 $3 $4 $5

# use 20 threads
$t 2 --num-threads=20 $1 $2 $3 $4 $5
$t 1 --num-threads=20 --disable-mmap --use-transactions=20 --duplicate $1 $2 $3 $4 $5

