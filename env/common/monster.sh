#!/bin/sh
#
# hamsterdb test script
#
# Copyright (C) 2005-2007, Christoph Rupp (chris@crupp.de). 
# All rights reserved. See file LICENSE for license and copyright 
# information.
#

# run over directories 1-3
d=3

t=../common/run-tests.sh

\rm -f *.db

$t $d --reopen=1 $1 $2 $3 $4 $5
$t $d --fullcheck-find=1 $1 $2 $3 $4 $5
$t $d --reopen=1 --useralloc=1 $1 $2 $3 $4 $5
$t $d --fullcheck-find=1 --useralloc=1 $1 $2 $3 $4 $5
$t $d --overwrite=1 --reopen=1 $1 $2 $3 $4 $5
$t $d --overwrite=1 --reopen=1 --useralloc=1 $1 $2 $3 $4 $5
$t $d --inmemorydb=1 $1 $2 $3 $4 $5
$t $d --inmemorydb=1 --useralloc=1 $1 $2 $3 $4 $5
$t $d --overwrite=1 --inmemorydb=1 $1 $2 $3 $4 $5
$t $d --use-cursors=1 --reopen=1 $1 $2 $3 $4 $5
$t $d --use-cursors=1 --overwrite=1 --reopen=1 $1 $2 $3 $4 $5
$t $d --use-cursors=1 --inmemorydb=1 $1 $2 $3 $4 $5
$t $d --use-cursors=1 --overwrite=1 --inmemorydb=1 $1 $2 $3 $4 $5
$t $d --use-cursors=1 --duplicate-first=1 --reopen=1 $1 $2 $3 $4 $5
$t $d --use-aes=1 --reopen=1 $1 $2 $3 $4 $5
$t $d --use-compression=1 --reopen=1 $1 $2 $3 $4 $5

$t $d --fullcheck-backwards=1 --inmemorydb=1 $1 $2 $3 $4 $5
$t $d --fullcheck-backwards=1 --reopen=1 $1 $2 $3 $4 $5
$t $d --fullcheck-backwards=1 --duplicate=1 --reopen=1 $1 $2 $3 $4 $5
$t $d --fullcheck-backwards=1 --overwrite=1 --reopen=1 $1 $2 $3 $4 $5

$t $d --duplicate=1 --reopen=1 $1 $2 $3 $4 $5
$t $d --fullcheck-find=1 $1 $2 $3 $4 $5
$t $d --duplicate=1 --reopen=1 --useralloc=1 $1 $2 $3 $4 $5
$t $d --duplicate=1 --fullcheck-find=1 --useralloc=1 $1 $2 $3 $4 $5
$t $d --duplicate=1 --inmemorydb=1 $1 $2 $3 $4 $5
$t $d --duplicate=1 --inmemorydb=1 --useralloc=1 $1 $2 $3 $4 $5
$t $d --duplicate=1 --use-cursors=1 --reopen=1 $1 $2 $3 $4 $5
$t $d --duplicate=1 --use-cursors=1 --inmemorydb=1 $1 $2 $3 $4 $5

$t $d --duplicate=1 --sort-duplicates=1 --reopen=1 $1 $2 $3 $4 $5
$t $d --duplicate=1 --sort-duplicates=1 --reopen=1 --useralloc=1 $1 $2 $3 $4 $5
$t $d --duplicate=1 --sort-duplicates=1 --fullcheck-find=1 --useralloc=1 $1 $2 $3 $4 $5
$t $d --duplicate=1 --sort-duplicates=1 --inmemorydb=1 $1 $2 $3 $4 $5
$t $d --duplicate=1 --sort-duplicates=1 --inmemorydb=1 --useralloc=1 $1 $2 $3 $4 $5
$t $d --duplicate=1 --sort-duplicates=1 --use-cursors=1 --reopen=1 $1 $2 $3 $4 $5
$t $d --duplicate=1 --sort-duplicates=1 --use-cursors=1 --inmemorydb=1 $1 $2 $3 $4 $5

$t $d --disable-mmap=1 --reopen=1 $1 $2 $3 $4 $5
$t $d --disable-mmap=1 --overwrite=1 --reopen=1 $1 $2 $3 $4 $5

$t $d --keysize=8 --reopen=1 $1 $2 $3 $4 $5
$t $d --keysize=8 --overwrite=1 --reopen=1 $1 $2 $3 $4 $5
$t $d --keysize=8 --inmemorydb=1 $1 $2 $3 $4 $5
$t $d --keysize=8 --overwrite=1 --inmemorydb=1 $1 $2 $3 $4 $5
$t $d --keysize=8 --use-cursors=1 --overwrite=1 --inmemorydb=1 $1 $2 $3 $4 $5

