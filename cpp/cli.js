var net = require('net');
var fs  = require('fs');

var SEP = '\r\r\r';

if(process.argv.length!=4){
  console.log('usage:',process.argv[1],'host file.');
  process.exit(1);
}

var s = process.argv[2].split(':');
var filename = process.argv[3];

var client = net.connect({host:s[0],port:parseInt(s[1])},()=>{
  console.log("connected to server.");
  console.log('req:',filename);
  client.write(filename);
});

function recv_file(client){

}

//每次重入，只会写入一部分
  /*var chunk = data.toString().split(SEP);
  var filename = chunk[0];
  console.log('filename:',filename);
  */
/*
fs.open(filename,'a',(err,fd)=>{
  if(err)
    throw err;
  var offset = 0;
  client.on('data',(data)=>{
    //需要同步写
    fs.write(fd,data,(err)=>{
      if(err) throw err;
    });
    offset+=data.length;
    console.log('len:',data.length,'\toffset:',offset);
    client.end();
  }).on('end',()=>{
    console.log('disconnected.');
    fs.close(fd,(err)=>{
      if(err) throw err;
    });
  });
});
*/
var size = 0;
client.on('data',(data)=>{
  //每次都重新打开了文件，所以文件偏移量需要一个状态保存才能异步写
  fs.appendFile(filename,data,(err)=>{
    if(err) throw err;
  });
  size+=data.length;
  client.end();
}).on('end',()=>{
  console.log('filename:',filename,'\tsize:',size);
  console.log('disconnected.');
});
