package filedata

import (
	"crypto/md5"
	"log"

	"gopkg.in/vmihailenco/msgpack.v2"
)

const (
	SIZE        = 16 //info length
	ENCODE_SIZE = 100
)

type FileInfo struct {
	Name string
	Size int
	Info [SIZE]byte //check md5
}

var (
	_ msgpack.CustomEncoder = &FileInfo{}
	_ msgpack.CustomDecoder = &FileInfo{}
)

func init() {
	log.SetFlags(log.Llongfile)
}

func CheckErr(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

func NewFileInfo() *FileInfo {
	m := new(FileInfo)
	return m
}

func (self *FileInfo) EncodeMsgpack(enc *msgpack.Encoder) error {
	return enc.Encode(self.Name, self.Size, self.Info)
}

func (self *FileInfo) DecodeMsgpack(dec *msgpack.Decoder) error {
	return dec.Decode(&self.Name, &self.Size, &self.Info)
}

func ReadFileInfo(data []byte) *FileInfo {
	var fileinfo FileInfo
	err := msgpack.Unmarshal(data, &fileinfo)
	CheckErr(err)
	return &fileinfo
}

func WriteFileInfo(fileinfo *FileInfo) []byte {
	b, err := msgpack.Marshal(fileinfo)
	CheckErr(err)
	return b
}

//计算文件散列码
func (self *FileInfo) CalcCheckInfo(data []byte) {
	self.Info = md5.Sum(data)
}

//检查文件内容是否一致
func (self *FileInfo) IsDataError(data []byte) bool {
	recv_data_md5 := md5.Sum(data)

	for i := range recv_data_md5 {
		if recv_data_md5[i] != self.Info[i] {
			log.Printf("\n[src md5]:%x\n[dst md5]:%x\n", self.Info, recv_data_md5)
			return true
		}
	}
	return false

}
