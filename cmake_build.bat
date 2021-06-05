mkdir build
cd build
rm -rf *
cmake -G "Unix Makefiles" ..
make VERBOSE=1
make install
