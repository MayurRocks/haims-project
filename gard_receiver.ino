#include <VirtualWire.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

char m;
int cp = 0;
int reset_btn = A1;
void setup()
{
    Serial.begin(9600);  // Debugging only
    lcd.begin();
    lcd.clear();
    lcd.backlight();
    lcd.setCursor(5,0);
    lcd.print("WELCOME");
    pinMode(reset_btn,INPUT);
    vw_set_ptt_inverted(true); // Required for DR3100
    vw_set_rx_pin(12);
    vw_setup(2000);  // Bits per sec
    vw_rx_start();       // Start the receiver PLL running
}

void loop()
{
    uint8_t buf[VW_MAX_MESSAGE_LEN];
    uint8_t buflen = VW_MAX_MESSAGE_LEN;

    if (vw_get_message(buf, &buflen)) // Non-blocking
    {
     int i;
     digitalWrite(13, true); // Flash a light to show received good message
     lcd.clear();
     lcd.setCursor(0,0);
     for (i = 0; i < buflen; i++)
     {
      cp = i;
      if(cp < 16)
      {
        lcd.setCursor(cp,0);
        m = buf[i];
        lcd.print(m);
      }
      else if(cp > 15)
      {
        cp = cp - 16;
        lcd.setCursor(cp,1);
        m = buf[i];
        lcd.print(m);
      }
  }
        delay(300);
        digitalWrite(13, false);
    }
    if(analogRead(reset_btn) > 1010)
    {
      lcd.clear();
      digitalWrite(13, false);
    }
}
