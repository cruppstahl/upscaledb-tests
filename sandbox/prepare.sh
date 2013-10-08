

CUR=`pwd`
DIR=$HOME/Downloads/hamsterdb

# make the test
#cd $DIR
#make -j 8
#cd $CUR

# copy the test binary
cp $DIR/tools/ham_bench/ham_bench ham_bench

# done!
echo "ok"
