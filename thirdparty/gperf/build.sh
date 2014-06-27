echo "be sure installed autoconf"
#sh build_libunwind.sh 

echo "be sure installed linunwind"
echo "extract "
unzip gperftools-2.1.zip 

echo "clean"
BASEDIR=`pwd`

cd gperftools-2.1

echo "install to $BASEDIR"
./configure --prefix="$BASEDIR"
make -j5
make install
echo "make ok clean"
cd ..
rm -fr gperftools-2.1
