#!/bin/bash

start=$[$(date +%s%N)/1000000]
cd build
rm -rf *
cmake ..
make -j install

end=$[$(date +%s%N)/1000000]
take=$(( end - start ))
echo Time taken to execute commands is ${take} millseconds.