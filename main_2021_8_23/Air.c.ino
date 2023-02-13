
/** 
 * File  Air.ｃ
 * @author Niwa Yasuharu
 * latest renew　2018.12.29
 */
 
 /*エアシリンダのピンをHIGH,LOWする関数*/
void Air_pinMode(int air, int mode){
  if      (mode == 'HIGH') digitalWrite(air_pin[air], HIGH);
  else if (mode ==  'LOW') digitalWrite(air_pin[air], LOW );
}
