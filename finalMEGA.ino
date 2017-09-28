#include <Servo.h>
#include <Wire.h> 
//#include <VirtualWire.h>
#include <LiquidCrystal_I2C.h>
#include <IRremote.h>
#include "RTClib.h"
#include "DHT.h"
#define DHTPIN 40
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

LiquidCrystal_I2C lcd(0x27, 16, 2);
RTC_DS1307 rtc;

// Servo motors 
Servo right_window; 
Servo left_window;
Servo mcb;

// Input pins 
int potpin = A0;  
int RECV_PIN = 13;
int lpg_a_pin = A4;
int smoke_a_pin = A3;
int lpg_d_pin = 29;
int smoke_d_pin =47;
int flame = 48;
int reset_btn = 33;
int sp_btn = 37;
int reed_s = A9;
int acv = A1;

//int data_tx = 9;
IRrecv irrecv(RECV_PIN);

//Veriables
int val_right = 30;
int val_left = 62;
int mcb_pos = 20;
int fval = 0;
decode_results results;

//emergency veriables
int window_emg = 0;
int mcb_trip = 0;
int buzzerPin = 17;
int smoke_level = 0;
int lpg_level = 0;
int sp_status = 0;
int semg = 0;
int temg = 0;
int lpgemg = 0;
int femg = 0;

char psos[]="+918976339392";// gsm code ph no

 /*const char *msg1 = "Room no: 201     DOOR EMERGENCY";
 const char *msg2 = "Room no: 201     LPG EMERGENCY";
 const char *msg3 = "Room no: 201     FIRE EMERGENCY";
 const char *msg4 = "Room no: 201     ACV EMERGENCY";
   */
//leds
int mcb_led = 30;
int power_led = 36;
int room_led = 41;
int lpg_led = 34;
int smoke_led = 32;
int flame_led = 5;// not working
int reset_led = 31;
int pause_led = 35;
int start_led = 39;

void setup()
{

Serial.begin(9600);
lcd.begin();
dht.begin();
irrecv.enableIRIn(); 

 if (! rtc.isrunning())
{
rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}
  

right_window.attach(9);
left_window.attach(8);
mcb.attach(11);  
mcb.write(20);
//vw_set_ptt_inverted(true);
//vw_set_tx_pin(9);
//vw_setup(1000);  

pinMode(reed_s,INPUT);
//pinMode(data_tx,OUTPUT);      
pinMode(room_led,OUTPUT);
pinMode(reset_btn,INPUT);
pinMode(reset_led,OUTPUT);
pinMode(mcb_led,OUTPUT);
pinMode(lpg_a_pin,INPUT);
pinMode(lpg_d_pin,INPUT);
pinMode(acv, INPUT);
pinMode(smoke_a_pin,INPUT);
pinMode(smoke_d_pin,INPUT);
pinMode(power_led,OUTPUT);
digitalWrite(power_led,HIGH);
pinMode(buzzerPin,OUTPUT);
pinMode(flame_led,OUTPUT);
pinMode(flame,INPUT);
pinMode(lpg_led,OUTPUT);
pinMode(smoke_led,OUTPUT);
pinMode(sp_btn,INPUT);
pinMode(start_led,OUTPUT);
pinMode(pause_led,OUTPUT);


}// end of setup **********************************************************************************

