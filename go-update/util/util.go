package util

import (
	"log"
)

func init() {
	log.SetFlags(log.Llongfile)
}

func CheckErr(e error) {
	if e != nil {
		log.Fatal(e)

	}
}

const (
	HOST = "127.0.0.1"
	PORT = "50005"
)
