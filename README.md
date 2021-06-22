# network

Part of my daily plan for studying network.

#### compile cmd
 `cmake -G "Unix Makefiles" ..`

 #### 提高并行能力
- 业务分离，发送和接受分开处理，不能因为耗时操作阻塞了网络数据传输，生产者-缓冲区-消费者模式
- recv次数大于send次数