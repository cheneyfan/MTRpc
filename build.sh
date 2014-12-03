rm -fr build
rm -fr out32
rm -fr out
#rm -fr mtrpc_32/* mtrpc_64/*
rm proto/*.pb.*
mkdir build
cd build;
cmake ..
make -j4
make install

