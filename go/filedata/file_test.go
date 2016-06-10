package filedata

import (
	"testing"
)

func Test_Serialization(t *testing.T) {
	finfo := new(FileInfo)
	finfo.Name = "1234.c"
	finfo.Size = 4096
	data := []byte{1, 2, 3, 4, 5, 6}
	finfo.CalcCheckInfo(data)
	//copy(finfo.Info, [16]byte{0x1, 0x2, 0x3, 0x4})

	b := WriteFileInfo(finfo)

	tmp := ReadFileInfo(b)

	if !(finfo.Name == tmp.Name && finfo.Size == tmp.Size) {
		t.Errorf("%+v\n", tmp)
	}

	t.Logf("%+v\n", tmp)

	if finfo.IsDataError(data) {
		t.Error("data check ERROR!\n")
	}
	t.Logf("size:%d", len(b))
}
