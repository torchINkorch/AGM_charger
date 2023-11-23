#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <Arduino.h>
#define OLED_WIDTH 128
#define OLED_HEIGHT 64
Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire, -1);
const unsigned long eventInterval = 1000;// delay for voltage reading & calc
  unsigned long previousTime = 0;
const unsigned long eventInterval_4 = 5000;
  unsigned long previousTime_4 = 0;
unsigned char CHARGE;
unsigned char CCV_CHARGE;
float SOC_voltage;
float CCV_voltage;

const int rel_max = 10;
const int rel_mid = 11;
const int rel_low = 12;
const int rel_enable = 13;
const int fan_pwm = 14;
const int ac_in = 6;
const int charge_pumpPIN = 7;
const int adc_0 = 1;
const int adc_1 = 2;
const int adc_2 = 3;
const int adc_3 = 4;
const int charge_pumpPIN = D4;
bool flag_stage_1 = false;//bulk charge at max current 16-17A up to 80%
bool flag_stage_2 = false;//absorbtion charge 5-6Afrom 80% to 99%
bool flag_stage_3 = false;//topping charge 99%-100% at minimum current 
bool stop_flag = false;// charging disabled
bool AC = false;// 230 vac
bool CSR = false;// charge service routine

void setup() {
  WiFi.mode(WIFI_OFF);
  Wire.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  analogReference(DEFAULT);
  pinMode(rel_max, OUTPUT);
  pinMode(rel_mid, OUTPUT);
  pinMode(rel_low, OUTPUT);
  pinMode(ac_in, INPUT); // pin input to indicate when AC mains are availible
  pinMode(fan_pwm, OUTPUT);
  pinMode(adc_0, INPUT);  // voltage
  pinMode(adc_1, INPUT);  // current
  pinMode(adc_2, INPUT);  // thr1
  pinMode(adc_3, INPUT);  // thr2
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(fan_pwm, LOW);
  display.setTextColor(WHITE);
  display.display();
  display.clearDisplay();
 }
void loop()
{
  unsigned long currentTime = millis();
  display.drawFastHLine(0, 13, 128, WHITE);
 // display.drawFastVLine(98, 13, 51, WHITE);
  display.display();
  if ( digitalRead(ac_in) == HIGH ) AC = true;// 230vac availiable  run program when AC mains 230VAC are availiable
  source_detector();
  bat_value();
  if ( CSR == false )
  {
    if ( currentTime - previousTime >= eventInterval ) 
        {
          calc_SOC_voltage();
          display_SOC_voltage();
          previousTime = currentTime;
        }
  }
  if ( SOC_voltage >= 12.60 && AC == true)
      {
        stop_charging();
        charged();
        analogWrite(charge_pumpPIN, 0);//charge pump
      }
  if ( SOC_voltage < 12.60 )      // 12.89 = 100% bat
  {
    
    if ( stop_flag == false  &&  AC == true )
      {
        CSR = true;
        charging();
        analogWrite(charge_pumpPIN, 127);//charge pump
        bat_value();
            if ( currentTime - previousTime_4 >= eventInterval_4 ) 
                {
                  calc_CCV_voltage();
                  display_CCV_voltage();
                  previousTime_4 = currentTime;
                }
            if ( CCV_voltage < 12.40) first_stage();// first stage charge @ max Amps
            if ( CCV_voltage >= 12.40 && CCV_voltage <= 12.70 ) second_stage();// second stage charge of absorbtion
            if ( CCV_voltage > 12.70 && CCV_voltage < 14.30 ) third_stage();//topping charge up to float
            if ( CCV_voltage >= 14.30 ) stop_charging();// stop charging by writing 
       
      } 
  }   
  
/////*****************************************************************************************************************************
 
  if ( digitalRead(ac_in) == LOW )     // run this program when there are no AC mains 230VAC running on Bat
    {
      charged();
      bat_value();
      flag_stage_1 = false;//bulk charge at max current 16-17A up to 80%
      flag_stage_2 = false;//absorbtion charge 5-6Afrom 80% to 99%
      flag_stage_3 = false;//topping charge 99%-100% at minimum current 
      stop_flag = false;
      
  if ( currentTime - previousTime >= eventInterval ) 
        {
          calc_SOC_voltage();
          display_SOC_voltage();
          previousTime = currentTime;
        }
      AC = false;   //reset AC flag
      analogWrite(charge_pumpPIN, 0);//charge pump
      source_detector();
    }
    
    
}
void display_SOC_voltage()
{
  display.fillRect(64, 0, 40, 10, BLACK);
  display.setTextSize(1);
  display.setCursor(64, 0);
  display.println(SOC_voltage);
  display.setCursor(94, 0);
  display.println("v");
  display.display();
}
void display_CCV_voltage()
{
  display.fillRect(64, 0, 40, 10, BLACK);
  display.setTextSize(1);
  display.setCursor(64, 0);
  display.println(CCV_voltage);
  display.setCursor(94, 0);
  display.println("v");
  display.display();
}
void stage1_indicator()
{
  display.fillRect(100, 32, 28, 32, BLACK);
  display.setTextSize(1);
  display.setCursor(100, 32);
  display.println("st_1");
  display.display();
}
void stage2_indicator()
{
  display.fillRect(100, 32, 28, 32, BLACK);
  display.setTextSize(1);
  display.setCursor(100, 32);
  display.println("st_2");
  display.display();
}
void stage3_indicator()
{
  display.fillRect(100, 32, 28, 32, BLACK);
  display.setTextSize(1);
  display.setCursor(100, 32);
  display.println("st_3");
  display.display();
}
void stop_flag_indicator()
{
  display.fillRect(100, 32, 28, 32, BLACK);
  display.setTextSize(1);
  display.setCursor(100, 32);
  display.println("st_F");
  display.display();
}

