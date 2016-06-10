package util

import (
	"bufio"
	"container/list"
	"io"
	"log"
	"net"
	"os"
	"path/filepath"

	"../filedata"
)

const (
	PORT    = "50005"
	VARSIZE = 1024
)

func init() {
	log.SetFlags(log.Llongfile)
}

func CheckErr(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

func ReadFile(filename string) (finfo *filedata.FileInfo, data []byte) {
	fp, err := os.Open(filename)
	CheckErr(err)
	defer fp.Close()

	r := bufio.NewReader(fp)
	s, err := fp.Stat()
	CheckErr(err)
	size := s.Size()
	name := filepath.Base(filename)
	finfo = &filedata.FileInfo{Name: name, Size: int(size)}

	data = make([]byte, finfo.Size)
	n, err := r.Read(data)
	CheckErr(err)
	if n != finfo.Size {
		log.Printf("ERROR read:%d,total:%d\n", n, size)
	}
	return
}

func WriteFile(finfo *filedata.FileInfo, data []byte) {
	fp, err := os.Create(finfo.Name)
	CheckErr(err)
	defer fp.Close()

	w := bufio.NewWriter(fp)
	n, err := w.Write(data)
	CheckErr(err)
	if n != finfo.Size {
		log.Printf("ERROR 接收:%d,写入:%d,原始大小:%d\n", len(data), n, finfo.Size)
	}
	w.Flush()
}

//list -> e.Value :[]byte
func NewBufferList(src []byte) *list.List {
	lst := list.New()
	lens := len(src)
	n := lens / VARSIZE
	for i := 0; i < n; i++ {
		lst.PushBack(src[i*VARSIZE : (i+1)*VARSIZE])
	}
	if lens-n*VARSIZE > 0 {
		lst.PushBack(src[n*VARSIZE:])
	}
	return lst
}

//链表发送和接收数据
//没有处理粘包和丢包，仅是分片
func SendData(conn net.Conn, src []byte) {
	lst := NewBufferList(src)

	for e := lst.Front(); e != nil; e = e.Next() {
		p := e.Value.([]byte)
		n, err := conn.Write(p)
		//log.Printf("发送长度%d\n", n)
		CheckErr(err)
		if n != len(p) {
			log.Fatalf("发送%v错误!\n", e.Value)
		}
		//log.Printf("[%d]发送%x\n", n, string(p))
	}
}

func RecvData(conn net.Conn, size int) []byte {
	//一次发送的数据可能大于VARSIZe
	buf := make([]byte, 2*VARSIZE)
	data := make([]byte, size, size)
	cnt := 0
	for {
		n, err := conn.Read(buf)
		//log.Printf("接收长度%d\n", n)
		/*if n == 0 {
			log.Println("对方关闭了写端!")
			break
		}*/
		if err != nil {
			if err != io.EOF {
				log.Fatalf("ERROR!接收到%d\n", n)
			}
			break
		}
		//添加在后面
		//log.Printf("[%d]收到%x\n", n, string(buf[:n]))
		copy(data[cnt:], buf[:n])
		cnt += n
	}
	return data
}
