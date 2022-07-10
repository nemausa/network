ulimit -n 102400
export LD_LIBRARY_PATH=./lib/unix:$LD_LIBRARY_PATH
./bin/tcp_server
