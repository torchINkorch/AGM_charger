/*
******************FUNCTIONS TO CTRL SHIFT REGISTER 74HC595BT*********************

*/
void stop_charging()
{
  digitalWrite(rel_max, LOW);
  digitalWrite(rel_mid, LOW);
  digitalWrite(rel_low, LOW);
  stop_flag_indicator();
  CSR = false;
  stop_flag = true;

}

/////////////////////////////////////////////////////////////////
void first_stage()
{
  digitalWrite(rel_max, HIGH);
  digitalWrite(rel_mid, LOW);
  digitalWrite(rel_low, LOW);
  stage1_indicator();
}
/////////////////////////////////////////////////////////////////

void second_stage()
{
  digitalWrite(rel_max, LOW);
  digitalWrite(rel_mid, HIGH);
  digitalWrite(rel_low, LOW);
  stage2_indicator();
  flag_stage_2 = true;

}
/////////////////////////////////////////////////////////////////

void third_stage()
{
  digitalWrite(rel_max, LOW);
  digitalWrite(rel_mid, LOW);
  digitalWrite(rel_low, HIGH);
  stage3_indicator();
  flag_stage_3 = true;

}
/////////////////////////////////////////////////////////////////

//******************OLED DISPLAY FUNCTIONS***************************************


void bat_value()
{
      if ( SOC_voltage < 11.63 ) CHARGE = 0;
      if ( (SOC_voltage >= 11.63) && (SOC_voltage < 11.70) ) CHARGE = 10;
      if ( (SOC_voltage >= 11.70) && (SOC_voltage < 11.81) ) CHARGE = 20;
      if ( (SOC_voltage >= 11.81) && (SOC_voltage < 11.96) ) CHARGE = 30;
      if ( (SOC_voltage >= 11.96) && (SOC_voltage < 12.11) ) CHARGE = 40;
      if ( (SOC_voltage >= 12.23) && (SOC_voltage < 12.41) ) CHARGE = 50;
      if ( (SOC_voltage >= 12.41) && (SOC_voltage < 12.51) ) CHARGE = 60;
      if ( (SOC_voltage >= 12.51) && (SOC_voltage < 12.65) ) CHARGE = 70;
      if ( (SOC_voltage >= 12.65) && (SOC_voltage < 12.78) ) CHARGE = 80;
      if ( (SOC_voltage >= 12.78) && (SOC_voltage < 12.89) ) CHARGE = 90;
      if ( SOC_voltage >= 12.89 ) CHARGE = 100;
      display.fillRect(0, 15, 100, 30, BLACK);
      display.setTextSize(3);
      display.setCursor(70, 20);
      display.println("%");
      display.setCursor(10, 20);
      display.println(CHARGE);
}
void calc_CCV_voltage()
{
  int b0 = analogRead(analogPin); // CCV voltage variable
  delay(200);
  int b1 = analogRead(analogPin);  // read current battery voltage @ CCV
  float w0 = 8.19 * b0 * (3.3 / 1023); // convert reading to voltage....x8, 
  float w1 = 8.19 * b1 * (3.3 / 1023);  
  CCV_voltage = ( w0 + w1 ) / 2; // read voltage twice and calculate M value
}
void calc_SOC_voltage()
{
    int a0 = analogRead(analogPin);  // read current battery voltage @ SOC0
    delay(200);
    int a1 = analogRead(analogPin);  // read current battery voltage @ SOC1
    float v0 = 8.19 * a0 * (3.3 / 1023); // convert reading to voltage....x8, SOC =source from open circuit 
    float v1 = 8.19 * a1 * (3.3 / 1023);  // SOC =source from open circuit 
    SOC_voltage = ( v0 + v1 ) / 2; // read voltage twice and calculate M value
}

