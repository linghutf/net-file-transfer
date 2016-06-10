1. js版本正常，验证多重回调处理数据，多次打开文件以定位到文件末尾
2. boost版本在小文件时不能正常传输，怀疑是buffer没有满导致没有触发发送
 - 使用tcpdump -i lo -nn(完整包头) -X 'port 50013' 查看发送了数据没有
 - 通过tcp发现数据已经发送,那就是接收方的问题
 - 通过日志在write_file中发现,只读一次就到达了eof边界,error被触发，所以剩下的数据没有处理，
 需要改进逻辑
 - 在eof发生时将收到的数据写到文件中，问题消失

3. 基本思想:传送文件名(长度+文件名),asio::buffer不能传string,bytes_transferred都是对方传输的array固定大小
4. 然后在回调函数中收一段数据就写一段数据
5. 改成local_systm的Unix socket只要改进本设置即可。
6. 还要注意底层使用了leveldb存储，需要对接这一部分。
7. TCP传输没有边界，需要人为加上边界\r\r\r或者是类似leveldb的传输都是前头一个bit表示是否有后续
find . -iwholename '*cmake*' -not -name CMakeLists.txt -delete
