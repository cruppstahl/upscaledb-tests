
dir=../../testfiles

files="1/45.tst 1/220.tst 1/ext_020.tst 2/ext_060.tst 2/blb-001.tst"

for file in $files
do
    echo "running $dir/$file $1 $2 $3 $4 $5"
    valgrind --tool=memcheck --suppressions=valgrind.suppress \
            ./test $dir/$file $1 $2 $3 $4 $5 
    echo ""
    echo ""
done

