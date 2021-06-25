#!/bin/bash

start_ms=$(ruby -e 'puts (Time.now.to_f * 1000).to_i')

mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make -j install

end_ms=$(ruby -e 'puts (Time.now.to_f * 1000).to_i')
elapsed_ms=$((end_ms - start_ms))
echo "-- ${elapsed_ms} ms passed"
