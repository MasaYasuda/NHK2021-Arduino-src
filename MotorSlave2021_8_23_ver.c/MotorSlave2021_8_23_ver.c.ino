
/* remaked by Ryuma Mochizuki
 * start        2020.11.06（試作）
 * latest renew 2020.11.06
 * 環境設定文字の大きさを13にすると見やすくなる
 */

  
//関数のプロトタイプ宣言
void Moter(int Number, int Mode, int Speed);
byte SerialRead(void);

//1つのモータスレーブで最大3つのモータを回すことができる。通信で「拾う」モーター番号
/*1~4までは足回り用
  5は回収機構用
  6,7は角度調整機構用
  8,9はショット機構用
  MD20Aを使う場合、
*/
//byte MOTOR0 = 0;   //0,1,2,3:MD20A 自作MDとは違うコードのため注意
//byte MOTOR1 = 1;
//byte MOTOR2 = 2;

byte MOTOR0 = 3;   
byte MOTOR1 = 4;   
byte MOTOR2 = 5;   
                     
//byte MOTOR0 = 6;
//byte MOTOR1 = 7;
//byte MOTOR2 = 8;


//ピン番号配列
byte pin[3][3] = {{4,5,6}, {12,10,11}, {14,3,9}}; //3つのモータの制御pinの番号を記憶しておくための配列
long timer1[3] = {0,0,0};
long timer2[3] = {0,0,0};
int  number    =  0;       //受信したデータからモータ番号を取り出しこの配列に代入する
int  Mode[3]   = {3,3,3};  //受信したデータから回転もモードを取り出しこの配列に代入する(停止状態に初期化)
int  Speed[3]  = {0,0,0};  //受信したデータから速度を取り出しこの配列に代入する(速度0に初期化)
byte Tmp1 = 0;     //通信バッファ1
int  Tmp2 = 0;     //通信バッファ2
byte HighByte = 0; //上位バイト
byte LowByte  = 0; //下位バイト
byte check  = 0;

byte after  = 0;
byte before = 0;
byte before_mode[3] = {0,0,0};
long safty1 = 0;





void setup(void){
  Serial.begin(115200); //ここの速度はマスター側の通信速度と合わせる
  for(int i=3; i<=11; i++){pinMode(i,OUTPUT);} //ピンの設定 3～11pinを出力に設定する
  //pwm周期を変更することで音が出ないようにしている
  TCCR0B &= B11111000;
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(14, OUTPUT);
  TCCR0B |= B00000001; TCCR1B &= B11111000;
  TCCR1B |= B00000001; TCCR2B &= B11111000;
  TCCR2B |= B00000001;
  //3つのタイヤ全てを固定状態にする
  Moter(0, 0, 0);
  Moter(1, 0, 0);
  Moter(2, 0, 0);
}



/*
 ＜通信内容＞
【注意】マスター側で1つのデータに対して下位バイト→上位バイトの順に2回に分けて受信
 1つのモータのデータは2バイト(2進数8桁 例 00000000)で送られる。
 [HighByte]1 　通信番号・1で固定ここで上位下位を判別
           0￢
           0 |
           0 | モーター番号2^4=16個（津田先輩のプログラムとここが違う）
           0 」
           0￢
           0 」Mode (0b00=0≪フリー回転≫　0b01=1≪前回転≫　0b10=2≪後回転≫　0b11=3≪停止≫　★速度221以上のときブレーキ )
           0　 Speedの上位1bit
 [LowByte] 0   通信番号・0で固定ここで上位下位を判別
           0￢
           0 |
           0 |
           0 | Speedの2^8=256段階(0～255)のうちの下位7bit
           0 |
           0 |
           0 」　　 */
 
