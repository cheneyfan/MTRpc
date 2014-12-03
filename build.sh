rm -fr build
rm -fr out32
rm -fr out
#rm -fr mtrpc_32/* mtrpc_64/*
rm proto/*.pb.*
mkdir build
cd build;
cmake ..
<<<<<<< HEAD
make -j2
=======
make -j4
>>>>>>> a8c4e21dde5bd0d301208e60bbda87f03a54e906
make install

