package main

import (
	"./util"
	"bufio"
	"flag"
	"io"
	"log"
	"net"
	"os"
	"path/filepath"
)

var (
	host     = flag.String("h", "localhost", "host ip")
	port     = flag.String("p", "50005", "port")
	filename = flag.String("f", "recv_data", "file")
)

func main() {
	flag.Parse()
	conn, err := net.Dial("tcp", *host+":"+*port)
	util.CheckErr(err)
	//defer conn.Close()

	c := make(chan int64)
	go func(ch chan<- int64) {
		RequestFile(filename, conn, ch)
	}(c)

	log.Printf("recved:%dbytes.\n", <-c)
	conn.Close()
}

func RequestFile(fd *string, conn net.Conn, c chan<- int64) {
	filename := filepath.Base(*fd)
	if filename == "" {
		filename = *fd
	}
	conn.Write([]byte(filename))

	fp, err := os.Create(*fd)
	defer fp.Close()
	util.CheckErr(err)

	w := bufio.NewWriter(fp)
	n, err := io.Copy(w, conn)
	util.CheckErr(err)
	c <- n
}
