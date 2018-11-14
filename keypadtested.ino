// 19, 18, 17, 16, 15, 14 atmega328p pin to
// rs, en, d4, d5, d6, d7 lcd pin  
//28 ard - 28 atmega  --> SCL
//27 ard - 27 atmega  --> SDA
//9 ard  - 13 atmega --> relay logic
//       9-10 atmega --> oscillator
// D4 ard 6 atmega   = left - cursor change on screen
// D3 ard 5 atmega= right - interrupt to change relary switch
// D2 ard 4 atmega= enter - select 
// D1 ard 3 atmega = top - ++ number
// D0 ard 2 atmega = bottom - -- number
//A0 - A1 display
//A2 - pot
//A3 -button

#include "Wire.h"
#include <EEPROM.h>
#include <Keypad.h>
#include <LiquidCrystal.h>
LiquidCrystal lcd(9, 10, 12, 13, 14, 15);
//LiquidCrystal lcd(13, 12, 8, 9, 10, 11);
#define DS3231_I2C_ADDRESS 0x68
#define DS3231_TEMPERATURE_ADDR 0x11
//#define relayPin 7
#define relayPin 2
#define potPin A2
#define buttonPin A3
#define buzzerPin 0


const byte numRows   = 4;
const byte numCols   = 4;
char keymap[numRows][numCols]= { 
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'} };

byte rowPins[numRows] = {2,3,4,6}; //Rows 0 to 3
byte colPins[numCols]= {11,8,A2,A3}; //Columns 0 to 3
Keypad myKeypad= Keypad(makeKeymap(keymap), rowPins, colPins, numRows, numCols);

byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
byte dayhour1,dayminute1,dayhour2,dayminute2,nighthour1,nightminute1,nighthour2,nightminute2;

// Convert normal decimal numbers to binary coded decimal
byte decToBcd(byte val){return( (val/10*16) + (val%10) );}
// Convert binary coded decimal to normal decimal numbers
byte bcdToDec(byte val){return( (val/16*10) + (val%16) );}


float DS3231_get_treg()
{
    int rv;  // Reads the temperature as an int, to save memory
//  float rv;
   
    uint8_t temp_msb, temp_lsb;
    int8_t nint;

    Wire.beginTransmission(DS3231_I2C_ADDRESS);
    Wire.write(DS3231_TEMPERATURE_ADDR);
    Wire.endTransmission();

    Wire.requestFrom(DS3231_I2C_ADDRESS, 2);
    temp_msb = Wire.read();
    temp_lsb = Wire.read() >> 6;

    if ((temp_msb & 0x80) != 0)
        nint = temp_msb | ~((1 << 8) - 1);      // if negative get two's complement
    else
        nint = temp_msb;

    rv = 0.25 * temp_lsb + nint;
    
    return rv;
}

//void storeEeprom(int address,int value)
//{
//  int a = EEPROM.read(address);
//  if(value != a){
//    EEPROM.write(address, value);
//    lcd.clear();
//    lcd.print("Store In Eeprom");
//    delay(350);
//  }
//}

void setup()
{
  Wire.begin();
  //Serial.begin(9600);
  //Serial.print("Ok !!");
  lcd.begin(16,2);
  //pinMode(7,OUTPUT);
  //pinMode(5,OUTPUT);
  //digitalWrite(7,LOW);
  delay(500);
    dayhour1    = EEPROM.read(1);
    dayminute1 = EEPROM.read(2);
    dayhour2    = EEPROM.read(3);
    dayminute2  = EEPROM.read(4);
    nighthour1  = EEPROM.read(5);
    nightminute2  = EEPROM.read(6);
    nighthour2  = EEPROM.read(7);
    nightminute2 = EEPROM.read(8);

  
 // pinMode(interruptpin,INPUT_PULLUP);
  //attachInterrupt(digitalPinToInterrupt(interruptpin),changeTime,LOW);
  // set the initial time here:
  // DS3231 seconds, minutes, hours, day, date, month, year
   //setDS3231time(30,25,18,4,15,5,18);
}

void updateEeprom(byte tem3, byte tem4,byte tem5,byte tem6,byte tem7,byte tem8,byte tem9,byte tem10)
{
  EEPROM.write(1,tem3);
  EEPROM.write(2,tem4);
  EEPROM.write(3,tem5);
  EEPROM.write(4,tem6);
  EEPROM.write(5,tem7);
  EEPROM.write(6,tem8);
  EEPROM.write(7,tem9);
  EEPROM.write(8,tem10);
  
}

void updateVar(byte tem3, byte tem4,byte tem5,byte tem6,byte tem7,byte tem8,byte tem9,byte tem10)
{
  dayhour1     = tem3;
  dayminute1   = tem4;
  dayhour2     = tem5;
  dayminute2   = tem6;
  nighthour1   = tem7;
  nightminute1 = tem8;
  nighthour2   = tem9;
  nightminute2 = tem10;
//  if((durationday1 + daytime1)>60){
//    if((daytime1 += 1)>24){};
//    durationday1 = durationday1 -60;
//    }
//  if((durationday2 + daytime2)>60){
//    daytime2 += 1;
//    durationday2 = durationday2 -60;
//    }
//   if((durationnight1 + nighttime1)>60){
//    daytime1+=1;
//    durationday2 = durationday2 -60;
//    }
 
    
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
void readDS3231time(byte *second,
byte *minute,
byte *hour,
byte *dayOfWeek,
byte *dayOfMonth,
byte *month,
byte *year)
{
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set DS3231 register pointer to 00h
  Wire.endTransmission();
  Wire.requestFrom(DS3231_I2C_ADDRESS, 7);
  // request seven bytes of data from DS3231 starting from register 00h
  *second = bcdToDec(Wire.read() & 0x7f);
  *minute = bcdToDec(Wire.read());
  *hour = bcdToDec(Wire.read() & 0x3f);
  *dayOfWeek = bcdToDec(Wire.read());
  *dayOfMonth = bcdToDec(Wire.read());
  *month = bcdToDec(Wire.read());
  *year = bcdToDec(Wire.read());
}
void displayTime()
{
 // byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  // retrieve data from DS3231
  readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month,
  &year);
  
  lcd.setCursor(0,2);
  lcd.print(hour);
  lcd.print(":");
   if (minute<10){lcd.print("0");}
  lcd.print(minute);
  lcd.print(":");
   if (second<10){lcd.print("0");}
  lcd.print(second);
  
//  Serial.print(hour, DEC);
//  // convert the byte variable to a decimal number when displayed
//  Serial.print(":");
//  if (minute<10)
//  {
//    Serial.print("0");
//  }
//  Serial.print(minute, DEC);
//  Serial.print(":");
//  if (second<10)
//  {
//    Serial.print("0");
//  }
//  Serial.print(second, DEC);
//  Serial.print(" ");
//  Serial.print(dayOfMonth, DEC);
//  Serial.print("/");
//  Serial.print(month, DEC);
//  Serial.print("/");
//  Serial.print(year, DEC);
//  Serial.print(" Day of week: ");
  lcd.setCursor(13,0);
  
  switch(dayOfWeek){
  case 1:
    lcd.print("Sun");
    break;
  case 2:
    lcd.print("Mon");
    break;
  case 3:
    lcd.print("Tue");
    break;
  case 4:
    lcd.print("Wed");
    break;
  case 5:
    lcd.print("Thr");
    break;
  case 6:
    lcd.print("Fri");
    break;
  case 7:
    lcd.print("Sat");
    break;
  }
}


void loop()
{  
//  lcd.setCursor(0,0);
//  lcd.print("SMART-FARM");
//   displayTime(); // display the real-time clock data on the Serial Monitor,
//   delay(500);   
//   int tempC = DS3231_get_treg();  // Reads the temperature as an int, to save memory
//   lcd.setCursor(9,2);
//   lcd.print("TMP:");
//   lcd.print(tempC);
//   lcd.print("c");


   
//   // switch section -----------
//   int currenttime = (hour*100)+minute;
//   //if(((hour >= dayhour1 && hour <=dayhour1+durationday1) && (minute >= daytime2 && minute <=daytime2+durationday2)) || ((hour >= nighttime1 && hour <=nighttime1+durationnight1) && (minute >= nighttime2 && minute <=nighttime2+durationnight2)))
//   if( ((((dayhour1*100)+dayminute1) <= currenttime ) && (((dayhour2*100)+dayminute2) >= currenttime )) || ((((nighthour1*100)+nightminute1) <= currenttime ) && (((nighthour2*100)+nightminute2) >=currenttime)))
//   {
//    
//     digitalWrite(relayPin,HIGH);
//   }else{
//     digitalWrite(relayPin,LOW);
//    }
//
//   if(((((dayhour1*100)+dayminute1)- currenttime) == 1) || (((((nighthour1*100)+ nightminute1)) - currenttime) == 1))
//   {
//     lcd.clear();
//     lcd.setCursor(2,2);
//     lcd.print("Switch ON");
//     delay(300);
//     digitalWrite(buzzerPin,HIGH);
//     delay(200);
//     digitalWrite(buzzerPin,LOW);
//     lcd.clear();
//   }
//
//    
//   if((((((nighthour2*100)+ nightminute2)) - currenttime) == 1) || ((((dayhour2*100)+dayminute2)- currenttime) == 1))
//   {
//   lcd.clear();
//     lcd.setCursor(2,2);
//     lcd.print("Switch Off");
//     delay(300);
//     digitalWrite(buzzerPin,HIGH);
//     delay(200);
//     digitalWrite(buzzerPin,LOW);
//     lcd.clear();
//   }  
   
  // if(analogRead(buttonPin) < 1020)
  byte tmp1=0;
  int inst[12];
  if(getButton() == 'D')
   {
    char valueup;  
    do{
         lcd.clear();
         lcd.setCursor(1,tmp1);
         //lcd.print("Hi");
          
         char value = getButton();
         valueup = value;
         if((value != NO_KEY) && (value != 'B')&& (value != 'C')&& (value != 'D')){
          inst[tmp1] = value -'0';
          //lcd.print(typeid((value -'0')));    hernaaaaaaaaaaa xaaaaaaaaaaaa
          lcd.setCursor(1,6);
          lcd.print(inst[tmp1]);
          delay(20);
          tmp1++;
         }
//        int temmillis1 = millis();
//         int temmillis2;  
//         if((temmillis2 - temmillis1 )>40000){break;} 
          
    }while(valueup != 'B');
    lcd.print("sexcy");
   }   
   lcd.print("kto ma "); 
//     updateEeprom(temp3,temp4,temp5,temp6,temp7,temp8,temp9,temp10); 
//     updateVar(temp3,temp4,temp5,temp6,temp7,temp8,temp9,temp10); 
//     lcd.clear();  
//         
//        do
//         {
//          if(getButton() == 1)
//          {
//              byte temp3 = updatevalue("Day Hour 1",24);delay(350);
//              byte temp4 = updatevalue("DayMinute 1",60);delay(350);
//              byte temp5 = updatevalue("Day Hour 2 ",24);delay(350);
//              byte temp6 = updatevalue("Day Minute 2",60);delay(350);
//              byte temp7 = updatevalue("NightHour 1",24);delay(350);
//              byte temp8 = updatevalue("NightMinute 1",60);delay(350);             
//              byte temp9 = updatevalue("Night Hour 2",24);delay(350);
//              byte temp10= updatevalue("Night Minute 2",60);delay(350);
//              updateEeprom(temp3,temp4,temp5,temp6,temp7,temp8,temp9,temp10); 
//              updateVar(temp3,temp4,temp5,temp6,temp7,temp8,temp9,temp10); 
//              lcd.clear();          
//          }
//
//         if(getButton() == 3)
//         {
//          byte temp7 = updatevalue("Second ",60);delay(350);          
//          byte temp9 = updatevalue("Minute ",60);delay(350);
//          byte temp10 = updatevalue("Hour ",24);delay(350);
//          byte temp11 = updatevalue("Day ",7);delay(350);
//          byte temp12 = updatevalue("Date ",32);delay(350);
//          byte temp13 = updatevalue("Month ",12);delay(350);
//          byte temp14 = updatevalue("Year ",30);delay(350);
//          lcd.clear();
//          setDS3231time(temp7,temp9,temp10,temp11,temp12,temp13,temp14);
//          
//         }

//          temmillis2 = millis()- temmillis1;
//          if((temmillis2 - temmillis1 )>20000){break;}         
//         }while(!(getButton() == 2));
//      lcd.clear();
//      lcd.setCursor(5,0); 
//      lcd.print("Saved !!!!!!!!");
//      delay(500);
//      lcd.clear();
   
  }




//int updatevalue(String sa,float num ){
//              int temp2;
//              lcd.clear();
//              do
//              {
//              lcd.setCursor(1,0);
//              lcd.print(sa);
//              lcd.setCursor(4,2);
//              int temp1 = analogRead (potPin);
//              temp2 = temp1 * (num / 1023);
//              //delay(1000); // slow vayo dheri so tala add garako
//              lcd.print(temp2);
//              lcd.print("  ");              
//              delay(200);
//              }while(!(getButton() == 2));
//              return temp2;
//             }


char getButton()
{
 char keypressed = myKeypad.getKey();
  if (keypressed != NO_KEY)
  {
  lcd.setCursor(10,2);
  //lcd.print(keypressed);
  return keypressed;
  } 
   return NO_KEY;
}




// select option ma adhkane, button type nahune lang lang hune. capacitor thulo vara khai faida ke befaida.
//common ground vayo/led 10k ma pane jallyo

