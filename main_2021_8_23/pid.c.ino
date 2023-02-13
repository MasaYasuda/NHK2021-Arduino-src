/** 
 * File  PID.ｃ
 * @author Niwa Yasuharu
 * latest renew　2018.12.26
 */
 
#define DELTA_T  0.020 //処理周期[s] 20ms




/*≪速度型PID制御≫*/
/*速度型PID関数 [引数]モータ番号 観測量 目標量　[返り値]単位時間あたりの加速度*/
double pid_velocity(int number, double target_speed, double sensor_speed) {
  //メカナムホイール　            【0番】   【1番】    【2番】　 【3番】
  double Kp[4]              = {0.50000, 0.50000,  0.50000, 0.50000};      //P制御定数 比例ゲイン Kp
  double Ki[4]              = {0.00030, 0.00030,  0.00030, 0.00030};      //I制御定数 積分ゲイン Ki
  double Kd[4]              = {0.01500, 0.01500,  0.01500, 0.01500};      //D制御定数 微分ゲイン Kd
  static double diff[4][2]  = {{0,0},{0,0},{0,0},{0,0}};                  //現在偏差　過去偏差の保存場所を各モータに対して1つずつ用意
  static double integral[4] = {0,0,0,0};                                  //偏差累積値                                                        ＋偏差
  double Ap=0.0,Ai=0.0,Ad=0.0;                                            //比例制御量Ap　積分制御量Ai　微分制御量Ad                            |
  double dvdt = 0.0;                                                      //操作量に積分する加速度                                             |  /|↘
  diff[number][0]   = diff[number][1];                                    //前回偏差を保存しておく                                          　 |/| | |↘
  diff[number][1]   = (target_speed - sensor_speed);                      //偏差を計算 (偏差) = {(目標値)－(観測値)}                           | | | | |↘______
  integral[number] += (diff[number][1] + diff[number][0])/2.0 * DELTA_T;  // (20msごとの台形の面積) = {(今偏差)＋(過去偏差)}÷2 * 0.020 を積分　|_|_|_|_|_|_|_|_|↘___________＋時間 　※【イメージ図】一個一個の四角形(台形)の面積を積分
  integral[number]  = constrain(integral[number], -1000,1000);            //上限下限を設定しないと積分値が大きくなりすぎる
  Ap = Kp[number] * diff[number][1];                                      //P比例制御要素 (定数 Kp) * (偏差)　 ※ただし (偏差) = {(目標値)－(観測値)}
  Ai = Ki[number] * integral[number];                                     //I積分制御要素 (定数 Ki) * (偏差の積分(面積の和))
  Ad = Kd[number] * (diff[number][1] - diff[number][0]) / DELTA_T;        //D微分制御要素 (定数 Kd) * (偏差の偏差)/(処理時間)
  dvdt = (Ap+Ai+Ad);                                                      //0.006msあたりの加速度を計算
  return constrain(dvdt,-30.0, 30.0);                                     //速度に加算する加速度(double型)を返す。-100～100に制限しておく
}




/*≪距離型PID制御≫*/
/*0番はジャイロセンサcruizcore用1,2,3番は接地エンコーダ用*/
double pid_distance(int number, long target_value, long sensor_value) {//numberはモータ番号
  //                    【gyro】 【接地エンコーダX,Y】
  double KpS[3]       = {0.9000,  0.50,  0.50};  //P制御定数 比例ゲイン KpS
  double KiS[3]       = {0.0500,  0.001, 0.001};  //I制御定数 積分ゲイン KiS
  double KdS[3]       = {0.0005,  0.10,  0.10};  //D制御定数 微分ゲイン KdS
  long   diffS[3][2]  = {{0,0},{0,0},{0,0}};    //二次元配列
  long   integralS[3] = {0,0,0};                //積分値  
  double Sp=0.0, Si=0.0, Sd=0.0;                //比例制御量Vp 積分制御量Vi 微分制御量Vd
  double dxdt = 0.0;                            //操作量に積分する速度q
  diffS[number][0]   = diffS[number][1];
  diffS[number][1]   = (target_value - sensor_value);
  integralS[number] += (diffS[number][1] + diffS[number][0])/2.0 * DELTA_T;
  integralS[number]  = constrain(integralS[number], -1, 1);
  Sp = KpS[number] * diffS[number][1];
  Si = KiS[number] * integralS[number];
  Sd = KdS[number] * (diffS[number][1] - diffS[number][0]) / DELTA_T;   
  dxdt = (double)((Sp+Si+Sd) / 300);  //0～1.0の間で返す
  
  //dxdt = (double)(Sp+Si+Sd);
  return dxdt;
}




