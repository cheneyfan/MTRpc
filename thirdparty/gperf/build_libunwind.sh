tar zvxf libunwind-1.1.tar.gz

echo "clean"
BASEDIR=`pwd`

cd libunwind-1.1/
echo "install to $BASEDIR"
./configure  CFLAGS=-U_FORTIFY_SOURCE
make -j5
sudo make install

echo "make ok clean"
cd ..
rm -fr libunwind-1.1/
