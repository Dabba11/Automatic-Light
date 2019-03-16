
#include "Wire.h"
#include <EEPROM.h>
//#include <Keypad.h>
#include <LiquidCrystal.h>
LiquidCrystal lcd(2, 3, 5, 6, 7, 8);
//LiquidCrystal lcd(13, 12, 8, 9, 10, 11);
//LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
#define DS3231_I2C_ADDRESS 0x68
#define DS3231_TEMPERATURE_ADDR 0x11
#define relayPin A1
//#define relayPin 2
/////////////////////////////////////////////////////
#define analoginput A2
#define buzzerPin A3
//#define lcdVcc  4
//bool btmp1;
//char chartmp;
//byte hourtmp2;
bool change;
float Rout;
unsigned long timeClear;
//int flag99 = 0;
unsigned long timeThen;
unsigned long timeNow;
//const byte numRows   = 4;
//const byte numCols   = 4;
byte flag1=0,flag2=0;
//byte flagsms = 0;
int i;
int rtcdelaytime;
//////////////////////////////////////////////////////////////

int buttonTimer,buttonRecord = 0;
int arrayam[12] = {1,3,5,7,9,11,0,0,0,0,0,0};
int arraypm[12] = {4,6,8,10,12,2,0,0,0,0,0,0};
int arrayam1[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
int arraypm1[12] = {0,0,0,0,0,0,0,0,0,0,0,0};


byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;


// Convert normal decimal numbers to binary coded decimal
byte decToBcd(byte val){return( (val/10*16) + (val%10) );}
// Convert binary coded decimal to normal decimal numbers
byte bcdToDec(byte val){return( (val/16*10) + (val%16) );}



void setup()
{
  timeClear = millis();
  //Serial.begin(9600);
  
  delay(4000);
  Wire.begin();
  lcd.begin(16,2);
//  myKeypad.setHoldTime(8000);
//  myKeypad.setDebounceTime(200);
//  myKeypad.addEventListener(keypadEvent);
  lcd.setCursor(3,1);
  lcd.print("BOOTING ...");
  pinMode(relayPin,OUTPUT);
//  pinMode(lcdVcc,OUTPUT);
//  digitalWrite(lcdVcc,HIGH);
  pinMode(buzzerPin,OUTPUT);
  digitalWrite(relayPin,LOW);
  //////////////////////////////////////////////////
  pinMode(analoginput, INPUT);
//  Rout = 10000*(1024.0/analogRead(analoginput)-1);
  Rout = 10000/(1024.0/analogRead(analoginput-1));
  
  delay(3000);
  lcd.clear();
  delay(3000);

//   
//    byte firstdigit  = EEPROM.read(1);
//    byte seconddigit = EEPROM.read(2);
//    lcd.setCursor(9,0);
//    lcd.print("FD:  ");
//    lcd.print(firstdigit);
//    lcd.setCursor(9,1);
//    lcd.print("SD:  ");
//    lcd.print(seconddigit);
//    delay(2000);
//    //lcd.clear(); 
//    byte i=3;
//    byte j =0;
//
//    lcd.setCursor(9,0);
//    lcd.print("AM: ");
//    lcd.setCursor(9,1);
//    lcd.print("PM: ");
//    lcd.setCursor(12,0);
//    lcd.print("     ");
//    lcd.setCursor(12,1);
//    lcd.print("     ");
//      for(i = 3;i <firstdigit+3; i++){
//        arrayam[i-3] = EEPROM.read(i); 
//          lcd.setCursor(13,0);
//          if(arrayam[i-3]<10){lcd.print("0");}
//          lcd.print(arrayam[i-3]); 
//          delay(1000);
//          // lcd.clear();      
//      }
//      while(i < seconddigit+3){
//         arraypm[j] = EEPROM.read(i);
//          lcd.setCursor(13,1);
//          if(arraypm[j]<10){lcd.print("0");}
//          lcd.print(arraypm[j]);
//          delay(1000);
//         j++;i++;
//      }
//     delay(1000);
//      for(i = 1;i <EEPROM.length(); i++){
//        int sa = EEPROM.read(i);
//        lcd.setCursor(9,0);   
//        lcd.print(sa); 
//        delay(1000);
//       lcd.clear();      
//      }

      digitalWrite(buzzerPin,HIGH);
      delay(700);
      digitalWrite(buzzerPin,LOW);
      firstPagedisplay();
      
     //readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);
     //updatetime();
      
 // pinMode(interruptpin,INPUT_PULLUP);
  //attachInterrupt(digitalPinToInterrupt(interruptpin),changeTime,LOW);
  // set the initial time here:
  // DS3231 seconds, minutes, hours, day, date, month, year
  // setDS3231time(00,58,17,3,14,1,18);
  rtcdelaytime = millis();
  displayTime();   
}

void(* resetFunc) (void) = 0;//declare reset function at address 0



void loop()
{ 
  
   //--------------------------when capsule is 1k------------------------
   if (Rout == 0){
    lcd.print("I know you Used Fuse");
   }
 if (Rout >= 1 && Rout <= 2){
    arrayam[0] = 2;
    arrayam[1] = 4;
    arrayam[2] = 6;
    arraypm[0] = 6;
    arraypm[1] = 8;
    arraypm[2] = 9;
  
   lcd.setCursor(14,1);
   lcd.print(" ");
   
  }
  else if(Rout >= 3 && Rout <= 4){
    arrayam[0] = 1;
    arrayam[1] = 3;
    arrayam[2] = 4;
    arraypm[0] = 4;
    arraypm[1] = 5;
    arraypm[2] = 7;
    
    lcd.setCursor(14,1);
    lcd.print(" ");
   
  }
  else if(Rout >= 4 && Rout <=5){
    arrayam[0] = 1;
    arrayam[1] = 3;
    arrayam[2] = 4;
    arraypm[0] = 4;
    arraypm[1] = 5;
    arraypm[2] = 7;
   
    lcd.setCursor(14,1);
    lcd.print(" ");
  }
  else{
    
    arrayam[0] = 1;
    arrayam[1] = 10;
    arrayam[2] = 4;
    arraypm[0] = 4;
    arraypm[1] = 5;
    arraypm[2] = 7;
   
    lcd.setCursor(14,1);
    lcd.print("12"); 
  }
 
  
  timeThen = millis();
   if((millis() - rtcdelaytime) >= 60000){
       displayTime(); // display the real-time clock data on the Serial Monitor,
       rtcdelaytime = millis();
   }
   delay(1000);   
//   int tempC = DS3231_get_treg();  // Reads the temperature as an int, to save memory
//   lcd.setCursor(13,2);
//   lcd.print(tempC);
   

   
 // switch section -----------

byte flag3=0,flag4=0;
 
  if(hour <12)
        {
            for( byte i=0;i<12;i++)
              {
                if(((arrayam[i] == hour) && (arrayam[i] !=0)) ||( (12-arrayam[i] == hour) && (arrayam[i] == 12)) )
                {
                    digitalWrite(relayPin,HIGH);
                    delay(5000);
                   // SendMessage();
                   // lcd.setCursor(1,0);
                    //lcd.print("HIGH-----");
                    flag3 = 1;
                        if(flag1 == 0){digitalWrite(buzzerPin,HIGH);
                          delay(3000);
                          digitalWrite(buzzerPin,LOW);flag1=1;
                          delay(2000);
                          }
                }
             }
                if(flag3 == 0){
                   // lcd.clear();
                   // lcd.print("mutu12");
                   //delay(3000);
                     digitalWrite(relayPin,LOW);}
          }
  if(hour >11)
  {
       for( byte i=0;i<12;i++)
       {
          if((((arraypm[i] +12 == hour) || (arraypm[i] == hour))) &&(arraypm[i] !=0)){
                digitalWrite(relayPin,HIGH);
                delay(5000);
               // SendMessage();
               // lcd.setCursor(1,0);
                //lcd.print("HIGH PM -----");
                flag4 = 1;
                if(flag2 == 0){digitalWrite(buzzerPin,HIGH);
                  delay(3000);
                digitalWrite(buzzerPin,LOW);flag2=1;
                }
          }
        }
         if(flag4 == 0){
                lcd.setCursor(1,0);
               // lcd.clear();
                  //lcd.print("mutu12");
                digitalWrite(relayPin,LOW);}
  }    


       if(millis() - timeClear >= 120000){
        lcd.setCursor(2,0);
        lcd.print("Cleaning Up");
        lcd.setCursor(1,1);
        lcd.print("But dont panic");
        delay(2000);
        lcd.begin(16,2);
        timeClear = millis();
        delay(1000);
        firstPagedisplay();
       }
}



void setDS3231time(byte second, byte minute, byte hour, byte dayOfWeek, byte
dayOfMonth, byte month, byte year)
{
  // sets time and date data to DS3231
      Wire.beginTransmission(DS3231_I2C_ADDRESS);
      Wire.write(0); // set next input to start at the seconds register
      Wire.write(decToBcd(second)); // set seconds
      Wire.write(decToBcd(minute)); // set minutes
      Wire.write(decToBcd(hour)); // set hours
      Wire.write(decToBcd(dayOfWeek)); // set day of week (1=Sunday, 7=Saturday)
      Wire.write(decToBcd(dayOfMonth)); // set date (1 to 31)
      Wire.write(decToBcd(month)); // set month
      Wire.write(decToBcd(year)); // set year (0 to 99)
      Wire.endTransmission();
     
}

void readDS3231time(byte *second,byte *minute,byte *hour,byte *dayOfWeek,byte *dayOfMonth,byte *month,byte *year)
{
      Wire.beginTransmission(DS3231_I2C_ADDRESS);
      Wire.write(0); // set DS3231 register pointer to 00h
      Wire.endTransmission();
      Wire.requestFrom(DS3231_I2C_ADDRESS, 7);
      // request seven bytes of data from DS3231 starting from register 00h
      *second = bcdToDec(Wire.read() & 0x7f);
      *minute = bcdToDec(Wire.read());
      *hour = bcdToDec(Wire.read() & 0x3f);
//      *dayOfWeek = bcdToDec(Wire.read());
//      *dayOfMonth = bcdToDec(Wire.read());
//      *month = bcdToDec(Wire.read());
//      *year = bcdToDec(Wire.read());
      *dayOfWeek = 0;
      *dayOfMonth =0;
      *month =0;
      *year = 0;
}
void displayTime()
{
 // byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  // retrieve data from DS3231
  readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);
    lcd.setCursor(5,2);
     if (hour<10){lcd.print(" ");}
    lcd.print(hour);
    lcd.print(":");
        if (minute<10){lcd.print("0");}
    lcd.print(minute);

  
}


void copy(int src[], int dst[], int len) 
  {

    
    for(i=0;i < 12;i++){ dst[i] = src[i]; 

    }
}
void firstPagedisplay(){
      readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);
     //  updatetime();
      lcd.clear();
      delay(3000);
      lcd.setCursor(3,0);
      lcd.print("POON-FARM");

//      }
      
}
