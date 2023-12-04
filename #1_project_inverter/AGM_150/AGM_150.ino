#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#define analogPin A0  // voltage read pin
#define OLED_WIDTH 128
#define OLED_HEIGHT 64
Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire, -1);
//const unsigned long eventInterval = 60;// delay for voltage reading & calc
//unsigned long previousTime = 0;
uint32_t freq = 1400;
unsigned char CCV_CHARGE;
unsigned char CHARGE;
static float SOC_voltage;
static float CCV_voltage;
const int rel_max = D8;
const int rel_mid = D6;
const int rel_EN = D7;
const int ac_in = D5;
const int charge_pumpPIN = D4;
bool stop_flag = false;     // charging disabled
bool stage_2_flag = false;
bool stage_1_flag = false;
bool AC = false;            // 230 vac
bool terminate = false;     // exit charging loop
void setup() {
  analogWriteFreq(freq);
  WiFi.mode(WIFI_OFF);
  Wire.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  pinMode(rel_max, OUTPUT);
  pinMode(rel_mid, OUTPUT);
  pinMode(rel_EN, OUTPUT);
  pinMode(ac_in, INPUT);  // pin input to indicate when AC mains are availible
  display.setTextColor(WHITE);
  display.display();
  display.clearDisplay();
  digitalWrite(rel_EN, LOW);
  analogWrite(charge_pumpPIN, 127);  //charge pump
}
void loop() {
  // unsigned long currentTime = millis();
  display.drawFastHLine(0, 13, 128, WHITE);
  display.display();
  VAC_detect();
  if (AC == true) MAINS_OPERATING_PROCEDURE();
  if (AC == false) BATT_OPERATING_PROCEDURE();
}














void VAC_detect() {
  if (digitalRead(ac_in) == HIGH) AC = true;
  else if (digitalRead(ac_in) == LOW) AC = false;
}



void MAINS_OPERATING_PROCEDURE() {
    digitalWrite(rel_EN, LOW);
    source_detector();
    calc_SOC_voltage();
    display_SOC_voltage();
    bat_value();
  if (SOC_voltage >= 13) {
    stop_flag = true;
    terminate = false;
    stop_charging();
    calc_SOC_voltage();
    display_SOC_voltage();
    bat_value();
    charged();
  } 
  else if ( SOC_voltage < 10 )stop_charging();
  else if ( (SOC_voltage > 10) && (SOC_voltage < 13) ) {
    stop_flag = false;
    stage_1_flag = false;
    stage_2_flag = false;
  if (stop_flag == false) {
      do {
        digitalWrite(rel_EN, HIGH);
        VAC_detect();
        calc_CCV_voltage();
        display_CCV_voltage();
        bat_value_CCV();
        charging();
        if (CCV_voltage <= 12.7) second_stage();                              // second stage charge @ min Amps
        if (CCV_voltage >= 12.8) first_stage();  // first stage reach charge 
        if (CCV_voltage >= 13.8) {
          stop_charging();  // stop charging by writing
          terminate = true;
        }
      } while (terminate == false && AC == true);
    }
  }
}



void BATT_OPERATING_PROCEDURE() {

  //reset AC flag
  digitalWrite(rel_EN, LOW);
  charged();
  source_detector();
  calc_SOC_voltage();
  display_SOC_voltage();
  bat_value();
  stage_2_flag = false;
  stage_1_flag = false;
  stop_flag = false;
  terminate = false;
}


void calc_CCV_voltage() {
  int b0 = analogRead(analogPin);  // CCV voltage variable
  CCV_voltage = 10 * b0 * (3.3 / 1023);
}

void calc_SOC_voltage() {
  int a0 = analogRead(analogPin);  // read current battery voltage @ SOC0
  SOC_voltage = 10 * a0 * (3.3 / 1023);
}

void stop_charging() {
  digitalWrite(rel_EN, LOW);
  digitalWrite(rel_max, LOW);
  digitalWrite(rel_mid, LOW);
  stop_flag_indicator();
  stop_flag = true;
  stage_1_flag = false;
  stage_2_flag = false;
}

/////////////////////////////////////////////////////////////////
void first_stage() {
  if ( stage_2_flag == true ){
  digitalWrite(rel_max, HIGH);
  digitalWrite(rel_mid, LOW);
  stage1_indicator();
  stage_1_flag = true;
  }
}
/////////////////////////////////////////////////////////////////

void second_stage() {
  if (stage_1_flag == false ){
  digitalWrite(rel_max, LOW);
  digitalWrite(rel_mid, HIGH);
  stage2_indicator();
  stage_2_flag = true;
  }
}
/////////////////////////////////////////////////////////////////


