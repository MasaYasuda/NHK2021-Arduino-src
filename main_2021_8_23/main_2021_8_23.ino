/***********更新記録***********/
//基板の変更に伴い、エンコーダ、ポテンションメータの部分の調整。


/* File     MR1_main.ｃ
 * @author  Niwa Yasuharu, Takagi
 * Gmail:   e.niwayakkun@gmail.com
 * latest   renew　2018.12.30
 * ★【注意】MEGA2560の10,11,12,13番ピンは UsbHostShield2.0 に用いるため、他の用途では使用することができない ( ← 間違えてサーボに使ってしまったけどどちらとも動いた !)
 */



//=========================================================================================================
/*ヘッダファイルのインクルード*/

#include <math.h>                                                       //算術用ヘッダファイルsin() cos() sqrt() pow()等が使用可能に
#include <MsTimer2.h>                                                  //タイマ2割り込みヘッダファイル
#define Pi 3.14159265359                                                //パイの定義
double Radian(double deg) {double rad = deg * (Pi/180.0); return rad;}  //度数を弧度法に変換
double Degree(double rad) {double deg = rad / (Pi/180.0); return deg;}  //弧度法を度数に変換

/*以下 PS4 contoroller, USBhosstShield2.0 関連のヘッダファイルのインクルード*/
#include <PS4BT.h>
#include <usbhub.h>      
#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif
#include <SPI.h>
/*PS4コントローラのブルートゥース接続の設定*/
USB Usb;            //USB型のインスタンスを生成
BTD Btd(&Usb);      //Btd型のインスタンスを生成
PS4BT PS4(&Btd);    //PS4BT型のインスタンスを生成
//★新品や別のPS4コントローラを使用する場合はペアリングを行うこと!!! (PS4コントローラをゲーム機に接続した場合でもペアリングは必須)
//PS4BT PS4(&Btd, PAIR); //PS4コントローラがペアリングされていない場合はこちらのコメントアウトを外し「PS4(&BTd);」をコメントアウトする SHARE+PS4ボタン長押し*/
//==========================================================================================================







