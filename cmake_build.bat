mkdir build
cd build
rm -rf *
cmake -G "Unix Makefiles" ..
make install
cd ..
