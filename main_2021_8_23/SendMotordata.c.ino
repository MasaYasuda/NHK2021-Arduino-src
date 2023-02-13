
/** 
 * File  Encoder.ｃ
 * @author Niwa Yasuharu
 * latest renew　2018.12.29
 */

 
void TransferMoterData(byte Number, byte mode, int Speed){//  >>>void Send_Motordata(void)内で動く
  byte LowByte   = 0x00;
  byte HighByte  = 0x00;      
  byte speeddata = 0;  
  if (mode == 4) {
    Speed = (int)(255-abs(Speed)/8 + 221);
  }
  speeddata = (byte)abs(Speed);
  //ビット演算
  LowByte  = (speeddata & 0b01111111);
  HighByte = 0b10000000 | ((0b00001111 & Number)<<3) | ((0b00000011 & mode)<<1) | ((0b10000000 & speeddata)>>7);
  Serial2.write(0xFF);     //先頭判別データ
  Serial2.write(LowByte);  //LowByte  0b0SSSSSSS
  Serial2.write(HighByte); //HighByte 0b1ENNNMMS　<<<<EがN説ある
  byte lowbit[8]  = {0};
  byte highbit[8] = {0};
//  for(int i=0; i<8; i++){
//    lowbit[i]  = (LowByte >> i)  & B00000001; 
//    highbit[i] = (HighByte >> i) & B00000001; 
//    Serial.print("\nlow \nB0SSSSSSS ");
//    for(int i=7; i>=0; i--) Serial.print(lowbit[i]);
//    Serial.print("\nhigh \nB1NNNNMMS ");
//    for(int i=7; i>=0; i--) Serial.print(highbit[i]);
//  }
}



void Send_Motordata(void) {
  int tmd = 0;
  Speed[0] = (int)Vout[0];
  Speed[1] = (int)Vout[1];
  Speed[2] = (int)Vout[2];
  Speed[3] = (int)Vout[3];
  for(tmd = 0;tmd<=3;tmd++){
    TransferMoterData(Number[tmd],Mode[tmd],Speed[tmd]);     //モータ番号(0~4\][)をforでまとめた。
  }


//  for(int i = 0 ; i <= 5 ; i++)
//  {
//    Serial.print(Vout[i]);
//    Serial.print("__");
//  }
//  Serial.println("");
 // Serial.println(Vout[0]);
// Serial.print("__");
// Serial.println(Vout[1]);
//  Serial.print("__");
// Serial.println(Vout[2]);
//  Serial.print("         ");
// Serial.print(target_speed[0]    );
// Serial.print("         ");
// Serial.print(target_speed[1]    );
// Serial.print("         ");
// Serial.print(target_speed[2]);
// Serial.print("         ");
// Serial.print(holizontal_speed[1]);
// Serial.print("         ");
// Serial.println(vertical_speed[1]);
}


 
