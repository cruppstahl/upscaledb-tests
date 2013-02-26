#!/bin/sh
#
# hamsterdb test script
#
# Copyright (C) 2005-2007, Christoph Rupp (chris@crupp.de). 
# All rights reserved. See file LICENSE for license and copyright 
# information.
#
#
# run with 'sh valgrind.sh 2&> valgrind.log'
#

\rm -rf *.db

./call.sh "--reopen=1" 
./call.sh "--overwrite=1 --reopen=1" 
./call.sh "--duplicate=1 --reopen=1" 
./call.sh "--use-cursors=1 --duplicate-first=1 --reopen=1" 
./call.sh "--inmemorydb=1" 
./call.sh "--inmemorydb=1 --duplicate=1"
./call.sh "--overwrite=1 --inmemorydb=1" 
./call.sh "--reopen=1 --use-cursors=1" 
./call.sh "--reopen=1 --use-cursors=1 --duplicate=1" 
./call.sh "--overwrite=1 --reopen=1 --use-cursors=1" 
./call.sh "--inmemorydb=1 --use-cursors=1" 
./call.sh "--overwrite=1 --inmemorydb=1 --use-cursors=1" 
./call.sh "--fullcheck-find=1" 
./call.sh "--fullcheck-find=1 --duplicate=1" 
./call.sh "--fullcheck-backwards=1" 
./call.sh "--fullcheck-backwards=1 --duplicate=1" 
./call.sh "--disable-mmap=1 --overwrite=1 --reopen=1" 
./call.sh "--keysize=8 --overwrite=1 --reopen=1" 
./call.sh "--keysize=12 --overwrite=1 --reopen=1" 
./call.sh "--keysize=33 --overwrite=1 --reopen=1" 
./call.sh "--keysize=680 --overwrite=1 --reopen=1" 
./call.sh "--pagesize=1024 --overwrite=1 --reopen=1" 
./call.sh "--pagesize=8192 --overwrite=1 --reopen=1" 
./call.sh "--pagesize=131072 --overwrite=1 --reopen=1" 
./call.sh "--cachesize=0 --overwrite=1 --reopen=1" 
./call.sh "--use-recovery=1 --reopen=1" 
./call.sh "--cacheunlimited=1" 
./call.sh "--direct-access=1 --inmemorydb=1"
./call.sh "--use-transactions=tmp"
./call.sh "--use-transactions=10"
./call.sh "--use-transactions=10000"
./call.sh "--use-transactions=all"
