#include <Wire.h>

//温湿度LED
#include "DHT.h"

#define DHTPIN 14
#define DHTTYPE DHT22
#define LED 12
#define L2 7
#define PIEZO 11
#define DELAY_TIME 500
DHT dht(DHTPIN, DHTTYPE);

int ir_in = 8;
int ir_out = 13;
unsigned int data[] = {349,352,86,88,86,264,86,262,88,264,86,264,86,264,86,89,85,89,86,89,86,264,86,265,86,264,86,264,86,264,86,89,86,89,86,265,86,264,86,89,86,89,86,88,86,89,86,89,86,89,86,264,86,265,86,89,86,89,86,88,86,88,86,88,86,89,349,352,86,88,86,264,86,264,86,264,86,264,86,264,86,89,86,88,86,89,86,264,86,265,86,264,86,264,86,265,86,89,86,89,86,264,86,264,86,89,86,89,86,89,86,89,86,89,86,89,86,265,86,264,86,89,86,89,86,88,86,89,85,89,86,89,349,352,86,1405,349,352,86,88,86,89,86,88,86,89,86,264,86,89,86,264,86,264,86,89,86,89,86,89,86,88,86,264,86,89,86,264,86,264,86,89,86,265,86,264,86,89,86,264,86,265,86,89,86,89,86,89,86,264,86,264,86,88,86,264,86,264,86,88,86,89,349,352,86,89,86,89,85,90,86,89,86,265,86,89,86,264,86,264,86,89,86,89,86,88,86,89,85,265,86,89,86,264,86,264,86,89,86,265,86,264,86,89,86,264,86,265,86,89,86,89,86,89,86,264,86,264,86,89,86,265,86,264,86,89,86,89,349,352,86};

int last = 0;
unsigned long us = micros();
//ここまで温湿度LED

#define SENSOR_ADRS 0x60                  // MPL115A2のI2Cアドレス
#define AVE_NUM     20                    // 圧力・温度のＡ／Ｄ変換値を平均化する回数
#define H_CORRECT   80                    // 自宅でのセンサと実際の高度差補正値(My自宅の標高は100m)

float a0 , b1 , b2 , c12 ;                // 係数のデータを保存する変数
unsigned long Press , Temp ;              // 圧力および温度の変換値を保存する変数

void setup()
{
     // シリアルモニターの設定
     Serial.begin(9600) ;
     
     //温度湿度LED
     pinMode(ir_in, INPUT);  // 入出力ピンの設定
     pinMode(ir_out, OUTPUT); 
     pinMode(LED, OUTPUT);
     pinMode(L2, OUTPUT);
     pinMode(PIEZO, OUTPUT);
     Serial.println("START");
     dht.begin();
     //ここまで温湿LED
     
     // Ｉ２Ｃの初期化
     Wire.begin() ;                       // マスターとする

     delay(3000) ;                        // 3Sしたら開始
     CoefficientRead() ;                  // メモリーマップから係数を読み出して置く
}
//void spk(){
//  for(int i=0; i<256; i++){
//    tone(PIEZO, i*10, 20);
//    delay(5);
//  }
//}  
void loop()
{
    if(digitalRead(8) == HIGH){
      digitalWrite(L2, HIGH);
      //spk();
      
    }else{
      digitalWrite(L2, LOW);
    }
  
     //シリアルチェック
     //Serial.print("\n");
     digitalWrite(LED, LOW);
     while(Serial.available() > 0){
       char serial_recv = Serial.read();
       switch(serial_recv){
         case 'o':
         sendSignal();
         digitalWrite(LED, HIGH);
         break;
       }
     }
  
     int i ;
     float ans ;
     unsigned long p , t ;

     p = t = 0 ;
     for (i=0 ; i < AVE_NUM ; i++) {      // ２０回読み込んで平均化する
          PressureRead() ;                // メモリーマップから圧力および温度のＡ／Ｄ変換値を読み出す
          p = p + Press ;
          t = t + Temp ;
     }
     Press = p / AVE_NUM ;
     Temp  = t / AVE_NUM ;

     delay(2000) ;                        // 2秒後に繰り返す
     

     float h = dht.readHumidity();
     float tm = dht.readTemperature();
     float f = dht.readTemperature(true);
  
     if (isnan(h) || isnan(t) || isnan(f)) {
       Serial.println("Failed to read from DHT sensor!");
       return;
     }
     Serial.print(h);
     Serial.print(",");
     Serial.print(tm);
     //Serial.print("\n");
     
     ans = PressureCalc() ;               // 気圧値の計算を行う
     Serial.print(",");
     Serial.print(ans) ;                  // 気圧値の表示を行う
     Serial.print("\n");
     ans = AltitudeCalc(ans,H_CORRECT) ;  // 高度の計算を行う
//     Serial.print(ans) ;                  // 高度の表示を行う
//     Serial.println(" m") ;
}