void charging()
{     
      display.drawRoundRect(0, 0, 48, 10, 2, WHITE);
      display.display();
      display.fillRect(2, 2, 5, 6, WHITE);    
      display.display();
      delay(100);
        display.fillRect(9, 2, 5, 6, WHITE);
        display.display();
        delay(100);
          display.fillRect(16, 2, 5, 6, WHITE); 
          display.display();
          delay(100);
            display.fillRect(23, 2, 5, 6, WHITE);
            display.display();
            delay(100);
              display.fillRect(30, 2, 5, 6, WHITE);
              display.display();
              delay(100);
                display.fillRect(37, 2, 5, 6, WHITE);
                display.display();
                delay(100);
                  display.fillRect(44, 2, 5, 6, WHITE);
                  display.display();
                  delay(100);
                   display.fillRect(2, 2, 44, 6, BLACK);
                   display.display();
   
}
void charged()
{
       display.drawRoundRect(0, 0, 48, 10, 2, WHITE);
        display.fillRect(2, 2, 5, 6, WHITE);    
          display.fillRect(9, 2, 5, 6, WHITE);
            display.fillRect(16, 2, 5, 6, WHITE); 
              display.fillRect(23, 2, 5, 6, WHITE);
                display.fillRect(30, 2, 5, 6, WHITE);
                  display.fillRect(37, 2, 5, 6, WHITE);
                    display.fillRect(44, 2, 5, 6, WHITE);  
                      display.display();
}
void no_bat()
{
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.println("NO BATTERY");
  display.display();
}
void source_detector()
{
  display.setTextSize(1);
  display.setCursor(0, 50);
  display.println("Source:");
  display.display();
  if ( AC == true )
  {
    display.fillRect(42, 50, 40, 14, BLACK);
    display.setTextSize(1);
    display.setCursor(42, 50);
    display.println("AC mains");
  }
  if ( AC == false )
  {
    display.fillRect(42, 50, 40, 14, BLACK);
    display.setTextSize(1);
    display.setCursor(42, 50);
    display.println("Batt");
  }   
    display.display();
}

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
  int b0 = analogRead(adc_0); // CCV voltage variable
  CCV_voltage = 10 * b0 * (3.3 / 4095);  
}
void calc_SOC_voltage()
{
    int a0 = analogRead(adc_0);  // read current battery voltage @ SOC0
    SOC_voltage = 10 * a0 * (3.3 / 4095);
}


