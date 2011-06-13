#!/bin/sh
#
# hamsterdb test script
#
# Copyright (C) 2005-2011, Christoph Rupp (chris@crupp.de). 
# All rights reserved. See file LICENSE for license and copyright 
# information.
#

t=../common/run-tests.sh

\rm -f *.db

$t 3 --reopen $1 $2 $3 $4 $5
$t 3 --fullcheck-find $1 $2 $3 $4 $5
$t 3 --reopen --useralloc $1 $2 $3 $4 $5
$t 3 --fullcheck-find --useralloc $1 $2 $3 $4 $5
$t 3 --overwrite --reopen $1 $2 $3 $4 $5
$t 3 --overwrite --reopen --useralloc $1 $2 $3 $4 $5
$t 3 --inmemorydb $1 $2 $3 $4 $5
$t 3 --inmemorydb --useralloc $1 $2 $3 $4 $5
$t 3 --overwrite --inmemorydb $1 $2 $3 $4 $5
$t 3 --use-cursors --reopen $1 $2 $3 $4 $5
$t 3 --use-cursors --overwrite --reopen $1 $2 $3 $4 $5
$t 3 --use-cursors --inmemorydb $1 $2 $3 $4 $5
$t 3 --use-cursors --overwrite --inmemorydb $1 $2 $3 $4 $5
$t 3 --use-cursors --duplicate-first --reopen $1 $2 $3 $4 $5
$t 3 --use-aes --reopen $1 $2 $3 $4 $5
$t 3 --use-compression --reopen $1 $2 $3 $4 $5

$t 3 --fullcheck-backwards --inmemorydb $1 $2 $3 $4 $5
$t 3 --fullcheck-backwards --reopen $1 $2 $3 $4 $5
$t 3 --fullcheck-backwards --duplicate --reopen $1 $2 $3 $4 $5
$t 3 --fullcheck-backwards --overwrite --reopen $1 $2 $3 $4 $5

$t 3 --duplicate --reopen $1 $2 $3 $4 $5
$t 3 --fullcheck-find $1 $2 $3 $4 $5
$t 3 --duplicate --reopen --useralloc $1 $2 $3 $4 $5
$t 3 --duplicate --fullcheck-find --useralloc $1 $2 $3 $4 $5
$t 3 --duplicate --inmemorydb $1 $2 $3 $4 $5
$t 3 --duplicate --inmemorydb --useralloc $1 $2 $3 $4 $5
$t 3 --duplicate --use-cursors --reopen $1 $2 $3 $4 $5
$t 3 --duplicate --use-cursors --inmemorydb $1 $2 $3 $4 $5

$t 3 --duplicate --sort-duplicates --reopen $1 $2 $3 $4 $5
$t 3 --duplicate --sort-duplicates --reopen --useralloc $1 $2 $3 $4 $5
$t 3 --duplicate --sort-duplicates --fullcheck-find --useralloc $1 $2 $3 $4 $5
$t 3 --duplicate --sort-duplicates --inmemorydb $1 $2 $3 $4 $5
$t 3 --duplicate --sort-duplicates --inmemorydb --useralloc $1 $2 $3 $4 $5
$t 3 --duplicate --sort-duplicates --use-cursors --reopen $1 $2 $3 $4 $5
$t 3 --duplicate --sort-duplicates --use-cursors --inmemorydb $1 $2 $3 $4 $5

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
$t 3 --use-recovery --pagesize024 --reopen $1 $2 $3 $4 $5
$t 3 --use-recovery --duplicate --reopen $1 $2 $3 $4 $5
$t 3 --use-recovery --overwrite --disable-mmap --reopen $1 $2 $3 $4 $5
$t 3 --use-recovery --keysize6 --reopen $1 $2 $3 $4 $5