void loop(void){
  
  if (Serial.available() >= 3){     //マスタースレーブから2バイト以上のデータがデータ貯蔵庫(バッファ)に送られている時 
    Tmp1 = Serial.read();           //1バイトデータを読む
    if(Tmp1 == 0xFF){                //読んだデータが先頭判別データだったら(3回に1回は先頭判別データ)      
      //Serial.println("0xFF !");    //先頭判別データの取得に成功
      LowByte  = Serial.read();     //下位1バイトを取得
      HighByte = Serial.read();     //上位1バイトを取得

      //受信したデータを出力
      /*byte lowbit[8]  = {0}; //初期化
      byte highbit[8] = {0}; //初期化
      for(int i=0; i<8; i++){
        lowbit[i]  = (LowByte >> i)  & B00000001;
        highbit[i] = (HighByte >> i) & B00000001; 
      }Serial.print("B0SSSSSSS " ); for(int i=7; i>=0; i--){Serial.print(lowbit[i]);}
      Serial.print("\nB1NNNNMMS "); for(int i=7; i>=0; i--){Serial.print((highbit[i]));}
      Serial.println("");
      */
      
      if(LowByte>>7 == 0b00000000  &&  HighByte>>7 == 0b00000001)  //上位,下位の２バイトの受信に成功したら
      {
        //Serial.println("LowByte  HighByte !");
        Tmp2 = (int)(HighByte & 0b01111000)>>3;   //モータ番号を取得
        if      (Tmp2 == MOTOR0) {number = 0; check = 1;}
        else if (Tmp2 == MOTOR1) {number = 1; check = 1;}
        else if (Tmp2 == MOTOR2) {number = 2; check = 1;}
        if(check == 1){
          Mode[number]  = (int)(HighByte & 0b00000110)>>1;                          //モードを取り出す
          Speed[number] = (int)(HighByte & 0b00000001)<<7 | (LowByte & 0b01111111);  //速度を取り出す
          
          /*Serial.println("number = "  + (String)Tmp2);
          Serial.println("Mode   = "  + (String)Mode[number]);
          Serial.println("Speed  = "  + (String)Speed[number]);*/
          
          if(Speed[number] > 221){ //速度221超過だったらブレーキの支持を出す。
            Mode[number] = 4;
            Speed[number] = (Speed[number] - 221) * 8;
            //Serial.println("brake mode !!!");
          }else if(Speed[number] > 0){
            if     (Mode[number] == 1) before_mode[number] = 1;
            else if(Mode[number] == 2) before_mode[number] = 2;
            //Serial.println("normal outputs !!!");
          }check = 0;
        }//else  Serial.println("none same number");       //モータ番号が担当するモータ番号と合致しなかった
      }//else  Serial.println("not success get high low"); //通信データが上位下位の1セットでなかった
    }//else  Serial.println("not header reloading");     //通信データがデータの先頭でなかった
    
    Moter(0, Mode[0], Speed[0]);   //出力0
    Moter(1, Mode[1], Speed[1]);   //出力1
    Moter(2, Mode[2], Speed[2]);   //出力2
    safty1 = millis();             //最後にデータを受け取った時間を記録する
  }
  else if (millis() - safty1 > 100 * 64) { //70msデータが受信されないと緊急停止する
    while (Serial.available() <= 3){
      Moter(0, 3, 0);
      Moter(1, 3, 0);
      Moter(2, 3, 0);
      //Serial.println("waiting 3 bytes");
    }
  }

}





//出力関数
void Moter(int Number, int Mode, int Speed){
  //回転までに一定時間(5ms)≪フリー回転≫状態で待つ  
  if(Mode == 1) {if (millis() - timer2[Number] < 5 * 64) {Mode = 0;}}
  if(Mode == 2) {if (millis() - timer1[Number] < 5 * 64) {Mode = 0;}}
  //モータの回転モードに対するswitch文【Mode】の値によって
  //Mode=0≪フリー回転≫ Mode=1≪前回転≫ Mode=2≪後回転≫ Mode=3≪停止≫ Mode=4≪ブレーキ≫
  if(1){
    switch (Mode){    
      case 0:  //free≪フリー回転≫
        digitalWrite(pin[Number][0], LOW);
        analogWrite(pin[Number][1], LOW);
        analogWrite(pin[Number][2], LOW);
        break;
      case 1:  //Advace≪前回転≫
        digitalWrite (pin[Number][0], LOW);
        analogWrite(pin[Number][1], LOW);
        analogWrite(pin[Number][2], Speed);
        timer1[Number] = millis();
        break;
      case 2:  //Reverse≪後回転≫
        digitalWrite(pin[Number][0], HIGH);
        analogWrite(pin[Number][1], LOW);
        analogWrite (pin[Number][2], Speed);
        timer2[Number] = millis();
        break;
      case 3:  //Stop≪停止≫
        digitalWrite(pin[Number][0], HIGH);
        digitalWrite(pin[Number][1], LOW);
        digitalWrite(pin[Number][2], LOW);
        break;
      case 4:  //brake≪ブレーキ≫
        if (before_mode[Number] == 2) {
          digitalWrite (pin[Number][0], HIGH);
          analogWrite(pin[Number][1], LOW);
          analogWrite (pin[Number][2], Speed);
        }else{
          digitalWrite (pin[Number][0], HIGH);
          analogWrite(pin[Number][1], Speed);
          analogWrite (pin[Number][2], LOW);
        }
        break;
    }
  }else{
    switch (Mode){    
      case 0:  //free≪フリー回転≫
        digitalWrite(pin[Number][0], LOW);
        analogWrite(pin[Number][1], LOW);
        analogWrite(pin[Number][2], LOW);
        Serial.println("フリー");
        break;
      case 1:  //Advace≪前回転≫
        digitalWrite (pin[Number][0], HIGH);
        analogWrite(pin[Number][1], Speed);
        analogWrite(pin[Number][2], LOW);
        Serial.println("前転");
        timer1[Number] = millis();
        break;
      case 2:  //Reverse≪後回転≫
        digitalWrite(pin[Number][0], HIGH);
        analogWrite(pin[Number][1], LOW);
        analogWrite (pin[Number][2], Speed);
        Serial.println("後転");
        timer2[Number] = millis();
        break;
      case 3:  //Stop≪停止≫
        digitalWrite(pin[Number][0], HIGH);
        digitalWrite(pin[Number][1], LOW);
        digitalWrite(pin[Number][2], LOW);
        break;
      case 4:  //brake≪ブレーキ≫
        if (before_mode[Number] == 2) {
          digitalWrite (pin[Number][0], HIGH);
          analogWrite(pin[Number][1], LOW);
          analogWrite (pin[Number][2], Speed);
        }else{
          digitalWrite (pin[Number][0], HIGH);
          analogWrite(pin[Number][1], Speed);
          analogWrite (pin[Number][2], LOW);
        }
        break;
    }
  }
}