// dataからリモコン信号を送信
void sendSignal() {
  for (int cnt = 0; cnt < 512; cnt++) {
    unsigned long len = data[cnt]*10;  // dataは10us単位でON/OFF時間を記録している
    if (len == 0) break;      // 0なら終端。
    unsigned long us = micros();
    do {
      digitalWrite(ir_out, 1 - (cnt&1)); // iが偶数なら赤外線ON、奇数なら0のOFFのまま
      delayMicroseconds(8);  // キャリア周波数38kHzでON/OFFするよう時間調整
      digitalWrite(ir_out, 0);
      delayMicroseconds(7);
    } while (long(us + len - micros()) > 0); // 送信時間に達するまでループ
  }
  Serial.print("OK\n");
}

// シリアルからの受信をチェック
void checkSerial() {
  if (Serial.available() > 0) {
    if (Serial.read() == ' ') {  // スペース受信で入力開始
      Serial.print(">");
      unsigned int x = 0;
      int i = 0;
      int len = 0;
      while (1) {
        while (Serial.available() == 0) {} // 次のバイトが来るまで無限ループで待つ
        int a = Serial.read();
        if (a >= '0' && a <= '9') {    // 0～9を受信なら
          len++;
          x *= 10;
          x += a - '0';
        } else if (len) {    // 数値を受信済み、かつ数値以外が来たら
          data[i++] = x; // 受信した数値をdataに記憶
          Serial.print(i);
          Serial.print(":");
          Serial.print(x);
          Serial.print("\t");
          if (x == 0 || i >= 512) { // 0受信で赤外線送信開始
            sendSignal();
            break;
          }
          x = 0;
          len = 0;
        }
      }
    }
  }
}

// メモリーマップから係数を読み出す処理
int CoefficientRead()
{
     int ans ;
     unsigned int h , l ;

     Wire.beginTransmission(SENSOR_ADRS) ;        // 通信の開始
     Wire.write(0x04) ;                           // 係数の読出しコマンド発行
     ans = Wire.endTransmission() ;               // データの送信と通信の終了
     if (ans == 0) {
          ans = Wire.requestFrom(SENSOR_ADRS,8) ; // 係数データの受信を行う
          if (ans == 8) {
               // ａ０の係数を得る
               h = Wire.read() ;
               l = Wire.read() ;
               a0 = (h << 5) + (l >> 3) + (l & 0x07) / 8.0 ;
               // ｂ１の係数を得る
               h = Wire.read() ;
               l = Wire.read() ;
               b1 = ( ( ( (h & 0x1F) * 0x100 ) + l ) / 8192.0 ) - 3 ;
               // ｂ２の係数を得る
               h = Wire.read() ;
               l = Wire.read() ;
               b2 = ( ( ( ( h - 0x80) << 8 ) + l ) / 16384.0 ) - 2 ;
               // Ｃ１２の係数を得る
               h = Wire.read() ;
               l = Wire.read() ;
               c12 = ( ( ( h * 0x100 ) + l ) / 16777216.0 )  ;
               ans = 0 ;
          } else ans = 5 ;
     }

     return ans ;
}
// メモリーマップから圧力および温度のＡ／Ｄ変換値を読み出す処理
int PressureRead()
{
     int ans ;
     unsigned int h , l ;

     // 圧力および温度の変換を開始させる処理
     Wire.beginTransmission(SENSOR_ADRS) ;        // 通信の開始
     Wire.write(0x12) ;                           // 圧力・温度の変換開始コマンド発行
     Wire.write(0x01) ;
     ans = Wire.endTransmission() ;               // データの送信と通信の終了
     if (ans != 0) return ans ;
     delay(3) ;                                   // 変換完了まで３ｍｓ待つ

     // Ａ／Ｄ変換値を得る処理
     Wire.beginTransmission(SENSOR_ADRS) ;        // 通信の開始
     Wire.write(0x00) ;                           // 圧力のHighバイトから読込むコマンド発行
     ans = Wire.endTransmission() ;               // データの送信と通信の終了
     if (ans == 0) {
          ans = Wire.requestFrom(SENSOR_ADRS,4) ; // Ａ／Ｄ変換値データの受信を行う
          if (ans == 4) {
               // 圧力のＡ／Ｄ変換値を得る
               h = Wire.read() ;
               l = Wire.read() ;
               Press = ( ( h * 256 ) + l ) / 64 ;
               // 温度のＡ／Ｄ変換値を得る
               h = Wire.read() ;
               l = Wire.read() ;
               Temp = ( ( h * 256 ) + l ) / 64 ;
               ans = 0 ;
          } else ans = 5 ;
     }

     return ans ;
}
// 気圧値(hPa)を計算する処理
float PressureCalc()
{
     float ret , f ;

     f = a0 + ( b1 + c12 * Temp ) * Press + b2 * Temp ;
     ret = f * ( 650.0 / 1023.0 ) + 500.0 ;
     return ret ;
}
// 気圧値(hPa)から高度を計算する処理
float AltitudeCalc(float pressure,int Difference)
{
     float h ;

     h = 44330.8 * (1.0 - pow( (pressure/1013.25) ,  0.190263 )) ;
     h = h + Difference ;
     return h ;
}