# HAM_DAM_SEQUENTIAL_INSERT
$t 3 --data-access-mode=HAM_DAM_SEQUENTIAL_INSERT --keysize=8 --reopen $1 $2 $3 $4 $5
$t 3 --data-access-mode=HAM_DAM_SEQUENTIAL_INSERT --cachesize=50 --reopen $1 $2 $3 $4 $5
$t 3 --data-access-mode=HAM_DAM_SEQUENTIAL_INSERT --keysize=680 --duplicate $1 $2 $3 $4 $5
$t 3 --data-access-mode=HAM_DAM_SEQUENTIAL_INSERT --keysize=680 --overwrite $1 $2 $3 $4 $5
$t 3 --data-access-mode=HAM_DAM_SEQUENTIAL_INSERT --pagesize024 --overwrite --keysize6 $1 $2 $3 $4 $5

# HAM_DAM_RANDOM_WRITE
$t 3 --data-access-mode=HAM_DAM_RANDOM_WRITE --keysize=8 --reopen $1 $2 $3 $4 $5
$t 3 --data-access-mode=HAM_DAM_RANDOM_WRITE --cachesize=50 --reopen $1 $2 $3 $4 $5
$t 3 --data-access-mode=HAM_DAM_RANDOM_WRITE --keysize=680 --duplicate $1 $2 $3 $4 $5
$t 3 --data-access-mode=HAM_DAM_RANDOM_WRITE --keysize=680 --overwrite $1 $2 $3 $4 $5
$t 3 --data-access-mode=HAM_DAM_RANDOM_WRITE --pagesize024 --overwrite --keysize6 $1 $2 $3 $4 $5

# HAM_HINT_APPEND
$t 3 --hints=HAM_HINT_APPEND --use-cursors --reopen $1 $2 $3 $4 $5
$t 3 --hints=HAM_HINT_APPEND --use-cursors --inmemorydb $1 $2 $3 $4 $5

# HAM_HINT_PREPEND
$t 3 --hints=HAM_HINT_PREPEND --use-cursors --reopen $1 $2 $3 $4 $5
$t 3 --hints=HAM_HINT_PREPEND --use-cursors --inmemorydb $1 $2 $3 $4 $5

# HAM_DIRECT_ACCESS
$t 3 --direct-access --inmemorydb $1 $2 $3 $4 $5
$t 3 --direct-access --inmemorydb --fullcheck-find $1 $2 $3 $4 $5
$t 3 --direct-access --inmemorydb --useralloc $1 $2 $3 $4 $5
$t 3 --direct-access --inmemorydb --overwrite $1 $2 $3 $4 $5
$t 3 --direct-access --inmemorydb --overwrite --useralloc $1 $2 $3 $4
$t 3 --direct-access --inmemorydb --use-cursors $1 $2 $3 $4 $5
$t 3 --direct-access --inmemorydb --use-cursors --overwrite $1 $2 $3
$t 3 --direct-access --inmemorydb --use-cursors --useralloc $1 $2 $3
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
$t 2 --use-transactions=tmp --use-aes $1 $2 $3 $4 $5
$t 2 --use-transactions=tmp --use-compression $1 $2 $3 $4 $5
$t 2 --use-transactions=tmp --disable-mmap $1 $2 $3 $4 $5
$t 2 --use-transactions=tmp --fullcheck-find $1 $2 $3 $4 $5
$t 2 --use-transactions=tmp --fullcheck-backwards $1 $2 $3 $4 $5
$t 2 --use-transactions=tmp --useralloc $1 $2 $3 $4 $5
$t 2 --use-transactions=tmp --useralloc --use-cursors $1 $2 $3 $4 $5
$t 2 --use-transactions=tmp --pagesize=1024 $1 $2 $3 $4 $5
$t 2 --use-transactions=tmp --pagesize=10240 $1 $2 $3 $4 $5
$t 2 --use-transactions=tmp --pagesize=102400 $1 $2 $3 $4 $5
$t 2 --use-transactions=tmp --cachesize=1024 $1 $2 $3 $4 $5
$t 2 --use-transactions=tmp --cachesize=102400 $1 $2 $3 $4 $5
$t 2 --use-transactions=tmp --cachesize=10240000 $1 $2 $3 $4 $5

