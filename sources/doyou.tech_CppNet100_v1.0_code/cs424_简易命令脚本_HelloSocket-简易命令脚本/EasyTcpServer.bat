::服务端IP地址
@set strIP=any
::服务端端口
@set nPort=4567
::消息线程数量 CellServer
@set nThread=1
::限制客户端数量 未启用
@set nClient=3

EasyTcpServer %strIP% %nPort% %nThread% %nClient%

@pause