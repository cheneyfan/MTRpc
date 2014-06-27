rm -fr protobuf-2.5.0
tar -zvxf protobuf-2.5.0.tar.gz
PD=`pwd`
echo $PD
cd protobuf-2.5.0
./configure --enable-shared=no --prefix="$PD"
make
make install

