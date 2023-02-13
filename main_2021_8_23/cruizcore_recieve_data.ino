
void Recieve_cruizcore_data(void) {
  byte data_packet[2] = {0x00, 0x00}; //通信パケットの定義  
  //シリアル3通信バッファに3バイト以上データが来ていたら,  
  if(Serial2.available() >= 3) {
    byte tmp = Serial2.read();
    if(tmp == 0xFF){ //先頭判別データの受信に成功したら
      //Serial.println("0xFF");
      //double型 4バイトにあたるデータを下位バイト上位バイトの順に１バイトずつ読み込む
      data_packet[0] = Serial2.read(); //上位バイト
      data_packet[1] = Serial2.read(); //下位バイト
      if((data_packet[0] != 0xAA) && (data_packet[1] != 0xAA)){
        cruizcore_degree = (double)((data_packet[0] << 8) | data_packet[1])/100; //先頭アドレスから4バイト分のデータを読み込み　double型のデータとして参照する
      }
    }
  }//Serial.println("waiting Serial3");
}