void bat_value_CCV() {
  if (CCV_voltage < 10.4) CCV_CHARGE = 0;
  else if ((CCV_voltage >= 10.4) && (CCV_voltage <= 10.9)) CCV_CHARGE = 10;
  else if ((CCV_voltage >= 11) && (CCV_voltage <= 11.4)) CCV_CHARGE = 20;
  else if ((CCV_voltage >= 11.5) && (CCV_voltage <= 11.9)) CCV_CHARGE = 30;
  else if ((CCV_voltage >= 11.5) && (CCV_voltage < 12)) CCV_CHARGE = 40;
  else if ((CCV_voltage >= 12) && (CCV_voltage < 12.4)) CCV_CHARGE = 50;
  else if ((CCV_voltage >= 12.4) && (CCV_voltage <= 12.6)) CCV_CHARGE = 60;
  else if ((CCV_voltage >= 12.7) && (CCV_voltage <= 13.1)) CCV_CHARGE = 70;
  else if ((CCV_voltage >= 13.2) && (CCV_voltage <= 13.5)) CCV_CHARGE = 80;
  else if ((CCV_voltage >= 13.6) && (CCV_voltage <= 14.1)) CCV_CHARGE = 90;
  else if (CCV_voltage >= 14.4) CCV_CHARGE = 100;
  display.fillRect(0, 15, 100, 30, BLACK);
  display.setTextSize(3);
  display.setCursor(70, 20);
  display.println("%");
  display.setCursor(10, 20);
  display.println(CCV_CHARGE);
}


void bat_value() {
  if (SOC_voltage < 10) CHARGE = 0;
  else if ((SOC_voltage >= 10.5) && (SOC_voltage <= 11.1)) CHARGE = 10;
  else if ((SOC_voltage >= 11.2) && (SOC_voltage <= 11.3)) CHARGE = 20;
  else if ((SOC_voltage >= 11.4) && (SOC_voltage <= 11.5)) CHARGE = 30;
  else if ((SOC_voltage >= 11.6) && (SOC_voltage <= 11.7)) CHARGE = 40;
  else if ((SOC_voltage >= 11.8) && (SOC_voltage <= 11.9)) CHARGE = 50;
  else if ((SOC_voltage >= 12) && (SOC_voltage <= 12.1)) CHARGE = 60;
  else if ((SOC_voltage >= 12.2) && (SOC_voltage <= 12.3)) CHARGE = 70;
  else if ((SOC_voltage >= 12.4) && (SOC_voltage <= 12.5)) CHARGE = 80;
  else if ((SOC_voltage >= 12.6) && (SOC_voltage <= 12.7)) CHARGE = 90;
  else if (SOC_voltage >= 12.8) CHARGE = 100;
  display.fillRect(0, 15, 100, 30, BLACK);
  display.setTextSize(3);
  display.setCursor(70, 20);
  display.println("%");
  display.setCursor(10, 20);
  display.println(CHARGE);
}


void display_SOC_voltage() {
  display.fillRect(64, 0, 40, 10, BLACK);
  display.setTextSize(1);
  display.setCursor(64, 0);
  display.println(SOC_voltage);
  display.setCursor(94, 0);
  display.println("v");
  display.display();
}

void display_CCV_voltage() {
  display.fillRect(64, 0, 40, 10, BLACK);
  display.setTextSize(1);
  display.setCursor(64, 0);
  display.println(CCV_voltage);
  display.setCursor(94, 0);
  display.println("ccv");
  display.display();
}
void stage1_indicator() {
  display.fillRect(100, 32, 28, 32, BLACK);
  display.setTextSize(1);
  display.setCursor(100, 32);
  display.println("max");
  display.display();
}
void stage2_indicator() {
  display.fillRect(100, 32, 28, 32, BLACK);
  display.setTextSize(1);
  display.setCursor(100, 32);
  display.println("mid");
  display.display();
}
void stage3_indicator() {
  display.fillRect(100, 32, 28, 32, BLACK);
  display.setTextSize(1);
  display.setCursor(100, 32);
  display.println("low");
  display.display();
}
void stop_flag_indicator() {
  display.fillRect(100, 32, 28, 32, BLACK);
  display.setTextSize(1);
  display.setCursor(100, 32);
  display.println("STF");
  display.display();
}

void charging() {
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

void charged() {
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

void source_detector() {
  display.setTextSize(1);
  display.setCursor(0, 50);
  display.println("Source:");
  display.display();
  if (AC == true) {
    display.fillRect(42, 50, 40, 14, BLACK);
    display.setTextSize(1);
    display.setCursor(42, 50);
    display.println("AC mains");
  }
  if (AC == false) {
    display.fillRect(42, 50, 40, 14, BLACK);
    display.setTextSize(1);
    display.setCursor(42, 50);
    display.println("Batt");
  }
  display.display();
}
