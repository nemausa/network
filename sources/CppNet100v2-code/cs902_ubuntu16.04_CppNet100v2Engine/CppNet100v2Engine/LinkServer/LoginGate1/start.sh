cd `dirname $0`
##################
#key-val
#字典
#std#map<k,v>
##################
ulimit -n
ulimit -n 81920
ulimit -n
#服务端IP地址
cmd='strIP=any'
#服务端端口
cmd=$cmd' nPort=5000'
#GateServer地址
cmd=$cmd" ssGateUrl=ws://192.168.88.133:4567"
#消息处理线程数量
cmd=$cmd" nThread=1"
#客户端连接上限
cmd="$cmd nMaxClient=81920"
#客户端发送缓冲区大小（字节）
cmd=$cmd" nSendBuffSize=102400"
#客户端接收缓冲区大小（字节）
cmd=$cmd" nRecvBuffSize=102400"
#收到消息后将返回应答消息
cmd=$cmd" -logingate"
#我接受客户端连接的url
cmd=$cmd" myurl=ws://192.168.88.133:5000"

#启动程序 传入参数
./start $cmd
#
read -p "..press any key to exit.." var