/*プロトタイプ宣言・グローバル変数の宣言*/
//==========================================================================================================
/*■■■[メカナムホイール]■■■*/
int  rolling_mode = 'None'; //'None''Left''Right'から選択
int  RollingDirection(void);                                                                              //回転方向をコントローラの右アナログスティックの角度で決定【返り値'None''Left''Right'】
void cruizcore_Compensation(double standard_deg);
void Mecanum_Operation(double moving_degree, double moving_rake, int rolling_direction, double rolling_rake); //【引数 ①移動角度・方向(0～360度)　②速度強(0～1) ③回転方向 ④回転速度強(0～1)】
/*■■■[モータースレーブ]■■■*/
void   Send_Motordata(void);        //モータのデータをモータスレーブへ送信する
byte   Number[9]  = {0,1,2,3,4,5,6,7,8};  //送信モータ番号（各モータスレーブと値を一致させること）
byte   Mode[9]    = {3,3,3,3,3,3,3,3,3};  //送信回転モード（フリ-回転0 前回転1 後回転2 停止3 ブレーキ4）
int    Speed[9]   = {0,0,0,0,0,0,0,0,0};  //送信モータ速度(0～220まで)足回りは早くても150位を限度に
/*■■■[PID制御の関数群]■■■*/
void   timer2_interrupt(void);                                              //タイマ2割り込み関数 1msごとに{ }内を割り込み実行する
void   Pid_Calculator_Distance(void);                                       //距離型PIDの計算をまとめた関数
void   Pid_Calculator_Velocity(void);                                       //速度型PIDの計算をまとめた関数
double pid_distance(int number, long sensor_value, long target_value);      //目標距離にエンコーダ値を近づける「距離型PID制御関数」,返り値：速度強(MIN:0～1:MAX)
double pid_velocity(int number, double target_speed, double sensor_speed);  //目標速度にモータ速度を近づける「速度型PID制御関数」,返り値：加速度
double target_speed[4]     = {0.0, 0.0, 0.0, 0.0};                          //目標速度
double sensor_speed[4]     = {0.0, 0.0, 0.0, 0.0};                          //実測速度
double Vout[4] = {0,0,0,0};                                                 //操作量
/*■■■[PS4コントローラ]■■■*/
void Arduino_UsbHostShield2_Bluetooth_setup(void);  //UsbHostShield2.0とPS4の通信を開始する関数
void Arduino_UsbHostShield2_connect_PS4(void);      //UsbHostShield2.0とPS4の通信が確立している時、ボタンデータを管理する関数
typedef struct Analogdata{                          //構造体[Analogdata]を宣言開始
  double xHat   = 0.0;                              //x方向の傾きを「1～(-1)」で表示
  double yHat   = 0.0;                              //y方向の傾きを「1～(-1)」で表示
  double degree = 0.0;                              //アナログスティックの向き(度数0～360度)
  double rake   = 0.0;                              //アナログスティックの傾き(0～1.0)
}Analogdata;                                        //構造体を宣言終了
Analogdata left, right;                             //Analogdata型の変数「left」と「right」を宣言する
unsigned long int tms = 0;
/*■■■[エンコーダー]■■■*/
void Encoder_Setup(void);                 //エンコーダの値を読み取りを開始する関数
void Encoder_Velocity(void);              //エンコーダの20ms(0.020秒)ごとのエンコーダ変化率を示す（割り込み内で実行）
void Serial_Encoder_value(void);          //エンコーダの値をシリアルモニタに出力する
void Serial_Encoder_speed_value(void);    //エンコーダの変化率をシリアルモニタに出力する
volatile long Encoder[6] = {0,0,0,0,0,0}; //エンコーダの値を記録しておく変数
volatile long encoder_omg[4] = {0,0,0,0}; //エンコーダの値の角速度[rad/s]を記録しておく変数
/*■■■[エアシリンダー]■■■*/
void Air_pinMode(int air, int mode);      //エアシリンダのピンをHIGH,LOWする関数 引数
int  air_pin[5] = {47,45,42,49,48};       //エアシリンダのピン番号の設定 ,45がエアシリ基盤の4番ピンに対応。他は不明
/*■■■[ポテンショメータ]■■■*/
#define POT_A00_MAX   380            //ポテンショメータ(A8ピン接続) の最大値を定数として定義 (こういうのはC言語などでは大文字で書くことになっている。)
#define POT_A00_MIN   300            //ポテンショメータ(A8ピン接続) の最小値を定数として定義
#define POT_A04_MAX   900            //ポテンショメータ(A11ピン接続)の最大値を定数として定義
#define POT_A04_MIN   580            //ポテンショメータ(A11ピン接続)の最小値を定数として定義
void Pot_Setup(void);                //ポテンショメータの値を下の配列に記録する関数
void Serial_Pot_value (void);        //ポテンショメータの値をシリアルモニタに出力する関数
int  pot_pin[2] = {A0,A4};    //ポテンショメータのピンの設定
int  pot_val[2] = {0,0};         //ポテンショメータの値を保存する配列
int first_pot_val = 0;
int set_pot_val = 0;
int std_pot_val = 0;
int  sgn[2]= {0,0};
/*■■■[cruizcore]■■■*/
double pid_cruizcore(double target_value, double sensor_value);
double cruizcore_degree        = 0.0;
double cruizcore_first_deg     = 0.0;
double cruizcore_standard_deg  = 0.0;
double rake_cruizcore          = 0.0;
#define KICK_DEG                 23
double cruizcore_diff[2] = {0.0, 0.0};   //二次元配列
double cruizcore_integral = 0.0;         //積分値
/*■■■[シャガイ投射]■■■*/
void Catapult_Operation (void);     //投射角度を変える関数
void Shooting_Operation(int mode);  //アームの動作を司る関数
int arm_operation_mode = 'Close';   //アームのモードを指定する変数
int catapapult_mode    = 'Stop' ;   //カタパルトのモードを指定する変数 'Reset' 'Shoot' から選択
/*■■■[自動関連]■■■*/
long target_x = 0;                //x座標エンコーダの目標値
long target_y = 0;                //y座標エンコーダの目標値
void D2 (long x, long y){
  target_x = x;
  target_y = y;
}
double auto_degree = 0;
double auto_rake   = 0;
double auto_rake_x = 0.0;
double auto_rake_y = 0.0;

double atl_degree;
double atl_rake;
int atRD;
double atr_rake;
/*■■■[シリアルモニタ]■■■*/
void Serial_Monitor(void); //この関数の{}内でSerial.print()を”まとめて”実行推奨 メイン文がスッキリする
int FIELD_COLOR = 'Select';
void  Recieve_cruizcore_data(void);
//==========================================================================================================