# use transactions: group 20 ops in one txn
$t 2 --use-transactions=20 $1 $2 $3 $4 $5
$t 2 --use-transactions=20 --overwrite $1 $2 $3 $4 $5
$t 2 --use-transactions=20 --duplicate $1 $2 $3 $4 $5
$t 2 --use-transactions=20 --use-cursors $1 $2 $3 $4 $5
$t 2 --use-transactions=20 --use-cursors --overwrite $1 $2 $3 $4 $5
$t 2 --use-transactions=20 --use-cursors --duplicate $1 $2 $3 $4 $5
$t 2 --use-transactions=20 --use-cursors --duplicate-first $1 $2 $3 $4 $5
$t 2 --use-transactions=20 --use-aes $1 $2 $3 $4 $5
$t 2 --use-transactions=20 --use-compression $1 $2 $3 $4 $5
$t 2 --use-transactions=20 --disable-mmap $1 $2 $3 $4 $5
$t 2 --use-transactions=20 --fullcheck-find $1 $2 $3 $4 $5
$t 2 --use-transactions=20 --fullcheck-backwards $1 $2 $3 $4 $5
$t 2 --use-transactions=20 --useralloc $1 $2 $3 $4 $5
$t 2 --use-transactions=20 --useralloc --use-cursors $1 $2 $3 $4 $5
$t 2 --use-transactions=20 --pagesize=1024 $1 $2 $3 $4 $5
$t 2 --use-transactions=20 --pagesize=10240 $1 $2 $3 $4 $5
$t 2 --use-transactions=20 --pagesize=102400 $1 $2 $3 $4 $5
$t 2 --use-transactions=20 --cachesize=1024 $1 $2 $3 $4 $5
$t 2 --use-transactions=20 --cachesize=102400 $1 $2 $3 $4 $5
$t 2 --use-transactions=20 --cachesize=10240000 $1 $2 $3 $4 $5

# use transactions: group 200 ops in one txn
$t 2 --use-transactions=200 $1 $2 $3 $4 $5
$t 2 --use-transactions=200 --overwrite $1 $2 $3 $4 $5
$t 2 --use-transactions=200 --duplicate $1 $2 $3 $4 $5
$t 2 --use-transactions=200 --use-cursors $1 $2 $3 $4 $5
$t 2 --use-transactions=200 --use-cursors --overwrite $1 $2 $3 $4 $5
$t 2 --use-transactions=200 --use-cursors --duplicate $1 $2 $3 $4 $5
$t 2 --use-transactions=200 --use-cursors --duplicate-first $1 $2 $3 $4 $5
$t 2 --use-transactions=200 --use-aes $1 $2 $3 $4 $5
$t 2 --use-transactions=200 --use-compression $1 $2 $3 $4 $5
$t 2 --use-transactions=200 --disable-mmap $1 $2 $3 $4 $5
$t 2 --use-transactions=200 --fullcheck-find $1 $2 $3 $4 $5
$t 2 --use-transactions=200 --fullcheck-backwards $1 $2 $3 $4 $5
$t 2 --use-transactions=200 --useralloc $1 $2 $3 $4 $5
$t 2 --use-transactions=200 --useralloc --use-cursors $1 $2 $3 $4 $5
$t 2 --use-transactions=200 --pagesize=1024 $1 $2 $3 $4 $5
$t 2 --use-transactions=200 --pagesize=10240 $1 $2 $3 $4 $5
$t 2 --use-transactions=200 --pagesize=102400 $1 $2 $3 $4 $5
$t 2 --use-transactions=200 --cachesize=1024 $1 $2 $3 $4 $5
$t 2 --use-transactions=200 --cachesize=102400 $1 $2 $3 $4 $5
$t 2 --use-transactions=200 --cachesize=10240000 $1 $2 $3 $4 $5

