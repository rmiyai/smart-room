//web繧ｵ繝ｼ繝�
var express = require('express');
var readline = require('readline');
var app = express();
var http = require('http').Server(app);
var io = require('socket.io')(http);
var SerialPort = require('serialport').SerialPort;
var serial = new SerialPort('/dev/ttyACM0',{
    baudrate:9600
});
http.listen(3000,function(){
    console.log('listen 3000 port');
});
var fs = require('fs');
var getData;

setInterval(function(){
fs.readFile('/media/IMATION USB/rttmp.txt', 'utf8', function(err, fd){
	if(err) console.log('file err ' + err);
	io.emit('recvmsg', fd);
	//console.log(fd);
})},5000);
 

app.use(express.static(__dirname + '/'));
app.use(express.static(__dirname + '/css/'));
app.get('/',function(req,res){
    res.sendfile('index.html');
});
 
serial.on('open',function(){
    console.log('open');
});
 
serial.on('data',function(data){
    io.emit('recvmsg',data.toString());
});

//繧ｯ繝ｩ繧､繧｢繝ｳ繝医°繧峨Γ繝�繧ｻ繝ｼ繧ｸ繧貞女縺大叙繧翫◎繧後ｒ繧ｷ繝ｪ繧｢繝ｫ縺ｧArduino縺ｸ
io.sockets.on('connection',function(socket){
	socket.on('sendmsg',function(data){
		serial.write(data,function(err,results){

		});
	});
});

io.sockets.on('connection',function(socket){
    socket.on('history',function(data){
        console.log(data);
        fs.readFile('/media/IMATION USB/2014-11-04.txt', 'utf8', function(err, fd){
            if(err) console.log('file err ' + err);
            io.emit('log_data',fd);
        });
    });
});


//繧ｰ繝ｩ繝墓緒逕ｻ逕ｨ
var WsServer = require('ws').Server;
var tid;

// WebSocket繧ｵ繝ｼ繝舌�ｼ菴懈��
var ws = new WsServer({
    host: '192.168.1.200',
    port: 8016
});

broadCast();// 繝�繝ｼ繧ｿ驟堺ｿ｡髢句ｧ�

// 繧ｯ繝ｩ繧､繧｢繝ｳ繝域磁邯壽凾繧､繝吶Φ繝�
ws.on('connection', function(socket) {
  console.log('conned: ' + ws.clients.length);
});

// 100ms豈弱↓繝�繝ｼ繧ｿ繧偵ヶ繝ｭ繝ｼ繝峨く繝｣繧ｹ繝医☆繧�
function broadCast(){
  tid = setInterval (function(){
    var fs = require('fs');
    var getData = [];
    var data = [];

    fs.readFile('/media/IMATION USB/rttmp.txt', 'utf8', function(err, fd){
      if(err) console.log('file err');
      getData = fd.toString().split(",");
      //console.log(getData);
        data[0]=getData[3].substr(-9);
        data[1]=getData[1];
        data[2]=getData[0];
        //console.log(data);

        ws.clients.forEach(function(client) { 
          client.send(JSON.stringify(data));
        });
    });


  }, 5000);
}


