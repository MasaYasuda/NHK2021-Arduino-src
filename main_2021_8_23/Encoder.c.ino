

/* File  Encoder.ｃ
 * @author Niwa Yasuharu
 * latest renew　2018.7.10.Sat
 * latest renew　2018.12.29
 * エンコーダの値を出力する関数群
 * ①Encoder＿Setup()を void Setup(void){}とloop(){}内で実行することにより6個のエンコーダの値を取得・出力できる。
 * ②Encoder_Velocity()でエンコーダ666msごとの変化率を計算。各メカナムホイールの回転数に対してPID制御を行うときに用いる。
 * ③その他の関数　Serial_...はloop(){}内で実行することによってPCに各値をシリアルモニタ出力できる。
 */


/*プロトタイプ宣言(関数の中身はこのページの一番下に記述)*/
void encoder1R(void); void encoder1F(void);
void encoder2R(void); void encoder2F(void);
void encoder3R(void); void encoder3F(void);
void encoder4R(void); void encoder4F(void);
void encoder5R(void); void encoder5F(void);
void encoder6R(void); void encoder6F(void);


/*エンコーダのピンの定義（Arduino MEGA 2560）*/
#define encoder1A 21  //D0
#define encoder1B 25  //A3
#define encoder2A 19  //D2
#define encoder2B 23  //A1
#define encoder3A 20  //D1
#define encoder3B 22  //A0
#define encoder4A 18  //D3
#define encoder4B 24  //A2
#define encoder5A 2   //E4
#define encoder5B 4   //G5
#define encoder6A 3   //E5
#define encoder6B 5   //E3


/* attachInterrupt()の割り込み番号はピンに対応している。unoは0と1のみ。megaは0から5(割り込み番号,ピン番号)=(0,2),(1,3),(2,21),(3,20),(4,19),(5,18) main_NHK の Setup(){} 内で実行すべき関数*/
void Encoder_Setup(void) {
  attachInterrupt(2, encoder1R, RISING);    
  attachInterrupt(2, encoder1F, FALLING);
  attachInterrupt(4, encoder2R, RISING);
  attachInterrupt(4, encoder2F, FALLING);
  attachInterrupt(5, encoder4F, FALLING);
  attachInterrupt(0, encoder5R, RISING);
  attachInterrupt(0, encoder5F, FALLING);
  attachInterrupt(1, encoder6R, RISING);
  attachInterrupt(1, encoder6F, FALLING);
  attachInterrupt(3, encoder3R, RISING);
  attachInterrupt(3, encoder3F, FALLING);
  attachInterrupt(5, encoder4R, RISING);
}
/*エンコーダの生値を出力*/
void Serial_Encoder_value(void){
  Serial.print(Encoder[0]);Serial.print("__");
  Serial.print(Encoder[1]);Serial.print("__");
  Serial.print(Encoder[2]);Serial.print("__");
  Serial.print(Encoder[3]);Serial.print("__");
  Serial.print(Encoder[4]);Serial.print("__");
  Serial.println(Encoder[5]);
  //Serial.print("______");  
}

/*エンコーダの速度の生値を出力*/
void Serial_Encoder_speed_value(void){
  Serial.print(sensor_speed[0]);Serial.print(",");
  Serial.print(sensor_speed[1]);Serial.print(",");
  Serial.print(sensor_speed[2]);Serial.print(",");
  Serial.print(sensor_speed[3]);Serial.print(",");
  Serial.print("__");
  Serial.print(encoder_omg[0]);Serial.print(",");
  Serial.print(encoder_omg[1]);Serial.print(",");
  Serial.print(encoder_omg[2]);Serial.print(",");
  Serial.print(encoder_omg[3]);Serial.print(",");
  Serial.print(+500);Serial.print(",");
  Serial.print(-500);Serial.println("");
}

/*エンコーダーの0.020秒あたりの変化率計算(タイマ割り込みを使うことでで57msごとに周期的に実行される)*/
long encoder_tmp[4] = {0,0,0,0};
#define resol 2048
void Encoder_Velocity(void) {
  for(int i=0; i<4; i++){
    //delayMicroseconds(100);
    long tmp1 = Encoder[i];
    sensor_speed[i] = (tmp1 - encoder_tmp[i]); //偏差計算 
//    Serial.print(tmp1);
//    Serial.print("__");
//    Serial.print(encoder_tmp[i]);
//    Serial.print("__");   
//    Serial.print(tmp1 - encoder_tmp[i]);
//    Serial.print("  "); 
    //delayMicroseconds(100);
    encoder_tmp[i]  = tmp1;
    encoder_omg[i] = sensor_speed[i] *360/resol *Pi/180 *50;
  }
//  Serial.println("");
}






//エンコーダのPIN番号は上から5,4,0,2,1,3




/* 同じEncoder[x]に関して＋-を反転させると正負の方向が逆転する
 * このプログラムではメカナムホイールを正方向に回した時,
 * エンコーダも正方向に増加するようにしている*/
void encoder1R(void) {
  if ((PINA & _BV(5)) == 0) Encoder[5]++;
  else                      Encoder[5]--;  
}
void encoder1F(void) {
  if ((PINA & _BV(5)) == 0) Encoder[5]++;
  else                      Encoder[5]--;  //d25
}
void encoder2R(void) {
  if ((PINA & _BV(1)) == 0) Encoder[0]--;
  else                      Encoder[0]++;
}
void encoder2F(void) {
  if ((PINA & _BV(1)) == 0) Encoder[0]--;
  else                      Encoder[0]++;  //d04
}
void encoder3R(void) {
  if ((PINA & _BV(3)) == 0) Encoder[4]++;
  else                      Encoder[4]--;
}
void encoder3F(void) {
  if ((PINA & _BV(3)) == 0) Encoder[4]--;
  else                      Encoder[4]++;  //d27
}
void encoder4R(void) {
  if ((PINA & _BV(0)) == 0) Encoder[2]++;
  else                      Encoder[2]--;
}
void encoder4F(void) {
  if ((PINA & _BV(0)) == 0) Encoder[2]++;  //d22
  else                      Encoder[2]--;
}
void encoder5R(void) {
  if ((PING & _BV(5)) == 0) Encoder[1]--;  
  else                      Encoder[1]++;
}
void encoder5F(void) {
  if ((PING & _BV(5)) == 0) Encoder[1]++;
  else                      Encoder[1]--; //d04
}
void encoder6R(void) {
  if ((PINE & _BV(3)) == 0) Encoder[3]++;
  else                      Encoder[3]--;
}
void encoder6F() {
  if ((PINE & _BV(3)) == 0) Encoder[3]++;
  else                      Encoder[3]--;  //d05
}
