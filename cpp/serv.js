var net = require('net');
var fs = require('fs');
var path = require('path');


var SEP = '\r\r\r';

var server = net.createServer((socket)=>{
  //socket.write('hello\n');
  //socket.end('goodbye\n');
}).on('error',(err)=>{
  throw err;
  server.close();
});

server.listen({port:50013},()=>{
  addr = server.address();
  console.log("opened server on %j",addr);
  server.on('connection',(conn)=>{
    console.log('connected to client.');

    conn.on('data',(data)=>{
      var filename = path.basename(data.toString());

      fs.readFile(filename,(err,data)=>{
        if(err) throw err;
        console.log(filename,':',data.length);
        conn.write(data);
      });
    });
  });
});