# use transactions: group 10000 ops in one txn
$t 2 --use-transactions=10000 $1 $2 $3 $4 $5
$t 2 --use-transactions=10000 --overwrite $1 $2 $3 $4 $5
$t 2 --use-transactions=10000 --duplicate $1 $2 $3 $4 $5
$t 2 --use-transactions=10000 --use-cursors $1 $2 $3 $4 $5
$t 2 --use-transactions=10000 --use-cursors --overwrite $1 $2 $3 $4 $5
$t 2 --use-transactions=10000 --use-cursors --duplicate $1 $2 $3 $4 $5
$t 2 --use-transactions=10000 --use-cursors --duplicate-first $1 $2 $3 $4 $5
$t 2 --use-transactions=10000 --use-aes $1 $2 $3 $4 $5
$t 2 --use-transactions=10000 --use-compression $1 $2 $3 $4 $5
$t 2 --use-transactions=10000 --disable-mmap $1 $2 $3 $4 $5
$t 2 --use-transactions=10000 --fullcheck-find $1 $2 $3 $4 $5
$t 2 --use-transactions=10000 --fullcheck-backwards $1 $2 $3 $4 $5
$t 2 --use-transactions=10000 --useralloc $1 $2 $3 $4 $5
$t 2 --use-transactions=10000 --useralloc --use-cursors $1 $2 $3 $4 $5
$t 2 --use-transactions=10000 --pagesize=1024 $1 $2 $3 $4 $5
$t 2 --use-transactions=10000 --pagesize=10240 $1 $2 $3 $4 $5
$t 2 --use-transactions=10000 --pagesize=102400 $1 $2 $3 $4 $5
$t 2 --use-transactions=10000 --cachesize=1024 $1 $2 $3 $4 $5
$t 2 --use-transactions=10000 --cachesize=102400 $1 $2 $3 $4 $5
$t 2 --use-transactions=10000 --cachesize=10240000 $1 $2 $3 $4 $5

# use transactions: group ALL ops in one txn
$t 2 --use-transactions=all $1 $2 $3 $4 $5
$t 2 --use-transactions=all --overwrite $1 $2 $3 $4 $5
$t 2 --use-transactions=all --duplicate $1 $2 $3 $4 $5
$t 2 --use-transactions=all --use-cursors $1 $2 $3 $4 $5
$t 2 --use-transactions=all --use-cursors --overwrite $1 $2 $3 $4 $5
$t 2 --use-transactions=all --use-cursors --duplicate $1 $2 $3 $4 $5
$t 2 --use-transactions=all --use-cursors --duplicate-first $1 $2 $3 $4 $5
$t 2 --use-transactions=all --use-aes $1 $2 $3 $4 $5
$t 2 --use-transactions=all --use-compression $1 $2 $3 $4 $5
$t 2 --use-transactions=all --disable-mmap $1 $2 $3 $4 $5
$t 2 --use-transactions=all --fullcheck-find $1 $2 $3 $4 $5
$t 2 --use-transactions=all --fullcheck-backwards $1 $2 $3 $4 $5
$t 2 --use-transactions=all --useralloc $1 $2 $3 $4 $5
$t 2 --use-transactions=all --useralloc --use-cursors $1 $2 $3 $4 $5
$t 2 --use-transactions=all --pagesize=1024 $1 $2 $3 $4 $5
$t 2 --use-transactions=all --pagesize=10240 $1 $2 $3 $4 $5
$t 2 --use-transactions=all --pagesize=102400 $1 $2 $3 $4 $5
$t 2 --use-transactions=all --cachesize=1024 $1 $2 $3 $4 $5
$t 2 --use-transactions=all --cachesize=102400 $1 $2 $3 $4 $5
$t 2 --use-transactions=all --cachesize=10240000 $1 $2 $3 $4 $5
