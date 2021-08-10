# network

Part of my daily plan for studying network.

#### compile cmd
 `cmake -G "Unix Makefiles" ..`

 #### 提高并行能力
- 业务分离，发送和接受分开处理，不能因为耗时操作阻塞了网络数据传输，生产者-缓冲区-消费者模式
- recv次数大于send次数

#### linux 向外TCP最大連接只能打開28232個端口限制
- echo "net.ipv4.ip_local_port_range = 1024 65000" >> /etc/sysctl.conf; sysctl -p

#### compile spldlog in windows
- git checkout v2.x
- cmake -S . -B build -G "Unix Makefiles"
- cmake --build build

#### 编译release版本，有返回值的必须返回，不然可能访问没有权限访问的默认寄存器