ulimit -n 10240
export LD_LIBRARY_PATH=./lib/osx:$LD_LIBRARY_PATH
./bin/tcp_server
