
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
#define relayPin 7
//#define relayPin 2
#define potPin A2
#define buzzerPin 5


const byte numRows   = 4;
const byte numCols   = 4;
int arrayam[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
int arraypm[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
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


void setup()
{
  Wire.begin();
  //Serial.begin(9600);
  //Serial.print("Ok !!");
  lcd.begin(16,2);
  lcd.print("helo");
  pinMode(relayPin,OUTPUT);
  pinMode(buzzerPin,OUTPUT);
  digitalWrite(buzzerPin,HIGH);
  digitalWrite(relayPin,HIGH);
  delay(500);
   
    byte firstdigit  = EEPROM.read(1);
    byte seconddigit = EEPROM.read(2);
    byte i=3;
    byte j =0;
    lcd.print("mutu1");
   
      for(i = 3;i <firstdigit+3; i++){
        arrayam[i-3] = EEPROM.read(i);        
      }
      while(i < seconddigit+3){
         arraypm[j] = EEPROM.read(i);
         j++;i++;
      }
     
      
 // pinMode(interruptpin,INPUT_PULLUP);
  //attachInterrupt(digitalPinToInterrupt(interruptpin),changeTime,LOW);
  // set the initial time here:
  // DS3231 seconds, minutes, hours, day, date, month, year
   //setDS3231time(59,minute2,hour2,4,15,5,18);
}

void loop()
{  
   digitalWrite(buzzerPin,LOW);
  lcd.setCursor(0,0);
  lcd.print("SMART-FARM");
   displayTime(); // display the real-time clock data on the Serial Monitor,
   delay(500);   
   int tempC = DS3231_get_treg();  // Reads the temperature as an int, to save memory
   lcd.setCursor(9,2);
   lcd.print("TMP:");
   lcd.print(tempC);
   lcd.print("c");


   
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

int arrayam1[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
int arraypm1[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
byte len1;
byte len2;
  char valueloop = getButton();
  if(valueloop == 'D')
   {  
    
    len1 = inputvalue("<-----AM----->",arrayam1); 
     for(int i=0;i < (sizeof(arrayam1)/sizeof(int)) ;i++){
            lcd.setCursor(i+1,0);
            lcd.print(arrayam1[i]);
            delay(40);
          }
          lcd.setCursor(9,1);
          lcd.print((sizeof(arrayam1)/sizeof(int)));
          delay(300);  
   len2 = inputvalue("<-----PM----->",arraypm1);
     for(int i=0;i < (sizeof(arraypm1)/sizeof(int)) ;i++){
            lcd.setCursor(i+1,0);
            lcd.print(arraypm1[i]);
            delay(40);
          }
          lcd.setCursor(9,1);
          lcd.print((sizeof(arraypm1)/sizeof(int)));
          delay(300); 
     updateEeprom(arrayam1,arraypm1); 
     copy(arrayam1,arrayam,len1);
     copy(arraypm1,arraypm,len2);
     
    lcd.print("sexcy"); 
   }
   //-----------------clock setting----------------------------------------

      if(valueloop == 'C'){
        lcd.clear();
        byte temmillis2;
        byte  temmillis1 = 0;
        int timevalue[10];
        char valueup;
        byte hour1, hour2;
        byte minute1, minute2;
        lcd.setCursor(0,0);
        lcd.print("HOUR");
        lcd.setCursor(9,0);
        lcd.print("MINUTE");
        byte ctmp1=0;
        byte dtmp1=0;
        lcd.setCursor(ctmp1,1);
        do{          
          valueup = getButton();
          if((valueup != NO_KEY) && (valueup != 'B')&& (valueup != 'C')&& (valueup != 'D')&& (valueup != '*')&& (valueup != '0')&& (valueup != '#')){
            lcd.setCursor(ctmp1,1);
            hour1 = valueup -'0';           
            ctmp1++;
            dtmp1++;
            if(ctmp1 == 1){hour2 += hour1;break;}
            hour2=hour1*10;
          }
          if(ctmp1 < 1){
          if(valueup == '*'){lcd.setCursor(ctmp1,1);
          lcd.print("10");break;}
         if(valueup == '0'){lcd.setCursor(ctmp1,1);
          lcd.print("11");break;}
         if(valueup == '#'){lcd.setCursor(ctmp1,1);
          lcd.print("12");break;}
          } 
         lcd.blink();
        }while(valueup != 'B');
      
      ctmp1=0;
      ctmp1=0;
      lcd.setCursor(ctmp1+9,1);
     do{
          valueup = getButton();
          if((valueup != NO_KEY) && (valueup != 'B')&& (valueup != 'C')&& (valueup != 'D')&& (valueup != '*')&& (valueup != '0')&& (valueup != '#')){
            lcd.setCursor(ctmp1 +9,1);
            minute1 = valueup - '0';
            ctmp1++;dtmp1++;
            if(ctmp1 == 1){minute2 += minute1;break;}
            minute2=minute1*10;
          }
          if(ctmp1 <1){
           if(valueup == '*'){lcd.setCursor(ctmp1+11,1);
          lcd.print("10");break;}
         if(valueup == '0'){lcd.setCursor(ctmp1+11,1);
          lcd.print("11");break;}
         if(valueup == '#'){lcd.setCursor(ctmp1+11,1);
          lcd.print("12");break;}
          }
        }while(valueup != 'B');
        setDS3231time(59,minute2,hour2,4,15,5,18);
        lcd.noBlink();
     }
      
   lcd.print("kto ma "); 
  //   updateEeprom(temp3,temp4,temp5,temp6,temp7,temp8,temp9,temp10); 
    // updateVar(temp3,temp4,temp5,temp6,temp7,temp8,temp9,temp10); 
     lcd.clear();   
  }


void updateEeprom(int arrayam[],int arraypm[])
{ byte i=2;
  byte j=0;
  while(arrayam[i] != 0){
      EEPROM.write(i+1,arrayam[i]);
      i++;
  }
  EEPROM.write(1,i-2);
   while(arraypm[i] != 0){
      EEPROM.write(i+1,arraypm[j]);
      i++;
      j++;
  }
  EEPROM.write(1,i-2);
  
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
  lcd.setCursor(13,0);
  
}


char getButton()
{
 char keypressed = myKeypad.getKey();
  if(keypressed != NO_KEY)
  {
  lcd.setCursor(10,2);
  //lcd.print(keypressed);
  return keypressed;
  } 
   return NO_KEY;
}

byte inputvalue(String display1,int arrayvalue[] ){
  byte ctmp1=0;
  byte dtmp1=0;
  //int inst[12]={0,0,0,0,0,0,0,0,0,0,0,0};
  byte len;
  char valueup; 
    lcd.clear();
    lcd.setCursor(2,0);
     lcd.print(display1); 
    do{ 
         char value = getButton();
         valueup = value;
         if((value != NO_KEY) && (value != 'B')&& (value != 'C')&& (value != 'D') && (value != '*')&& (value != '0')&& (value != '#')){
          arrayvalue[dtmp1] = value -'0';
          //lcd.print(typeid((value -'0')));    hernaaaaaaaaaaa xaaaaaaaaaaaa
          lcd.setCursor(ctmp1,1);
          lcd.print(arrayvalue[dtmp1]);         
          delay(20);
          dtmp1++;
          ctmp1++;
         }
         if(value == '*'){arrayvalue[dtmp1] = 10;lcd.setCursor(ctmp1,1);
          lcd.print(arrayvalue[dtmp1]);ctmp1++;ctmp1++;dtmp1++;}
         if(value == '0'){arrayvalue[dtmp1] = 11;lcd.setCursor(ctmp1,1);
          lcd.print(arrayvalue[dtmp1]);ctmp1++;ctmp1++;dtmp1++;}
         if(value == '#'){arrayvalue[dtmp1] = 12;lcd.setCursor(ctmp1,1);
          lcd.print(arrayvalue[dtmp1]);ctmp1++;ctmp1++;dtmp1++;}
//        int temmillis1 = millis();
//         int temmillis2;  
//         if((temmillis2 - temmillis1 )>40000){break;} 
          if(dtmp1 >=12){break;}
    }while(valueup != 'B');
    return dtmp1;
   
}

void copy(int* src, int* dst, int len) {
    memcpy(dst, src, sizeof(src[0])*len);
}
