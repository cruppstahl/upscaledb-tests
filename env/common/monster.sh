`rm -f *.db*`;

# AES encryption
run(3, "--enable-encryption --reopen");
run(3, "--enable-encryption --use-transactions=10 --reopen");

# client/server
run(3, "--enable-remote --reopen");
run(3, "--enable-remote --use-transactions=10 --reopen");

run(3, "--reopen");
run(3, "--fullcheck-find");
run(3, "--overwrite --reopen");
run(3, "--inmemorydb");
run(3, "--overwrite --inmemorydb");
run(3, "--use-cursors --reopen");
run(3, "--use-cursors --overwrite --reopen");
run(3, "--use-cursors --inmemorydb");
run(3, "--use-cursors --overwrite --inmemorydb");
run(3, "--use-cursors --duplicate-first --reopen");

run(3, "--fullcheck-backwards --inmemorydb");
run(3, "--fullcheck-backwards --reopen");
run(3, "--fullcheck-backwards --duplicate --reopen");
run(3, "--fullcheck-backwards --overwrite --reopen");

run(3, "--duplicate --reopen");
run(3, "--fullcheck-find");
run(3, "--duplicate --inmemorydb");
run(3, "--duplicate --use-cursors --reopen");
run(3, "--duplicate --use-cursors --inmemorydb");

run(3, "--disable-mmap --reopen");
run(3, "--disable-mmap --overwrite --reopen");

run(3, "--keysize=8 --reopen");
run(3, "--keysize=8 --overwrite --reopen");
run(3, "--keysize=8 --inmemorydb");
run(3, "--keysize=8 --overwrite --inmemorydb");
run(3, "--keysize=8 --use-cursors --overwrite --inmemorydb");

run(3, "--keysize=12 --reopen");
run(3, "--keysize=12 --overwrite --reopen");
run(3, "--keysize=12 --inmemorydb");
run(3, "--keysize=12 --overwrite --inmemorydb");

run(3, "--keysize=33 --reopen");
run(3, "--keysize=33 --overwrite --reopen");
run(3, "--keysize=33 --inmemorydb");
run(3, "--keysize=33 --overwrite --inmemorydb");

run(3, "--keysize=680 --reopen");
run(3, "--keysize=680 --overwrite --reopen");
run(3, "--keysize=680 --inmemorydb");
run(3, "--keysize=680 --overwrite --inmemorydb");

run(3, "--pagesize=1024 --reopen");
run(3, "--pagesize=1024 --overwrite --reopen");
run(3, "--pagesize=1024 --inmemorydb");
run(3, "--pagesize=1024 --overwrite --inmemorydb");

run(3, "--pagesize=8192 --reopen");
run(3, "--pagesize=8192 --overwrite --reopen");
run(3, "--pagesize=8192 --inmemorydb");
run(3, "--pagesize=8192 --overwrite --inmemorydb");

run(3, "--pagesize=131072 --reopen");
run(3, "--pagesize=131072 --overwrite --reopen");
run(3, "--pagesize=131072 --inmemorydb");
run(3, "--pagesize=131072 --overwrite --inmemorydb");

run(3, "--cacheunlimited --reopen");
run(3, "--cachesize=0 --reopen");
run(3, "--cachesize=0 --overwrite --reopen");
run(3, "--cachesize=0 --disable-mmap --reopen");
run(3, "--cachesize=0 --overwrite --disable-mmap --reopen");

run(3, "--cachesize --reopen");
run(3, "--cachesize --overwrite --reopen");
run(3, "--cachesize --disable-mmap --reopen");
run(3, "--cachesize --overwrite --disable-mmap --reopen");
run(3, "--cachesize --use-cursors --overwrite --disable-mmap --reopen");

run(3, "--cachesize=50 --reopen");
run(3, "--cachesize=50 --overwrite --reopen");
run(3, "--cachesize=50 --disable-mmap --reopen");
run(3, "--cachesize=50 --overwrite --disable-mmap --reopen");
run(3, "--cachesize=50 --use-cursors --overwrite --disable-mmap --reopen");

run(3, "--cachesize=1024 --reopen");
run(3, "--cachesize=1024 --overwrite --reopen");
run(3, "--cachesize=1024 --disable-mmap --reopen");
run(3, "--cachesize=1024 --overwrite --disable-mmap --reopen");

run(3, "--use-recovery --reopen");
run(3, "--use-recovery --overwrite --reopen");
run(3, "--use-recovery --pagesize=1024 --reopen");
run(3, "--use-recovery --duplicate --reopen");
run(3, "--use-recovery --overwrite --disable-mmap --reopen");
run(3, "--use-recovery --keysize=16 --reopen");

# HAM_HINT_APPEND
run(3, "--hints=HAM_HINT_APPEND --use-cursors --reopen");
run(3, "--hints=HAM_HINT_APPEND --use-cursors --inmemorydb");

# HAM_HINT_PREPEND
run(3, "--hints=HAM_HINT_PREPEND --use-cursors --reopen");
run(3, "--hints=HAM_HINT_PREPEND --use-cursors --inmemorydb");

