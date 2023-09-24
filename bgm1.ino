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

