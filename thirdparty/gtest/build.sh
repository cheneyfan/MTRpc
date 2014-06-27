echo "extracting"
unzip gtest-1.6.0.zip
cd gtest-1.6.0
cmake .
make
cd ..
mkdir lib
cp -r gtest-1.6.0/libgtest.a lib/
cp -r gtest-1.6.0/include .
rm -fr gtest-1.6.0 
