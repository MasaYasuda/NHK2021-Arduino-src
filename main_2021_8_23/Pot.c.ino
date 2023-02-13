
/* File Pot.ｃ
 * @author Niwa Yasuharu
 * latest renew　2019.1.4
 *int  pot_pin[2] = {A0,A4};  //ポテンショメータのピンの設定
 *                 ______
 * _______________/______
 * |⬛ 1       0 ⬛|       MAX 
 * |M4●pot0 A0   |        ⬆
 * |              |       MIN
 * |M5●pot1 A4  |
 * |⬛_2_______3_⬛|
 */


void Pot_Setup(void){
  static int pot_flag = 0; 
  static int pot_tmp = 0;
  for(int i=0; i<2; i++) pot_val[i] = analogRead(pot_pin[i]);
  pot_tmp += pot_val[1];
  pot_flag++;
  if(pot_flag == 20)
  {
    std_pot_val = pot_tmp / 20;
    pot_flag = 0;
    pot_tmp = 0;
  }
}

void Serial_Pot_value (void) {
  
  Serial.print("A0=" + (String)pot_val[0]);Serial.print("__");
  //Serial.println("A4=" + (String)pot_val[1]);
  Serial.println("A4=" + (String)std_pot_val);
  Serial.println(first_pot_val);
}