void setup(void)
{
  Serial.begin(115200);                              //0番通信 PCとシリアル通信開始(PS4コントローラ使用のため速度115200固定)
  Serial2.begin(115200);                             //2番シリアル通信(TX2)     「モータスレーブ」と8bit通信 モータ番号・速度・状態の3つをビット演算を用いて送信　【1番通信はエンコーダに使用している為使えない】
  Serial3.begin(115200);                             //3番シリアル通信(TX3,Rx3) 「センサスレーブ」と8bit通信 cruizcore とカラーセンサの値を手得する
  MsTimer2::set(1,timer2_interrupt);                 //タイマー2割り込みの設定 1msごとに「timer2_interrupt(){}」を割り込み実行  ※「Timer2_Interrupt.c」にある
  MsTimer2::start();                                 //タイマー2割り込み開始
  Arduino_UsbHostShield2_Bluetooth_setup();           //Aruduino_MEGA2560とUsbHostShield2.0を接続する
  Encoder_Setup();                                    //エンコーダの割り込みを開始
  Pot_Setup();                                        //ポテンショメータの値を下の配列に記録する関数
  sgn[0]=pot_val[0]; sgn[1]=pot_val[1]; //torobo2019：ポテンショメーターの最初の位置を保存。setupは一回しか読まない。
  for(int i=0; i<2; i++){pinMode(air_pin[i],OUTPUT);} //エアシリピン0,1番をすべて出力に設定
  for(int i=0; i<2; i++){Air_pinMode(i,'LOW');}       //すべてのエアシリンダをLOWに初期化  [code]
  cruizcore_standard_deg = cruizcore_degree;              //目標角を記録
  cruizcore_first_deg    = cruizcore_degree;              //基準角を記録
}







void loop (void)
{
  Arduino_UsbHostShield2_connect_PS4();  //PS4コントローラのアナログスティックの傾きと方向を読み取る
  Pot_Setup();                           //ポテンショメータの値を下の配列に記録する関数
  /*カタパルトの操作関連*/
//  if      (PS4.getButtonClick(SQUARE)) {catapapult_mode = 'Throw';} //カタパルト投射モード　　緑色にする
//  else if (PS4.getButtonClick(CROSS))  {catapapult_mode = 'Reset';} //カタパルトセットモード　黄色にする
//  Catapult_Operation ();                 //投射角度を変える関数

  Send_Motordata();                      //0,1,2,3番モータ出力値を各モータスレーブへ順番に送信する
  Serial_Monitor();                      //シリアルモニタへの出力全般

  switch(FIELD_COLOR) {
    
    case 'Select':                                                                                                                                                                        //『コート色選択 及び 制御モード選択』モード
      PS4.setLed(Yellow);                                                                                                                                                                //コート色を選択するまでコントローラのライトバーを黄色に      
      //if      (PS4.getButtonClick(SQUARE))  {FIELD_COLOR = 'White';   PS4.setLed(White);  cruizcore_standard_deg = cruizcore_degree; Encoder[4]=0; Encoder[5]=0; } //  PS4.setLedFlash(20, 20); }  //◎ 赤選択 PS4コントローラ ライトバーを赤色に 自動モード
      //else if (PS4.getButtonClick(CROSS))   {FIELD_COLOR = 'Blue';  PS4.setLed(Blue); cruizcore_standard_deg = cruizcore_degree; Encoder[4]=0; Encoder[5]=0;}                               //☓ 青選択 PS4コントローラ ライトバーを青色に 自動モード      
      if (PS4.getButtonClick(TRIANGLE)){FIELD_COLOR ='Green'; PS4.setLed(Green); cruizcore_standard_deg = cruizcore_degree; first_pot_val = std_pot_val; set_pot_val = first_pot_val + 40; cruizcore_diff[0] = 0; cruizcore_diff[1] = 0;cruizcore_integral = 0.0;}//△ 緑選択 PS4コントローラ ライトバーを緑色に 手動モード
      if (PS4.getButtonClick(SHARE)){cruizcore_first_deg = cruizcore_degree;} 
      for(int i = 0 ; i < 4 ; i++)
      {
        Mode[i]=0;Speed[i]=0;
      }
    break;                                                                                                              
    
    case 'Green':                                                                                   
       Mecanum_Operation (left.degree, left.rake, RollingDirection(), right.rake);  //メカナム手動制御
       if (PS4.getButtonClick(TRIANGLE)){FIELD_COLOR ='Select'; PS4.setLed(Yellow);}   
    break;
//    case 'White':
//      auto_degree =  Degree(atan2(auto_rake_y, auto_rake_x));                            //アークタンジェントで角度計算
//      //auto_rake   =  constrain(sqrt(pow(auto_rake_x,2)+pow(auto_rake_y,2)), 0.0, 1.0);   //x,yの値をもとに目標移動角に対する速度の傾き(大小)を算出する
//      Mecanum_Operation (auto_degree,left.rake , 'None',0);
//       //Mecanum_Operation (atl_degree, atl_rake, atRD, atr_rake);
//       if (PS4.getButtonClick(SQUARE)){FIELD_COLOR ='Select'; PS4.setLed(Yellow);}
  }  
}
