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


