
/* File  manual.ｃ
 * @author Niwa Yasuharu
 * latest renew　2018.1.20
 */


/*プロトタイプ宣言*/
//====================================================================================================
void Accelarate1(double rake);
void Accelarate2(double rake);
void Speed_Slow(void);
void Vertical_Velocity(double y);                                 //縦方向の速度解析
void Holizontal_Velocity(double x);                               //横方向の速度解析
void Rolling_Velocity (int rolling_direction);                    //回転方向速度解析
void Synthesis_Velocity(double moving_rake, double rolling_rake); //回転方向・縦方向・横方向の合成速度算出 引数:アナログスティックの傾き
/*速度計算に使用する配列群*/
double accel1 = 1.0;
double accel2 = 1.0;
double first_speed[4]        = {25.0,25.0,25.0,25.0}; //初期設定速度の最大値
double holizontal_speed[4]   = {0.0, 0.0, 0.0, 0.0};      //x軸方向速度
double vertical_speed[4]     = {0.0, 0.0, 0.0, 0.0};      //y軸方向速度
double rolling_speed[4]      = {0.0, 0.0, 0.0, 0.0};      //回転方向速度
double anti_rolling_speed[4] = {0.0, 0.0, 0.0, 0.0};      //直進補正速度
typedef struct Coordinate{ //座標に関する構造体を宣言 (英語辞典 Coordinate:座標)
  double x = 0.0;
  double y = 0.0;
}Coordinate;               //構造体の宣言終了
Coordinate dimension;      //Cordinate型の構造体dimensionを宣言する
//====================================================================================================





/*       【前】
 *   _____________　　　　　　　　　　　　　　　　　　　　　　　　　　　　　
 * 
 * 　　　  _____
 * 　+　　❘　　❘   -
 *  /0/----------\3\　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　
 * - |            | +      接地エンコーダ　　　　　　　　　　　　
 *   |            |         ｘ(holizontal: 水平)
 * - | x       y  | +         ｙ(vertical: 垂直)　
 *  \1\-----------/2/      
 *   +          -   
 *  
 * 手動操作時に実行する関数
 * PS4コントローラのアナログスティックの傾斜角度と傾斜具合によってメカナムホイールで動く機体を360度制御する。
 * 操作方法は左アナログスティックで360度全方位水平移動、右アナログスティックで回転｛同時に押すと曲がったりできる)
 * void loop() 内で実行
 * [引数]：移動角, 直進速度ベクトルの大きさ, 回転方向(左右), 回転方向速度ベクトルの大きさ */
void Mecanum_Operation (double moving_degree, double moving_rake, int rolling_direction, double rolling_rake)
{
  if(moving_rake > 0) {
    dimension.x = cos(Radian(moving_degree));     //①単位ベクトルを移動角にたいして cos() へ分解
    dimension.y = sin(Radian(moving_degree  ));     //②単位ベクトルを移動角にたいして sin() へ分解
  } else {
    dimension.x = 0; //0ベクトルにする
    dimension.y = 0;
  }  
  Accelarate1(moving_rake);                       //③発進時の速度を滑らかにする
  Accelarate2(rolling_rake);                      //④回転開始時の速度を滑らかにする    
  Speed_Slow();
  Holizontal_Velocity(dimension.x);               //⑤横方向速度解析 引数:x成分
  Vertical_Velocity(dimension.y);                 //⑥縦方向速度解析 引数:y成分
  Rolling_Velocity(rolling_direction);            //⑦回転方向速度解析
  Synthesis_Velocity(moving_rake, rolling_rake);  //⑧回転方向・縦方向・横方向の合成速度算出 引数:アナログスティックの傾き
 /* ⑨タイマ割り込み関数の中でPID_Velocity()を実行して最終的な出力値決定し
  * ⑩loop()関数の先頭でSend_Motordataを行いモータスレーブへ出力信号を送りモータを回す
  * ⑪ Serial.println("  (x,y)=(" + (String)dimension.x + ", " + (String)dimension.y + ")"); //ベクトル座標 */

 /*Serial.print(target_speed[0]    );
 Serial.print("         ");
 Serial.print(target_speed[1]    );
 Serial.print("         ");
 Serial.print(target_speed[2]);
 Serial.print("         ");
 Serial.print(holizontal_speed[1]);
 Serial.print("         ");
 Serial.println(vertical_speed[1]);*/
}





//(cruizcore_standard_deg != cruizcore_first_deg) || 

void Speed_Slow (void) {
  if(PS4.getButtonPress(L1))
  {
    if((cruizcore_standard_deg != cruizcore_first_deg) || (abs(cruizcore_standard_deg - cruizcore_degree) > 10)) {
        for(int i=0;i<4;i++)first_speed[i] = 10;
    }else for(int i=0;i<4;i++)first_speed[i] = 25;
  }
  if(abs(cruizcore_standard_deg - cruizcore_degree) > 10) {
        for(int i=0;i<4;i++)first_speed[i] = 10;
  }else for(int i=0;i<4;i++)first_speed[i] = 25;
}





/*その他
/*縦横方向の加速関数*/
void Accelarate1(double rake){
  if      (rake >= 0.1 && rake   <  0.4)    accel1  =  0.2;
  else if (rake >= 0.4 && rake   <  0.8)    accel1  =  0.1;
  else if (rake >= 0.8 && accel1 <= 1.0)    accel1 +=  0.01;
}

