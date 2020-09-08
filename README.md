# SmartRoom

Web上から室内のモニタリングと家電を操作をするシステム
![smartroom](https://user-images.githubusercontent.com/8679421/92490078-82c9cc00-f22b-11ea-917d-a705284b424f.jpg)

## 環境
* RaspberryPi2 Model B+
* Arduino
* Node.js

## 機能
* 温湿度のリアルタイムモニタリング（グラフ）
* 温湿度、気圧の表示
* 家電の操作
  - あらかじめ記憶した赤外線リモコンの信号をArduinoを介して送信する
* [Rctank](https://github.com/rmiyai/Rctank)と連携したカメラ画像の表示
* [Rctank](https://github.com/rmiyai/Rctank)で撮影した画像の閲覧
* 温湿度のログをグラフで表示