void loop() 
{
 
sp_check();
if(sp_status == 0)
{
digitalWrite(start_led,HIGH); 
 // rtc date and time display ============================================================
DateTime now = rtc.now();
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Date: ");
    lcd.print(now.day(), DEC);
    lcd.print('/');
    lcd.print(now.month(), DEC);
    lcd.print('/');
    lcd.print(now.year(), DEC);
    
     Serial.print("Date: ");
     Serial.print(now.day(), DEC);
     Serial.print('/');
     Serial.print(now.month(), DEC);
     Serial.print('/');
     Serial.print(now.year(), DEC);
     Serial.print(',');
    
    lcd.setCursor(0,1);
    lcd.print("Time: ");
    lcd.print(now.hour(), DEC);
    lcd.print(':');
    lcd.print(now.minute(), DEC);
    lcd.print(':');
    lcd.print(now.second(), DEC);
    
     Serial.print("Time: ");
     Serial.print(now.hour(), DEC);
     Serial.print(':');
     Serial.print(now.minute(), DEC);
     Serial.print(':');
     Serial.print(now.second(), DEC);
     Serial.print(',');
    
    delay(800);
 // DTH22 ================================================================================
 float h = dht.readHumidity();
 float t = dht.readTemperature();
 if (isnan(h) || isnan(t) ) 
   {
    return;
   }
lcd.clear();   
lcd.setCursor(0,0);
lcd.print("Humidity: ");
lcd.print(h);
lcd.print("% ");
lcd.setCursor(0,1);
lcd.print("Temp: ");
lcd.print(t);
lcd.print("C");

 Serial.print(" Humidity: ");
 Serial.print(h);
 Serial.print("%,");
 Serial.print(" Temp: ");
 Serial.print(t);
 Serial.print("C,");

if(t >= 50.00) 
{ 
 mcb_tripper();
 window();
// vw_send((uint8_t *)msg3, strlen(msg3));
 //vw_wait_tx();
 delay(1000);
   
 if(temg == 0)
 {
   
    delay(2500);  
    Serial.print("AT+CMGS=\"");
    Serial.print(psos);
    Serial.println("\"");
    while(Serial.read()!='>');
    {
    Serial.print("High Room TEMP > 40*C TAKE Action ");
      delay(500);
      Serial.write(0x1A);
      Serial.write(0x0D);
      Serial.write(0x0A);
      delay(5000);
    }
 
 temg = 1;
 }
 }
delay(1500);   

 // functions ================================================================================
 rml();
manual_window();
 rml();
gas_check();
 rml();
reset();
 rml();
flame_check();
 rml();
 door_status();
 rml();
 acvolt();
 rml();
 sp_check();

// remote control ========================================================================
  rml();
}
else
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Circuit Pause");
  delay(800);
  
}
 Serial.println();
  
}// end of loop *********************************************************************
void rml()
{
  if (irrecv.decode(&results))
  {
  Serial.println(results.value, DEC);
  irrecv.resume(); 
  if(results.value == 16236607)
  {
    digitalWrite(room_led,HIGH);
  }
  else  if(results.value == 16203967)
  {
  digitalWrite(room_led,LOW);
  }
  irrecv.resume();
  }
}
void sp_check()
{
  if(digitalRead(sp_btn) == HIGH)
  {
    if(sp_status == 0)
    {
      sp_status = 1;
      digitalWrite(start_led,LOW);
      digitalWrite(pause_led,HIGH);
    }
    else
    if(sp_status == 1)
    {
      sp_status = 0;
      digitalWrite(start_led,HIGH);
      digitalWrite(pause_led,LOW);
      
    }
  delay(800);
  }
}
void flame_check()
{
   Serial.print(" Flame: ");
   Serial.print(digitalRead(flame));
   Serial.print(",");
  
  if(digitalRead(flame)== LOW)
  {
   // vw_send((uint8_t *)msg3, strlen(msg3));
   // vw_wait_tx();
    delay(1000);
   
  if(fval == 0)
    {
      fval = 1;
      mcb_tripper();
      right_window.write(130);  
      left_window.write(60);  
      digitalWrite(flame_led,HIGH);
      delay(100);
      
  if(femg == 0)
  {
      delay(2500);  
      Serial.print("AT+CMGS=\"");
      Serial.print(psos);
      Serial.println("\"");
      while(Serial.read()!='>');
      {
        Serial.print("High Room TEMP > 40*C TAKE Action ");
        delay(500);
        Serial.write(0x1A);
        Serial.write(0x0D);
        Serial.write(0x0A);
        delay(5000);
      }
  
  femg = 1;
  }   
    }
  }
}
void gas_check()
{

lpg_level = analogRead(lpg_a_pin);
lpg_level =  map(lpg_level, 0, 1023, 000, 3000);
smoke_level = analogRead(smoke_a_pin);
smoke_level =  map(smoke_level, 0, 1023, 00, 3000);
 
 lcd.clear();
 lcd.setCursor(0, 0);
 lcd.print("CO2+CO ppm:");
 lcd.print(smoke_level);
  Serial.print(" Smoke: ");
   Serial.print(digitalRead(47));
   Serial.print(",");
  
 if(digitalRead(47)== HIGH)  
 { 
 digitalWrite(buzzerPin, HIGH);
 mcb_tripper();
 window_emg = 1;
 window();
// vw_send((uint8_t *)msg2, strlen(msg2));
// vw_wait_tx();
// delay(1000);
   
 digitalWrite(smoke_led, HIGH);
 
 if(semg == 0)
{
     Serial.println("AT+CMGF=1");
     delay(500);
    Serial.print("AT+CMGS=\"");
    Serial.print(psos);
    Serial.println("\"");
    delay(500);
    while(Serial.read()!='>');
    {
      Serial.print(" SMOKE ALERT");
      delay(500);
      Serial.write(0x1A);
      Serial.write(0x0D);
      Serial.write(0x0A);
      delay(5000);
    }
   
 semg = 1;
 }
 }
  else
 {
 digitalWrite(buzzerPin, LOW);
 }

 lcd.setCursor(0, 1);
 lcd.print("LPG ppm:");
 lcd.print(lpg_level);
  Serial.print(" LPG: ");
   Serial.print(digitalRead(29));
   Serial.print(",");
  
 if(digitalRead(29)== HIGH)  
 { 
 digitalWrite(buzzerPin, HIGH);
 mcb_tripper();
 window_emg = 1;
 window();
 //vw_send((uint8_t *)msg2, strlen(msg2));
   // vw_wait_tx();
    delay(1000);
   
 digitalWrite(lpg_led, HIGH);
 if(lpgemg == 0)
 {
     delay(1500);
     Serial.println("AT+CMGF=1");
     delay(500);
    
    Serial.print("AT+CMGS=\"");
    Serial.print(psos);
    Serial.println("\"");
    while(Serial.read()!='>');
    {
      Serial.print(" LPG LEAK ALERT");
      delay(500);
      Serial.write(0x1A);
      Serial.write(0x0D);
      Serial.write(0x0A);
      delay(5000);
    }
 lpgemg = 1;
 }
 }
 else
 {
   digitalWrite(buzzerPin, LOW);
 }
 delay(1000);
}
void acvolt()
{ 
  lcd.clear();
  float a;
  a = analogRead(acv);
  float v = a * (280.0 / 1023.0);
  lcd.clear();
  lcd.setCursor(3,0);
  lcd.print("AC Voltage");
  lcd.setCursor(0,1);
    lcd.print("VAC: ");
  lcd.print(v);
  lcd.print(" VAC");

   Serial.print(" VAC: ");
   Serial.print(v);
   Serial.print(",");
  
  if(v>250)
  {
    
 //vw_send((uint8_t *)msg4, strlen(msg4));
   // vw_wait_tx();
    delay(1000);
    mcb_tripper();
  }
delay(800);
}
void mcb_tripper()
{
  if(mcb_trip == 0)
  {
     for (mcb_pos = 15; mcb_pos <= 30; mcb_pos += 3) 
     {
      mcb.write(mcb_pos);
      delay(18);   
     }
     digitalWrite(mcb_led,HIGH);
     mcb_trip = 1;
  
  digitalWrite(reset_led,HIGH);
  }
  delay(800);
}
void manual_window()
{
  if(window_emg == 0 && fval == 0)
  {
    val_right = analogRead(potpin);          
    val_left = analogRead(potpin); 
    val_right = map(val_right, 0, 1023, 30, 130);
    val_left = map(val_left, 0, 1023, 160, 60);
    right_window.write(val_right);  
    left_window.write(val_left);  
    delay(100);          
  } 
}

