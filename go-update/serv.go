package main

import (
	"bufio"
	"io"
	"log"
	"net"
	"os"

	"./util"
)

func EndConn(e error,conn net.Conn){
    if e!=nil{
        if e!=io.EOF{
            log.Println(e)
        }
        conn.Close()
    }
}
func main() {
    tcpAddr, err := net.ResolveTCPAddr("tcp", util.HOST+":"+util.PORT)
	util.CheckErr(err)
	listener, err := net.ListenTCP("tcp", tcpAddr)
	util.CheckErr(err)
	for {
		conn, err := listener.Accept()
		if err != nil {
			continue
		}
		go HandleClient(conn)
	}
}

func HandleClient(conn net.Conn) {
    data := make([]byte,256)
    size,err:=conn.Read(data)
    //util.CheckErr(err)
    EndConn(err,conn)

    filename:=string(data[:size])

	fp, err := os.Open(filename)
    EndConn(err,conn)
	//util.CheckErr(err)
    defer fp.Close()

	w := bufio.NewReader(fp)

	n, err := io.Copy(conn, w)
    EndConn(err,conn)
	//util.CheckErr(err)
    log.Printf("file:%s,writen %d bytes.", filename,n)
    conn.Close()
}