$t $d --keysize=12 --reopen=1 $1 $2 $3 $4 $5
$t $d --keysize=12 --overwrite=1 --reopen=1 $1 $2 $3 $4 $5
$t $d --keysize=12 --inmemorydb=1 $1 $2 $3 $4 $5
$t $d --keysize=12 --overwrite=1 --inmemorydb=1 $1 $2 $3 $4 $5

$t $d --keysize=33 --reopen=1 $1 $2 $3 $4 $5
$t $d --keysize=33 --overwrite=1 --reopen=1 $1 $2 $3 $4 $5
$t $d --keysize=33 --inmemorydb=1 $1 $2 $3 $4 $5
$t $d --keysize=33 --overwrite=1 --inmemorydb=1 $1 $2 $3 $4 $5

$t $d --keysize=680 --reopen=1 $1 $2 $3 $4 $5
$t $d --keysize=680 --overwrite=1 --reopen=1 $1 $2 $3 $4 $5
$t $d --keysize=680 --inmemorydb=1 $1 $2 $3 $4 $5
$t $d --keysize=680 --overwrite=1 --inmemorydb=1 $1 $2 $3 $4 $5

$t $d --pagesize=1024 --reopen=1 $1 $2 $3 $4 $5
$t $d --pagesize=1024 --overwrite=1 --reopen=1 $1 $2 $3 $4 $5
$t $d --pagesize=1024 --inmemorydb=1 $1 $2 $3 $4 $5
$t $d --pagesize=1024 --overwrite=1 --inmemorydb=1 $1 $2 $3 $4 $5

$t $d --pagesize=8192 --reopen=1 $1 $2 $3 $4 $5
$t $d --pagesize=8192 --overwrite=1 --reopen=1 $1 $2 $3 $4 $5
$t $d --pagesize=8192 --inmemorydb=1 $1 $2 $3 $4 $5
$t $d --pagesize=8192 --overwrite=1 --inmemorydb=1 $1 $2 $3 $4 $5

$t $d --pagesize=131072 --reopen=1 $1 $2 $3 $4 $5
$t $d --pagesize=131072 --overwrite=1 --reopen=1 $1 $2 $3 $4 $5
$t $d --pagesize=131072 --inmemorydb=1 $1 $2 $3 $4 $5
$t $d --pagesize=131072 --overwrite=1 --inmemorydb=1 $1 $2 $3 $4 $5

$t $d --cacheunlimited=1 --reopen=1 $1 $2 $3 $4 $5
$t $d --cachesize=0 --reopen=1 $1 $2 $3 $4 $5
$t $d --cachesize=0 --overwrite=1 --reopen=1 $1 $2 $3 $4 $5
$t $d --cachesize=0 --disable-mmap=1 --reopen=1 $1 $2 $3 $4 $5
$t $d --cachesize=0 --overwrite=1 --disable-mmap=1 --reopen=1 $1 $2 $3 $4 $5

$t $d --cachesize=1 --reopen=1 $1 $2 $3 $4 $5
$t $d --cachesize=1 --overwrite=1 --reopen=1 $1 $2 $3 $4 $5
$t $d --cachesize=1 --disable-mmap=1 --reopen=1 $1 $2 $3 $4 $5
$t $d --cachesize=1 --overwrite=1 --disable-mmap=1 --reopen=1 $1 $2 $3 $4 $5
$t $d --cachesize=1 --use-cursors=1 --overwrite=1 --disable-mmap=1 --reopen=1 $1 $2 $3 $4 $5

$t $d --cachesize=50 --reopen=1 $1 $2 $3 $4 $5
$t $d --cachesize=50 --overwrite=1 --reopen=1 $1 $2 $3 $4 $5
$t $d --cachesize=50 --disable-mmap=1 --reopen=1 $1 $2 $3 $4 $5
$t $d --cachesize=50 --overwrite=1 --disable-mmap=1 --reopen=1 $1 $2 $3 $4 $5
$t $d --cachesize=50 --use-cursors=1 --overwrite=1 --disable-mmap=1 --reopen=1 $1 $2 $3 $4 $5

