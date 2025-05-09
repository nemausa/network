cd `dirname $0`
##################
#key-val
#字典
#std#map<k,v>
##################
ulimit -n
ulimit -n 81920
ulimit -n
#GateServer地址
cmd="csGateUrl=ws://192.168.88.133:4567"
#客户端发送缓冲区大小（字节）
cmd=$cmd" nSendBuffSize=102400"
#客户端接收缓冲区大小（字节）
cmd=$cmd" nRecvBuffSize=102400"

#启动程序 传入参数
./start $cmd
#
read -p "..press any key to exit.." var

