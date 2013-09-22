

CUR=`pwd`

# make the test
cd ../new/
make -j 8
cd $CUR

# copy the test binary
cp ../new/.libs/lt-test test

# done!