$t $d --cachesize=1024 --reopen=1 $1 $2 $3 $4 $5
$t $d --cachesize=1024 --overwrite=1 --reopen=1 $1 $2 $3 $4 $5
$t $d --cachesize=1024 --disable-mmap=1 --reopen=1 $1 $2 $3 $4 $5
$t $d --cachesize=1024 --overwrite=1 --disable-mmap=1 --reopen=1 $1 $2 $3 $4 $5

$t $d --use-recovery=1 --reopen=1 $1 $2 $3 $4 $5
$t $d --use-recovery=1 --overwrite=1 --reopen=1 $1 $2 $3 $4 $5
$t $d --use-recovery=1 --pagesize=1024 --reopen=1 $1 $2 $3 $4 $5
$t $d --use-recovery=1 --duplicate=1 --reopen=1 $1 $2 $3 $4 $5
$t $d --use-recovery=1 --overwrite=1 --disable-mmap=1 --reopen=1 $1 $2 $3 $4 $5
$t $d --use-recovery=1 --keysize=16 --reopen=1 $1 $2 $3 $4 $5

# HAM_DAM_SEQUENTIAL_INSERT
$t $d --data_access_mode=HAM_DAM_SEQUENTIAL_INSERT --keysize=8 --reopen=1 $1 $2 $3 $4 $5
$t $d --data_access_mode=HAM_DAM_SEQUENTIAL_INSERT --cachesize=50 --reopen=1 $1 $2 $3 $4 $5
$t $d --data_access_mode=HAM_DAM_SEQUENTIAL_INSERT --keysize=680 --duplicate=1 $1 $2 $3 $4 $5
$t $d --data_access_mode=HAM_DAM_SEQUENTIAL_INSERT --keysize=680 --overwrite=1 $1 $2 $3 $4 $5
$t $d --data_access_mode=HAM_DAM_SEQUENTIAL_INSERT --pagesize=1024 --overwrite=1 --keysize=16 $1 $2 $3 $4 $5

# HAM_DAM_RANDOM_WRITE
$t $d --data_access_mode=HAM_DAM_RANDOM_WRITE --keysize=8 --reopen=1 $1 $2 $3 $4 $5
$t $d --data_access_mode=HAM_DAM_RANDOM_WRITE --cachesize=50 --reopen=1 $1 $2 $3 $4 $5
$t $d --data_access_mode=HAM_DAM_RANDOM_WRITE --keysize=680 --duplicate=1 $1 $2 $3 $4 $5
$t $d --data_access_mode=HAM_DAM_RANDOM_WRITE --keysize=680 --overwrite=1 $1 $2 $3 $4 $5
$t $d --data_access_mode=HAM_DAM_RANDOM_WRITE --pagesize=1024 --overwrite=1 --keysize=16 $1 $2 $3 $4 $5

# HAM_HINT_APPEND
$t $d --hints=HAM_HINT_APPEND --use-cursors=1 --reopen=1 $1 $2 $3 $4 $5
$t $d --hints=HAM_HINT_APPEND --use-cursors=1 --inmemorydb=1 $1 $2 $3 $4 $5

# HAM_HINT_PREPEND
$t $d --hints=HAM_HINT_PREPEND --use-cursors=1 --reopen=1 $1 $2 $3 $4 $5
$t $d --hints=HAM_HINT_PREPEND --use-cursors=1 --inmemorydb=1 $1 $2 $3 $4 $5

# HAM_DIRECT_ACCESS
$t $d --direct_access=1 --inmemorydb=1 $1 $2 $3 $4 $5
$t $d --direct_access=1 --inmemorydb=1 --fullcheck-find=1 $1 $2 $3 $4 $5
$t $d --direct_access=1 --inmemorydb=1 --useralloc=1 $1 $2 $3 $4 $5
$t $d --direct_access=1 --inmemorydb=1 --overwrite=1 $1 $2 $3 $4 $5
$t $d --direct_access=1 --inmemorydb=1 --overwrite=1 --useralloc=1 $1 $2 $3 $4
$t $d --direct_access=1 --inmemorydb=1 --use-cursors=1 $1 $2 $3 $4 $5
$t $d --direct_access=1 --inmemorydb=1 --use-cursors=1 --overwrite=1 $1 $2 $3
$t $d --direct_access=1 --inmemorydb=1 --use-cursors=1 --useralloc=1 $1 $2 $3
$t $d --direct_access=1 --inmemorydb=1 --use-cursors=1 --duplicate-first=1 $1 $2 $3 $4 $5
$t $d --direct_access=1 --inmemorydb=1 --fullcheck-backwards=1 $1 $2 $3 $4 $5
