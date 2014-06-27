echo "extracting"
unzip gmock-1.6.0.zip
PWD=`pwd`
cd gmock-1.6.0
cmake -DCMAKE_INSTALL_PREFIX=$PWD .
#./configure --prefix=$PWD
make;
cp -fr include/ ../
mkdir -p ../lib
cp -fr libgmock.a ../lib/
cd ..
rm -fr gmock-1.6.0

