// serv-1.go
package main

import (
	"fmt"
	"io"
	"net"

	"./filedata"
	"./util"
)

func main() {
	tcpAddr, err := net.ResolveTCPAddr("tcp4", ":"+util.PORT)
	util.CheckErr(err)
	listener, err := net.ListenTCP("tcp", tcpAddr)
	util.CheckErr(err)
	for {
		conn, err := listener.Accept()
		if err != nil {
			continue
		}
		//fmt.Println("开始处理连接:")
		go HandleClient(conn)
	}
}

/*
func HandleClient(conn net.Conn) {
	//接收请求文件
	conn.Read()
}
*/
func HandleClient(conn net.Conn) {

	buf := make([]byte, filedata.ENCODE_SIZE)
	defer conn.Close()
	//循环接收文件
	for {
		//这里改为事件通知会比较好，轮询很耗费系统资源
		//等待接收下一个文件,阻塞
		read_len, err := conn.Read(buf)
		if err != nil {
			if err != io.EOF {
				util.CheckErr(err)
			}
			continue //循环查询是否有数据
		}
		/*
			if err != nil {
				fmt.Println(err)
				break
			}
		*/
		if read_len == 0 { //对方关闭了写端
			break
		} else {
			//first recv file head
			//fmt.Println("接收info:")
			finfo := filedata.ReadFileInfo(buf[:read_len])
			if finfo.Size == 0 {
				fmt.Printf("file info ERROR!%+v\n", finfo)
				//break
			}
			//fmt.Printf("info:%+v\n", finfo)
			//recv data
			//fmt.Println("接收数据!")
			data := util.RecvData(conn, finfo.Size)
			//fmt.Printf("data:\n%x\n", string(data))
			//data := make([]byte, finfo.Size)
			//方案1
			/*tmp := make([]byte, 256)
			for {
				n, err := conn.Read(tmp)

				if err != nil {
					if err != io.EOF {
						fmt.Println("read error:", err)
					}
					break //read finished
				}
				buf = append(buf, tmp[:n]...)
			}*/
			/*方案2
			var buf bytes.Buffer
			io.Copy(&buf, conn)
			buf.Write(data)
			if len(data) != finfo.Size {
				fmt.Printf("recv ERROR size:%d!\n", finfo.Size)
				break
			}
			*/
			//debug

			/*
				if finfo.IsDataError(data) {
					fmt.Println("check ERROR!")
					break
				}
			*/
			//检查文件内容是否一致
			//得到读取完毕才写入
			if !finfo.IsDataError(data) {
				//fmt.Println("开始写数据!")
				util.WriteFile(finfo, data)
				//fmt.Println("写文件完成!")
			}
		}
	}
}
