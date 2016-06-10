# TCP文件传输

## 思想
1. 服务器发起文件信息请求,客户端处理请求,读取文件，将文件名、大小、文件内容MD5组成的对象序列化，传输到服务器
2. 服务器根据收到的数据反序列化得到文件信息，发起文件内容请求，准备收取文件数据
3. 将文件内容分片，在两端发送和接收数据，接收错误发起重发请求(TCP保证可靠性，所以可以忽略)
4. 校验文件数据，服务器写入文件，发送完成标志,完成文件传输

## 依赖
1. msgpack.v2 序列化库，性能较好(Go)
2. 类似libevent基于事件的库，socket性能较之前循环查询好(基本实现)(C)
3. simplog日志库(c)
4. Boost.asio,thread,system,file_system,log(C++)

## 使用

```bash
cpp:

cmake

make

./server PORT

./client HOST:PORT file


c:

make

./fileserv [-hlocalhost -p9000]

./filecli [-hlocalhost -p9000 -oOUTFILENAME] -f filename


go:

make

./serv

./cli --help
```

## 改进历史

### 2016-05-17 创建基于Boost.asio的异步传输分支，Proactor模式                              
1. 异步回调逻辑更加清晰                               
2. Proactor比reactor模式区别在于是收完数据后再通知调用者，而reactor在有数据可读就会移交控制权，
3. fread,fwrite每次读取BUFSIZ就操作，由TCP保证有序,减少阻塞时间

### 2016-05-15 c++中包含一个NodeJS版本简单实现
1. 简洁，回调简洁

### 2016-05-10 创建go-update分支,基于可靠TCP的Go简洁版本
1. 改变数据发送方为服务器，为多个clinet提供文件服务
2. client向server提供带文件名的请求，server收到请求，发送数据
3. client收到数据并写入文件，关闭连接。server继续为其他连接服务
4. 所有连接基于TCP可靠连接，和golang内部实现的异步IO和多路选择

### 2016-05-09 使用C基于libevent实现

#### 已解决 
1. 客户端发送文件后即退出，将所有信号禁用实现
2. 服务器在接收一次传输后，再次传输文件成功，比较大小之前total初始化0解决
3. simplog不是线程安全的，在客户端自定义log configuration之后,程序会崩溃，因此没有开启自定义配置
4. libevent关注的是read事件，在socket可读，即有数据到达的时候需要处理，这是主动逻辑，也是唯一需要
自己实现的，可写是自动管理的，借助evbuffer缓存读和写的数据。

#### 待跟进
1. libevent get_output是读取socket数据缓存,缓存需要自己清理
2. 考虑实现实现文件边读socket边写，导致文件大的情况下内存压力
3. 数据经历了多次拷贝，可以考虑使用zero-copy
4. socket只要有数据到来必会触发read事件回调，buffer发送之后再次查询length会为0

### 2016-05-04 使用Go实现，同步阻塞模型，效率低，暂未优化
1. 程序在windows下出现64k BUG,在linux下md5校验完全失败，但每次接收的错误数据都一样，应该是逻辑错误
2. 分片大小和文件大小都影响数据收取方第一次是否会丢失，所以依赖于网络环境，需要改进接收模型
3. socket阻塞式读写，应该改成IO事件通知方式，有优化空间(conn实现了通用io.Reader,io.Writer，已改进)
4. 未充分利用go的routine进制并发处理读写
