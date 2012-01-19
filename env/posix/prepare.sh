

CUR=`pwd`
HAM=/home/chris/prj/hamsterdb_master/
HAM=/home/chris/prj/hamsterdb/
#HAM=/home/chris/tmp/hamsterdb-2.0.0.rc2/

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