/*回転方向の加速関数*/
void Accelarate2(double rake){
  if      (rake < 0.5)                      accel2 =  0.5;
  else if (rake >=0.8 && accel2 <= 1.0)     accel2 += 0.01;
}

/*左スティック制御*/
/*水平(x軸)方向の速度成分の解析 (機体：右正, タイヤ：右正)*/
void Holizontal_Velocity(double x){
//  if(PS4.getButtonPress(L1))
//  {
    holizontal_speed[0] = -first_speed[0] * x;
    holizontal_speed[1] = -first_speed[1] * x;
    holizontal_speed[2] = first_speed[2] * x ;
    holizontal_speed[3] = first_speed[3] * x;
//  }
//  else
//  {
//    holizontal_speed[0] = -first_speed[0] * x*2.5 ;
//    holizontal_speed[1] = -first_speed[1] * x*2.5 ;
//    holizontal_speed[2] = first_speed[2] * x*2.5 ;
//    holizontal_speed[3] = first_speed[3] * x*2.5;
//  }
//  holizontal_speed[0] = first_speed[0] * x ;
//  holizontal_speed[1] = first_speed[1] * x ;
//  holizontal_speed[2] = first_speed[2] * x ;
//  holizontal_speed[3] = first_speed[3] * x ;
}



/*縦方向の速度成分の解析 (機体:上正, タイヤ：右正)*/
void Vertical_Velocity(double y){
//  if(PS4.getButtonPress(L1))
//  {
    vertical_speed[0] = -first_speed[0] * y ;          
    vertical_speed[1] = first_speed[1] * y;
    vertical_speed[2] = first_speed[2] * y; 
    vertical_speed[3] =  -first_speed[3] * y;
//  }
//  else
//  {
//    vertical_speed[0] = -first_speed[0] * y *2.5;          
//    vertical_speed[1] =  first_speed[1] * y *2.5;
//    vertical_speed[2] = first_speed[2] * y *2.5; 
//    vertical_speed[3] =  -first_speed[3] * y *2.5;
//  } 
}


/*右スティック制御 (12/30 点検OK)
 *(左右旋回を制御する直進移動中にも押すことができるがその場合は曲がっていく)*/
void Rolling_Velocity (int rolling_direction)
{
  static long tmp = 0;
  /*⬛⬛⬛ 機体水平移動 ⬛⬛⬛*/
  if(rolling_direction == 'None'){
      rolling_speed[0] = -first_speed[0];  rolling_speed[1] = first_speed[1];
      rolling_speed[2] = -first_speed[2];  rolling_speed[3] = first_speed[3];
      for(int i=0 ; i<4 ; i++)
      {
        rolling_speed[i] = 0;
      }
      right.rake = 0;
  }  
  /*⬛⬛⬛ 機体旋回移動 ⬛⬛⬛*/
  else {
    tmp = millis();
    if (rolling_direction == 'Right') { //右旋回
      //Serial.println("rolling right !");
      rolling_speed[0] = -first_speed[0];  
      rolling_speed[1] = +first_speed[1];
      rolling_speed[2] = -first_speed[2];
      rolling_speed[3] = +first_speed[3];  
    }
    else if (rolling_direction == 'Left') { //左旋回
      //Serial.println("rolling left !");
      rolling_speed[0] = +first_speed[0]; 
      rolling_speed[1] = -first_speed[1];
      rolling_speed[2] = +first_speed[2]; 
      rolling_speed[3] = -first_speed[3];      
    }
  }
  if((millis()-tmp) < 1000){
    cruizcore_standard_deg = cruizcore_degree; //基準角度の上書き    
    rake_cruizcore = 0;
  }
}



/* 左右回転方向を決定する関数 (12/30 点検OK)
 * 手動モードの時 void loop 内 のMecanum_Operetion()内で引数の１つとして実行 */
int RollingDirection(void) {
  int rolling_direction = 'None'; //初期化
  if(right.rake > 0) {
    if      ((right.degree>=0 && right.degree<=30) || (right.degree>=330 && right.degree<=360)) rolling_direction = 'Right';
    else if (right.degree>=150 && right.degree<=210) rolling_direction = 'Left';
  }else rolling_direction = 'None';
//  Serial.println(rolling_direction);
  return rolling_direction; //値を返す
}



/* 合成速度算出関数
 * 縦横・回転方向の速度を合成して最終目標速度を決定 */
void Synthesis_Velocity(double moving_rake, double rolling_rake) {
  for(int i=0;i<4;i++) {
    double sign = 0; //縦横方向に関する速度の正負 ベクトルが度の象限を向いているかを記録しないと、target_speed[] を計算するときに2乗する関係で全部速度が正方向に固定されてしまい思ったように動かない
   // if      ((vertical_speed[i] + holizontal_speed[i]) > 0) sign =  1.0;
    //else if ((vertical_speed[i] + holizontal_speed[i]) < 0) sign = -1.0;
    //target_speed[i] =   (sign * ((sqrt(pow(vertical_speed[i],2.0) + pow(holizontal_speed[i],2.0))) * moving_rake * accel1))  +  (rolling_speed[i] * rolling_rake * accel2 / 2.0)  + (rolling_speed[i] * rake_cruizcore);
    target_speed[i] =   (vertical_speed[i]+holizontal_speed[i])* moving_rake  +  (rolling_speed[i] * rolling_rake / 2.0)  + (rolling_speed[i] * rake_cruizcore);
    
  }
}
