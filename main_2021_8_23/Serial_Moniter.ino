


/*シリアルモニタまとめ関数*/
void Serial_Monitor(void)
{  
//    for(int i=0;i<4;i++){
//      Serial.print(Mode[i]);  Serial.print("_");
//    }
//    Serial.println("");
  //Serial.print(cruizcore_degree);  Serial.print("__");  

  Serial_Encoder_value();
//  Serial_Encoder_speed_value();
// Serial_Pot_value ();

//Serial.println(target_speed[1]);
//  for(int i=0;i<4;i++){
//    Serial.print(target_speed[i]);Serial.print("__");
//  }//Serial.println("");
//  Serial.print("  ");
//   for(int i=0;i<4;i++){
//    Serial.print(sensor_speed[i]);Serial.print("__");
//  }Serial.println("");


//Serial.print(right.rake);Serial.print(",");
//Serial.print(left.rake);Serial.print(",");
//Serial.print(1);Serial.print(",");
//Serial.print(-1);Serial.print(",");


//  for(int i=0;i<4;i++){
//    Serial.print(target_speed[i]);Serial.print(",");      //{0,1,2,3}={青色，赤色，緑色，黄色}
//  }
//  Serial.print(50);Serial.print(",");
//  Serial.print(-50);Serial.print(",");

//    Serial.print(target_speed[3]);Serial.print(","); //青色
//    Serial.print(sensor_speed[3]);Serial.print(","); //赤色
//    Serial.print(0); Serial.print(",");              //緑色
//    Serial.print(100); Serial.println("");          //  
  
//  Serial.print  ("cruizcore_degree = "         + (String)cruizcore_degree);       //ジャイロセンサ
//  Serial.print  ("\tcruizcore_standard_deg = " + (String)cruizcore_standard_deg); //ジャイロセンサ
//  Serial.print  ("\trake_cruizcore = ");
//  Serial.print  (rake_cruizcore,6);
//  Serial.print  ("\ttarget_speed = ");
//  Serial.println(target_speed[0],6);

//  Serial.print(cruizcore_standard_deg);Serial.print(","); //緑色
//  Serial.print(cruizcore_degree);      Serial.print(","); //黄色
//  Serial.print(target_speed[3]/10);  Serial.println(""); //黄色

//  Serial.print(Encoder[4]); Serial.print(","); //黄色
//  Serial.print(target_x);   Serial.print(","); //黄色
//  Serial.print(200); Serial.print(","); //黄色
//  Serial.print(-50); Serial.println(""); //黄色

  //PS4関連
//  Serial.print(F("\txHat_"));  Serial.print((double)left.xHat);
//  Serial.print(F("\tyHat_"));    Serial.println((double)left.yHat);
//    Serial.print(F("\tLrake"));  
//  Serial.print((double)left.rake);
//    Serial.print(F("\tLdegree"));  
//  Serial.println((double)left.degree);
//    Serial.print(F("\tRrake"));    
//  Serial.print((double)right.rake);
//    Serial.print(F("\tRdegree"));  
//  Serial.println((double)right.degree);

//    Serial.println ("auto_rake_x"       + (String)auto_rake_x
//                   + "\tauto_rake_y"     + (String)auto_rake_y
//                   + "\t\tauto_degree"   + (String)auto_degree
//                   + "\tauto_rake"       + (String)auto_rake
//                   + "\tEncoder[4] = "   + (String)Encoder[4]
//                   + "\tEncoder[5] = "   + (String)Encoder[5]
//                   + "\ttarget_speed[0]" + (String)target_speed[0]
//                   + "\tSpeed[0]" + (String)Speed[0]);

//  Serial.println("Speed[0]" + (String)Speed[0]
//                 + "\tSpeed[1]" + (String)Speed[1]
//                 + "\tSpeed[2]" + (String)Speed[2]
//                 + "\tSpeed[3]" + (String)Speed[3]);
                 

}
