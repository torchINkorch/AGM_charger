#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#define analogPin A0  // voltage read pin
#define OLED_WIDTH 128
#define OLED_HEIGHT 64
Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire, -1);
const unsigned long eventInterval = 1000;// delay for voltage reading & calc
  unsigned long previousTime = 0;
const unsigned long eventInterval_4 = 5000;
  unsigned long previousTime_4 = 0;
const unsigned long eventInterval_5 = 7130;
  unsigned long previousTime_5 = 0;
unsigned char CHARGE;
unsigned char CCV_CHARGE;
float SOC_voltage;
float CCV_voltage;
const int rel_max = D8;
const int rel_mid = D7;
const int rel_low = D6;
const int ac_in = D5;
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
    stop_flag = false; 
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