# HAM_DIRECT_ACCESS
run(3, "--direct-access --inmemorydb");
run(3, "--direct-access --inmemorydb --fullcheck-find");
run(3, "--direct-access --inmemorydb --overwrite");
run(3, "--direct-access --inmemorydb --use-cursors");
run(3, "--direct-access --inmemorydb --use-cursors --overwrite $1 $2 $3");
run(3, "--direct-access --inmemorydb --use-cursors --duplicate-first");
run(3, "--direct-access --inmemorydb --fullcheck-backwards");

# use temporary transactions
run(3, "--use-transactions=tmp");
run(3, "--use-transactions=tmp --overwrite");
run(3, "--use-transactions=tmp --duplicate");
run(3, "--use-transactions=tmp --use-cursors");
run(3, "--use-transactions=tmp --use-cursors --overwrite");
run(3, "--use-transactions=tmp --use-cursors --duplicate");
run(3, "--use-transactions=tmp --use-cursors --duplicate-first");
run(3, "--use-transactions=tmp --disable-mmap");
run(3, "--use-transactions=tmp --fullcheck-find");
run(3, "--use-transactions=tmp --fullcheck-backwards");
run(3, "--use-transactions=tmp --pagesize=1024");
run(3, "--use-transactions=tmp --pagesize=10240");
run(3, "--use-transactions=tmp --pagesize=102400");
run(3, "--use-transactions=tmp --cachesize=1024");
run(3, "--use-transactions=tmp --cachesize=102400");
run(3, "--use-transactions=tmp --cachesize=10240000");

# use transactions: group 5 ops in one txn
run(3, "--use-transactions=5");
run(3, "--use-transactions=5 --overwrite");
run(3, "--use-transactions=5 --duplicate");
run(3, "--use-transactions=5 --use-cursors");
run(3, "--use-transactions=5 --use-cursors --overwrite");
run(3, "--use-transactions=5 --use-cursors --duplicate");
run(3, "--use-transactions=5 --use-cursors --duplicate-first");
run(3, "--use-transactions=5 --fullcheck-find");
run(3, "--use-transactions=5 --fullcheck-backwards");

# use transactions: group 20 ops in one txn
run(3, "--use-transactions=20");
run(3, "--use-transactions=20 --overwrite");
run(3, "--use-transactions=20 --duplicate");
run(3, "--use-transactions=20 --use-cursors");
run(3, "--use-transactions=20 --use-cursors --overwrite");
run(3, "--use-transactions=20 --use-cursors --duplicate");
run(3, "--use-transactions=20 --use-cursors --duplicate-first");
run(3, "--use-transactions=20 --fullcheck-find");
run(3, "--use-transactions=20 --fullcheck-backwards");

# use transactions: group 200 ops in one txn
run(3, "--use-transactions=200");
run(3, "--use-transactions=200 --overwrite");
run(3, "--use-transactions=200 --duplicate");
run(3, "--use-transactions=200 --use-cursors");
run(3, "--use-transactions=200 --use-cursors --overwrite");
run(3, "--use-transactions=200 --use-cursors --duplicate");
run(3, "--use-transactions=200 --use-cursors --duplicate-first");
run(3, "--use-transactions=200 --fullcheck-find");
run(3, "--use-transactions=200 --fullcheck-backwards");

# use transactions: group ALL ops in one txn
run(3, "--use-transactions=all");
run(3, "--use-transactions=all --overwrite");
run(3, "--use-transactions=all --duplicate");
run(3, "--use-transactions=all --use-cursors");
run(3, "--use-transactions=all --use-cursors --overwrite");
run(3, "--use-transactions=all --use-cursors --duplicate");

run(3, "--use-transactions=all --use-cursors --duplicate-first");
run(3, "--use-transactions=all --fullcheck-find");
run(3, "--use-transactions=all --fullcheck-backwards");

# use 2 threads
run(3, "--num-threads=2");
run(3, "--num-threads=2 --overwrite");
run(3, "--num-threads=2 --use-cursors");
run(3, "--num-threads=2 --use-cursors --duplicate-first");
run(3, "--num-threads=2 --fullcheck-backwards");
run(3, "--num-threads=2 --use-transactions=20");
run(2, "--num-threads=2 --use-transactions=20 --overwrite");
run(2, "--num-threads=2 --use-transactions=20 --duplicate");
run(2, "--num-threads=2 --use-transactions=20 --use-cursors");
run(2, "--num-threads=2 --use-transactions=20 --use-cursors --overwrite");
run(2, "--num-threads=2 --use-transactions=20 --use-cursors --duplicate");
run(2, "--num-threads=2 --use-cursors --duplicate-first");

# use 10 threads
run(2, "--num-threads=10");
run(2, "--num-threads=10 --overwrite");
run(2, "--num-threads=10 --use-cursors --duplicate");
run(2, "--num-threads=10 --use-transactions=20");

