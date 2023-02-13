



/*タイマ2割り込み処理 1msごとにカッコ内が処理される)*/

void timer2_interrupt(void)
{
  static unsigned long Timer2 = 1;
  interrupts();
  /*メカナムホイールに関するPID制御*/
  Recieve_cruizcore_data();                                               //ジャイロセンサcruizcoreの値を取得する  
  if(Timer2 >= 20 && arm_operation_mode != 'Throw')                       //20msごとに{}内実行
  {
    Encoder_Velocity();                                                   //エンコーダの20msごとの変化率(エンコーダ値の差)を計算
    Pid_Calculator_Velocity();                                            //目標速度に各モータ速度を近づける    
    if( FIELD_COLOR == 'White') Pid_Calculator_Distance();                  //auto_rake_x,yの計算    
    if((
      //FIELD_COLOR == 'White' || 
      FIELD_COLOR == 'Green'))                  //回転指示がないとき回転補正をかける
    {
      //rake_cruizcore = 0;
      rake_cruizcore = pid_cruizcore (cruizcore_standard_deg, cruizcore_degree); 
    }
    Timer2 = 1;                                                           //Timer2 リセット    
  }
  else
  {
    Timer2 ++;                                                             //1msごとに1ずつ増える  
  }
}
