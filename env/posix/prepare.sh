

CUR=`pwd`
#HAM=/home/chris/prj/hamsterdb/
#HAM=/home/chris/tmp/hamsterdb-1.1.10/
HAM=/home/chris/prj/hamsterdb_master/

# update the header files
cp $HAM/include/ham/*.h ../common/ham/

# make the hamsterdb library
cd $HAM/src
make
cd $CUR

# copy the library
cp $HAM/src/.libs/libhamsterdb.a .

# make the test
cd ../../src/
make
cd $CUR

# copy the test binary
cp ../../src/test .

# done!
