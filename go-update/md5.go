/**
* 文件MD5
*/

package main

import(
    "fmt"
    "os"
    "log"
    "bufio"
    "crypto/md5"
    "reflect"
)

func CheckErr(err error){
    if err != nil{
        log.Fatal(err)
    }
}

func main(){
    fp,err:=os.Open(os.Args[1])
    CheckErr(err)
    defer fp.Close()

    finfo,err:=fp.Stat()
    CheckErr(err)

    size:=finfo.Size()

    data:=make([]byte,size)

    r:=bufio.NewReader(fp)
    nRead,err:=r.Read(data)
    CheckErr(err)
    if int64(nRead)!=size{
        log.Println("读取大小错误!")
    }
    bs:=md5.Sum(data)
    s:=fmt.Sprintf("%x\n",bs)

    //fmt.Sscanf(s,"%x",tbs)
    tbs:=[]byte(s) //转换成string后，stirng还原成byte
    if reflect.DeepEqual(tbs,bs){//不一致
        fmt.Println("OK!")
        fmt.Println(tbs[0])
    }
    //fmt.Printf("%x\n%s\n",bs,string(tbs))

    var rbs [16]byte
    fmt.Sscanf(s,"%x",rbs)
    /*for i,d:=range []byte(s){
        fmt.Sscanf(d,"%x",rbs[i])
    }*/
    for i:=range rbs{
        fmt.Printf("%x",rbs[i])
    }
    fmt.Printf("\n%x\n",bs)

    if compare(bs,rbs){//reflect.DeepEqual(bs,rbs){
        fmt.Println("rbs OK")
    }
}

func compare(p,q [16]byte) bool{
    /*if p==nil &&q==nil{
        return true
    }
    if p==nil || q==nil{
        return false
    }
    if len(p)!=len(q){
        return false
    }*/
    for i:=range p{
        if p[i]!=q[i]{
            return false
        }
    }
    return true
}
