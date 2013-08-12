

CUR=`pwd`
HAM=/home/chris/prj/hamsterdb_master/
HAM=/home/chris/prj/hamsterdb/
#HAM=/home/chris/tmp/hamsterdb-2.1.2

# update the header files
cp $HAM/include/ham/*.h ../common/ham/

# make the hamsterdb library
cd $HAM/src
make -j 8
cd $CUR

# copy the library
cp $HAM/src/.libs/libhamsterdb.a .
cp $HAM/src/server/.libs/libhamserver.a .

# make the test
cd ../../src/
make -j 8
cd $CUR

# copy the test binary
cp ../../src/test .

# done!