/*≪距離型PID制御cruizcore用≫*/
/*0番はジャイロセンサcruizcore用1,2,3番は接地エンコーダ用*/
double pid_cruizcore(double target_value, double sensor_value) {
  double Kp = 0.7000;            //P制御定数 比例ゲイン KpS 0.5000
  double Ki = 0.1000;            //I制御定数 積分ゲイン KiS 0.0010
  double Kd = 0.0000;            //D制御定数 微分ゲイン KdS
//  double cruizcore_diff[2] = {0.0, 0.0};   //二次元配列
//  double cruizcore_integral = 0.0;         //積分値
  double Sp=0.0, Si=0.0, Sd=0.0; //比例制御量Vp 積分制御量Vi 微分制御量Vd
  double dxdt = 0.0;             //操作量に積分する速度
//  static double v_dxdt = 0.0,sum_vect = 0,d_vect = 0;
//  static double pre_dxdt,dev_dxdt,v=0,root_v=0,vect = 0;
//  static int n = 0;
//  static double v_integral = 0.0;
  cruizcore_diff[0]   = cruizcore_diff[1];
  cruizcore_diff[1]   = (target_value - sensor_value);
  cruizcore_integral += ((cruizcore_diff[1] + cruizcore_diff[0]) / 2.0) * DELTA_T;
  cruizcore_integral  = constrain(cruizcore_integral, -1000, 1000); 
//  v_integral += ((diff[1] + diff[0]) / 2.0) * DELTA_T; 
//  Sp = Kp * diff[1];
//  Si = Ki * v_integral;

//  pre_dxdt = v_dxdt;
//  v_dxdt = (double)(Sp+Si+Sd);
//  dev_dxdt = -pre_dxdt + v_dxdt;

  Sp = Kp * cruizcore_diff[1];
  Si = Ki * cruizcore_integral;
  Sd = Kd * (cruizcore_diff[1] - cruizcore_diff[0]) / DELTA_T;
  dxdt = (double)((Sp+Si+Sd) / 40.0);  //-1.0～1.0の間で返す 45
  dxdt = constrain(dxdt, -1.0, 1.0);
  
  //ベクトル的な考え方で単位ベクトルにしてみた
//  v = pow(dev_dxdt,2)+pow(0.02,2);
//  if(0 <= v && v <= 0.016)
//  {
//    root_v = 0;
//    vect = 0;
//  }
//  else
//  {
//    root_v = sqrt(v);
//    vect = (dev_dxdt+0.02)/root_v;
//  }
//  vect = constrain(vect,-1.0,1.0);
//  if(n == 5)
//  {
//    n = 0;
//    d_vect = sum_vect / 5.0;
//    sum_vect = 0;
//  }
//  else
//  {
//    sum_vect += vect;
//    n++;
//  }

//  Serial.print(Sp);
//  Serial.print(",");
//  Serial.print(cruizcore_integral);
//  Serial.print(",");
//  Serial.print(Si);
//  Serial.print(",");
//  Serial.print(dxdt);
//  Serial.print(",");
//  Serial.print(cruizcore_diff[0]);
//  Serial.print(",");
//  Serial.print(v_dxdt);
//  Serial.print(",");
//  Serial.print(pre_dxdt);
//  Serial.print(",");
//  Serial.print(dev_dxdt);
//  Serial.print(",");
//  Serial.print(v);
//  Serial.print(",");
//  Serial.print(root_v);
//  Serial.print(",");
//  Serial.print(vect);
//  Serial.print(",");
//  Serial.print(cruizcore_diff[1]);
//  Serial.print(",");
//  Serial.print(dxdt);
//  Serial.println("");
  
  return dxdt;
  //return constrain(vect,-1.0,1.0);
}











/*PID制御の計算*/
void Pid_Calculator_Velocity(void) {
  /*各タイヤの速度PID*/
  double dvdt[4] = {0,0,0,0};
  for(int i=0; i<4; i++) {    
    dvdt[i] = pid_velocity(i, target_speed[i], sensor_speed[i]);  //PID制御（通常出力モード）
    Vout[i] = Vout[i] + dvdt[i];
    if(PS4.getButtonPress(L1))
    {
      Vout[i] = constrain(Vout[i],-30,+30);  //リミッター設定 暴走したとき用
    }
    else
    {
      Vout[i] = constrain(Vout[i],-60,+60);
    }
    if(target_speed[i] == 0) Vout[i] = 0;
    if     (Vout[i] > 0.0) Mode[i] = 1;    //前回転
    else if(Vout[i] < 0.0) Mode[i] = 2;    //後回転
    else if(Vout[i] == 0.0) {
      if   (abs(sensor_speed[0]) <= 100
         && abs(sensor_speed[1]) <= 100
         && abs(sensor_speed[2]) <= 100
         && abs(sensor_speed[3]) <= 100){
           Mode[0] = 0;
           Mode[1] = 0;
           Mode[2] = 0;
           Mode[3] = 0;
           Serial2.flush(); //今までのデータバッファを刷新する
           Send_Motordata();
      }else  Mode[i] = 4;
    }else    Mode[i] = 0;    //フリー回転（例外
  }
//  if(abs(Vout[0]) < 15)
//  {
//    Vout[0] = Vout[0];
//    Vout[1] = Vout[1];
//    Vout[2] = Vout[2];
//    Vout[3] = Vout[3];
//  }
//  else if(abs(Vout[0]) < 30)
//  {
//    Vout[0] = Vout[0]*1.1;
//    Vout[1] = Vout[1];
//    Vout[2] = Vout[2];
//    Vout[3] = Vout[3];
//  }
//  else if(abs(Vout[0]) < 50)
//  {
//    Vout[0] = Vout[0]*1.3;
//    Vout[1] = Vout[1]*0.8;
//    Vout[2] = Vout[2]*1.1;
//    Vout[3] = Vout[3]*0.8;
//  }
//  else
//  {
//    Vout[0] = Vout[0]*1.4;
//    Vout[1] = Vout[1]*0.7;
//    Vout[2] = Vout[2]*1.1;
//    Vout[3] = Vout[3]*0.7;
//  }
    
//    Vout[0] = constrain(Vout[0],-70,70);
//    Vout[1] = constrain(Vout[0],-60,60);
//    Vout[2] = constrain(Vout[0],-70,70);
//    Vout[3] = constrain(Vout[0],-60,60);
}



/*自動モードのとき接地エンコーダの距離型PID*/
void Pid_Calculator_Distance() {
  auto_rake_x  = pid_distance(1, target_x, Encoder[4]);  //x方向に関する距離型PID
  auto_rake_y  = pid_distance(2, target_y, Encoder[5]);  //y方向に関する距離型PID
}
