1. socket在读取时可能一次无法读完，因此要循环判断
2. 文件头序列化判断，然后申请二进制快，接收文件内容
3. 通过md5检查发现client发送的数据md5码是正确的，所以排除读取文件错误，问题出现在发送端和接收端的数据处理

4. 经过socket内容测试发信数据传输方数据是正确的
5. 问题出来接收方，数据不对?routine没写入数据,是groutine没有等待接收完成就开始去写数据，所以出错
6. 改正后能正常收发大于64k的数据，而在小于64k时收到数据不正常
7. 接收方一次接收到的数据可能不是64k,所以使用64k缓冲会导致数据超出部分被丢弃了(不是)
8. 接收第一个分片数据总是小于发送的数据
9. 5,6,7,8的问题在于VARSIZE太大，导致收取方第一次丢失数据