void window()
{
  if(window_emg == 1)
  {
    right_window.write(30);  
    left_window.write(160);  
    digitalWrite(reset_led,HIGH);
    delay(100);          
  } 
}

void reset()
{
    if(digitalRead(reset_btn )== HIGH)
     {
     digitalWrite(smoke_led,LOW);
     digitalWrite(lpg_led,LOW);
     digitalWrite(flame_led,LOW);
     digitalWrite(mcb_led,LOW);
     for (mcb_pos = 30; mcb_pos >= 15; mcb_pos -= 1) 
     {
      mcb.write(mcb_pos);
      delay(15);   
     }
  
      window_emg = 0;
      mcb_trip = 0;
      fval = 0;
       femg = 0;
       lpgemg = 0;
       semg = 0;
       temg = 0;     
     digitalWrite(reset_led,LOW);
     }
}


void door_status()
{
  int ad;
  if(analogRead(reed_s)==1023)
  {
    ad = 1;
  }
    else
  {
  ad = 0;
  }
   Serial.print(" door: ");
   Serial.print(ad);
   Serial.print(",");
  
  if(ad == 1)
  {
 lcd.clear();
 lcd.setCursor(0, 0);
 lcd.print("Door Safe");
  }
  if(ad == 0)
  {
     lcd.clear();
     lcd.setCursor(0, 0);
     lcd.print("Door Opened ");
     lcd.setCursor(0, 1);
    lcd.print("Please Close ");
    
// vw_send((uint8_t *)msg1, strlen(msg1));
 //   vw_wait_tx();
    delay(800);
   }
   delay(800);
}

