

CUR=`pwd`

HAM=../../../../hamsterdb/trunk/win32/out

# update the header files
cp $HAM/../../include/ham/*.h ../common/ham/

# copy the hamsterdb-library
cp $HAM/dll/hamsterdb.lib .
cp $HAM/dll/hamsterdb.dll .

# done!
