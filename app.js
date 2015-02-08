//webサーバ
var express = require('express');
var app = express();
var exec = require('child_process').exec;

process.on('uncaughtException', function(err) {
    console.log(err);
});

var http = require('http').Server(app);
var io = require('socket.io')(http);
var SerialPort = require('serialport').SerialPort;
var serial = new SerialPort('/dev/ttyACM0',{
    baudrate:9600
});
http.listen(8000,function(){
    console.log('listen 8000 port');
});
var getData;
var fs = require('fs');
var readline = require('readline');
var data_array;

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
    socket.on('getDir',function(data){
        fs.readdir('/media/IMATION USB/',function(err, files){
            if(err) console.log(err);
            files.sort();
            io.emit('init',files);
        });
    });
    //ファイルの内容をwsで送信
    socket.on('history',function(data){
        fs.readFile('/media/IMATION USB/'+data, 'utf8', function (err, text){
            io.emit('text', text);
        })
    });

    socket.on('serverData', function(data){
        console.log(data);
        var child = exec('sh /home/pi/t_cpu.sh', function(err, stdout, stderr){
            if(!err){
                console.log('stdout' + stdout);
                console.log('stderr' + stderr);
                io.emit('result', stdout);
            }else{
                console.log(err);
                console.log(err.code);
                console.log(err.signal);
            }
        });
    });
});



