
/**
 * File  PS4.c
 * @author Niwa Yasuharu
 * latest renew　2018.12.30
 * 参考元URL https://github.com/felis/USB_Host_Shield_2.0
 *【注意】MEGA2560の10,11,12,13番ピンはUsbHostShield2.0に用いるため、他の用途では使用することができない 
 */



/*UsbHostShield2.0とPS4コントローラ、Aruduino_MEGA2560 の通信を開始するための関数*/
void Arduino_UsbHostShield2_Bluetooth_setup(void) {
    delay(100);  //MEGAとUsbHostShield2.0の通信の開始を少し待たないとArduinoの再起動時にBluetoothドングルが反応しなくなってしまう事(PS4コンがつなげられない症状)がある。
    #if !defined(__MIPSEL__)
    while (!Serial){;;} // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
    #endif
    if (Usb.Init() == -1) {Serial.println(F("\r\nOSC did not start")); while (1);}
    Serial.println(F("\r\nPS4 Bluetooth Library Started"));
}









void Arduino_UsbHostShield2_connect_PS4(void)
{
  static long int drct;//正面のモード
  static byte cnt =0;
  static byte cvr =0;
  static byte aut =0;
  static byte kick_drct = 0;
  Usb.Task();
  if(PS4.connected())  //PS4コントローラとの接続が正常な場合
  {
     if(PS4.getButtonPress(OPTIONS)){Air_pinMode(1,'HIGH');}
     else                           {Air_pinMode(1,'LOW'); }
     
//     if(PS4.getButtonClick(OPTIONS)){
//          cvt=(cvt+1)%2;
//        }
//        if(cvt == 1){
//          Air_pinMode(1,'HIGH');         
//        }
//        else{
//          Air_pinMode(1,'LOW');
//        }

    /*左スティック____________________________________4.getA_______*/
    left.xHat  = (double)(PS4.getAnalogHat(LeftHatX)-127)/127;
    left.yHat  = (double)(127-PS4.getAnalogHat(LeftHatY))/127;
    /*アナログスティックの傾きを計算*/
    left.rake  = constrain(sqrt( (left.xHat * left.xHat) + (left.yHat * left.yHat)),  0.0,  1.0);
    if(left.rake < 0.1) left.rake = 0.0;
    /*アナログスティックの角度を計算(数学の図形と同じで右から±180度)*/
    if(left.rake > 0){
      left.degree = Degree(atan2(left.yHat,left.xHat)); 
    }
    else left.degree = 90.0;
    
//         if (-135 <= left.degree  &&  left.degree <= -45)  left.degree = 270;
//    else if ( 45  <= left.degree  &&  left.degree <=  135) left.degree = 90;
//    else if (-45  <= left.degree  &&  left.degree <=  45)  left.degree = 0;
//    else                                                   left.degree = 180;

         if (  -67.5<= left.degree  &&  left.degree <= -22.5) left.degree = 315;
    else if ( -111.5<= left.degree  &&  left.degree <= -67.5) left.degree = 270;
    else if ( -157.5<= left.degree  &&  left.degree <=-111.5) left.degree = 225;
    else if (  111.5<= left.degree  &&  left.degree <= 157.5) left.degree = 135;
    else if (   67.5<= left.degree  &&  left.degree <= 111.5) left.degree = 90;
    else if (   22.5<= left.degree  &&  left.degree <=  67.5) left.degree = 45;
    else if (  -22.5<= left.degree  &&  left.degree <=  22.5) left.degree = 0;
    else                                                      left.degree = 180;



    /*右スティック_____________________________________________*/
    right.xHat  = (double)(PS4.getAnalogHat(RightHatX)-127)/127;
    right.yHat  = (double)(127-PS4.getAnalogHat(RightHatY))/127;
    right.rake  = constrain(sqrt( (right.xHat * right.xHat) + (right.yHat * right.yHat)),  0.0,  1.0);
    if(right.rake < 0.1) right.rake = 0.0;
    if(right.rake > 0)right.degree = Degree(atan2(right.yHat,right.xHat));
    else right.degree = 90.0;
    if      ((-180<= right.degree && right.degree <=-90)  ||   90 <= right.degree  &&  right.degree <= 180) right.degree = 0;
    else if (-90  <= right.degree && right.degree <= 90) right.degree = 180;
    //else right.degree = 0;


      if(PS4.getButtonPress(OPTIONS)){Air_pinMode(1,'HIGH');}
     else                           {Air_pinMode(1,'LOW'); }
  if(PS4.getButtonPress(SHARE)){Air_pinMode(0,'HIGH');}
     else                           {Air_pinMode(0,'LOW'); }

    
    /*ボタンデータ_____________*/
   if(FIELD_COLOR == 'Green'){
//      Serial.print(tms); Serial.print("_");Serial.println(millis());


  
    


//4輪のときは、0～3のモータ使用のため、汎用性を持たすために[3]は開けておく。

//        if(PS4.getButtonClick(LEFT)) cruizcore_standard_deg = cruizcore_first_deg + KICK_DEG;
//        if(PS4.getButtonClick(RIGHT))cruizcore_standard_deg = cruizcore_first_deg - KICK_DEG + 180;
//        if(PS4.getButtonClick(UP))   cruizcore_standard_deg = cruizcore_first_deg;

        if(PS4.getButtonPress(R1)){
          Mode[4]=2;
          Speed[4]=180;
        }
        else if(PS4.getButtonPress(R2)){
          Mode[4]=1;
          Speed[4]=180;
        }
        else{
          Mode[4]=0;
          Speed[4]=0;
        }

                if(PS4.getButtonPress(UP)){
          Mode[5]=2;
          Speed[5]=50;
        }
        else if(PS4.getButtonPress(DOWN)){
          Mode[5]=1;
          Speed[5]=50;
        }
        else{
          Mode[5]=0;
          Speed[5]=0;
        }

         if(PS4.getButtonPress(SQUARE)){
          Mode[6]=2;
          Speed[6]=50;
        }
        else if(PS4.getButtonPress(CIRCLE)){
          Mode[6]=1;
          Speed[6]=50;
        }
        else{
          Mode[6]=0;
          Speed[6]=0;
        }
//
//        if(PS4.getButtonPress(LEFT)){
//          Mode[4]=2;
//          Speed[4]=20;
//          Mode[5]=2;
//          Speed[5]=20;
//        }
//        else if(PS4.getButtonPress(RIGHT)){
//          Mode[4]=1;
//          Speed[4]=210;
//          Mode[5]=1;
//          Speed[5]=210;
//        }
//        else{
//          Mode[4]=0;
//          Speed[4]=0;
//          Mode[5]=0;
//          Speed[5]=0;
//        }

        

//        if(PS4.getButtonClick(SQUARE)){set_pot_val++;first_pot_val++;}
//        if(PS4.getButtonClick(CROSS)){set_pot_val--;first_pot_val--;}
 /*
        if(PS4.getButtonClick(R1)){
          kick_drct = 1;
          //tms = millis();
          Mode[4]=2;
          Mode[5]=2;
        }
        else if(PS4.getButtonClick(R2)){
          kick_drct = 2;
          //tms = millis();
          for(int j=0;j<4;j++){
            Mode[j]=3;Speed[j]=0;
          }
          Mode[4]=1;Speed[4]=220;
          Mode[5]=1;Speed[5]=220;
        }
        else if(PS4.getButtonClick(L2)){
          Mode[4]=0;Speed[4]=0;
          Mode[5]=0;Speed[5]=0;
          kick_drct = 0;
        }
        
        switch (kick_drct)
        {
          case 1:
          if((set_pot_val - std_pot_val) < 0)
          {
            Speed[4] = constrain((abs(set_pot_val - std_pot_val))*15,0,60);
            Speed[5] = constrain((abs(set_pot_val - std_pot_val))*15,0,60);
            Mode[4] = 1;
            Mode[5] = 1;
          }
          else
          {
            Speed[4] = constrain((set_pot_val - std_pot_val)*15,0,60);
            Speed[5] = constrain((set_pot_val - std_pot_val)*15,0,60);
            Mode[4] = 2;
            Mode[5] = 2;
          }
          break;
          case 2:
          if(std_pot_val < first_pot_val)
           {
            Mode[4]=0;Speed[4]=0;
            Mode[5]=0;Speed[5]=0;
            kick_drct = 0;
          }
          
          break;
          default:
          if(first_pot_val - std_pot_val < 0)
          {
            Speed[4] = constrain((abs(first_pot_val - std_pot_val))*8,0,15);
            Speed[5] = constrain((abs(first_pot_val - std_pot_val))*8,0,15);
            Mode[4] = 1;
            Mode[5] = 1;
          }
          else
          {
            Speed[4] = constrain((first_pot_val - std_pot_val)*8,0,15);
            Speed[5] = constrain((first_pot_val - std_pot_val)*8,0,15);
            Mode[4] = 2;
            Mode[5] = 2;
          }
          break;
        }
        */
   }
//   if(FIELD_COLOR == 'White')
//   {
//    if(PS4.getButtonClick(UP))
//    {
//      D2(400,400);
//    }
//    if(PS4.getButtonClick(LEFT))
//    {
//      D2(0,0);
//    }
//   }
  }

 
  
  /*接続が何らかの理由で切断されてしまったとき (PS4コントローラの充電不足などによりおこる事がある)*/
  else {
    /*接続が切れたとき最後に入力した情報が継続されマシンが動き続けてしまうのを防ぐ*/
    /*全てのPS4コントロ－ラのデータを初期値に戻すことでマシンの暴走を防ぐ(ただし自動モードは継続してしまうことに注意)*/    
    left.rake    = 0; /*左右左スティックの傾きを強制的に0にすることで手動モードのマシンを停止させる。*/
    left.degree  = 0;
    right.rake   = 0;
    right.degree = 0;
  }
  

  
}
