//webサーバ
var express = require('express');
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

//クライアントからメッセージを受け取りそれをシリアルでArduinoへ
io.sockets.on('connection',function(socket){
	socket.on('sendmsg',function(data){
		serial.write(data,function(err,results){

		});
	});
});

io.sockets.on('connection',function(socket){
    socket.on('sendmsg',function(data){
        console.log(data);
    });
});


//グラフ描画用
var WsServer = require('ws').Server;
var tid;

// WebSocketサーバー作成
var ws = new WsServer({
    host: '192.168.1.200',
    port: 8016
});

broadCast();// データ配信開始

// クライアント接続時イベント
ws.on('connection', function(socket) {
  console.log('conned: ' + ws.clients.length);
});

// 100ms毎にデータをブロードキャストする
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


