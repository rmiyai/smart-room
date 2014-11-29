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
    var dataAry = mkData();
    ws.clients.forEach(function(client) { 
      client.send(JSON.stringify(dataAry));
    });
  }, 100);
}

// データ配列を作る
function mkData(){
    var data = [];
    var fs = require('fs');

    fs.readFile('/media/IMATION USB/rttmp.txt', 'utf8', function(err, fd){
        if(err) console.log('file err');
        data = fd.split(",");
    });

    var tmp = data[0];
    data[0] = data[2];
    data[2] = tmp;

    return data;
} 