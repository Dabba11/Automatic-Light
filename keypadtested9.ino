
// 19, 18, 17, 16, 15, 14 atmega328p pin to
// rs, en, d4, d5, d6, d7 lcd pin  
//28 ard - 28 atmega  --> SCL
//27 ard - 27 atmega  --> SDA
//9 ard  - 13 atmega --> relay logic
//       9-10 atmega --> oscillator
//A2 - pot
//A3 -button
int flag99 = 0;
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
bool change;
unsigned long timeThen;
unsigned long timeNow;
const byte numRows   = 4;
const byte numCols   = 4;
byte flag1=0,flag2=0;
byte flagsms = 0;
int i;
int rtcdelaytime;
int buttonTimer,buttonRecord = 0;
int arrayam[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
int arraypm[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
int arrayam1[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
int arraypm1[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
char keymap[numRows][numCols]= { 
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'} };

byte rowPins[numRows] = {2,3,4,11}; //Rows 0 to 3
byte colPins[numCols]= {6,8,16,17}; //Columns 0 to 3
Keypad myKeypad= Keypad(makeKeymap(keymap), rowPins, colPins, numRows, numCols);

byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;


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
    //rv =  rv - 21;
    return rv;
}


void setup()
{
  //Serial.begin(9600);
  Wire.begin();
  lcd.begin(16,2);
  myKeypad.setHoldTime(80000);
  myKeypad.setDebounceTime(200);
  myKeypad.addEventListener(keypadEvent);
  lcd.setCursor(3,1);
  lcd.print("BOOTING ...");
  pinMode(relayPin,OUTPUT);
  pinMode(buzzerPin,OUTPUT);
  digitalWrite(relayPin,LOW);
  delay(3000);
  lcd.clear();
  delay(3000);

   
    byte firstdigit  = EEPROM.read(1);
    byte seconddigit = EEPROM.read(2);
    lcd.setCursor(9,0);
    lcd.print("FD:  ");
    lcd.print(firstdigit);
    lcd.setCursor(9,1);
    lcd.print("SD:  ");
    lcd.print(seconddigit);
    delay(2000);
    //lcd.clear(); 
    byte i=3;
    byte j =0;

    lcd.setCursor(9,0);
    lcd.print("AM: ");
    lcd.setCursor(9,1);
    lcd.print("PM: ");
    lcd.setCursor(12,0);
    lcd.print("     ");
    lcd.setCursor(12,1);
    lcd.print("     ");
      for(i = 3;i <firstdigit+3; i++){
        arrayam[i-3] = EEPROM.read(i); 
          lcd.setCursor(13,0);
          if(arrayam[i-3]<10){lcd.print("0");}
          lcd.print(arrayam[i-3]); 
          delay(1000);
          // lcd.clear();      
      }
      while(i < seconddigit+3){
         arraypm[j] = EEPROM.read(i);
          lcd.setCursor(13,1);
          if(arraypm[j]<10){lcd.print("0");}
          lcd.print(arraypm[j]);
          delay(1000);
         j++;i++;
      }
     delay(1000);
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
                    delay(2000);
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
                delay(2000);
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

byte len1;
byte len2;
byte k=12;
byte j=0;
  char valueloop = getButton();
     
  if(valueloop == 'C')
      {  
                          
                         
                               len1 = inputvalue("<===== AM =====>",arrayam1); 
                              if (len1 == 13){
                                goto exittime1;
                              }
                               len2 = inputvalue("<===== PM =====>",arraypm1);
                                if (len2 == 13){
                                goto exittime1;
                              }
                          
                                   for(i=len1; i <12;i++){ arrayam1[len1] =0;len1++;}
                                   for(i=len2; i <12;i++){ arraypm1[len2] =0;len2++;}
                                     lcd.clear();
                                     delay(3000);
                                     lcd.setCursor(9,0);
                                     lcd.print("-AM-");
                                      j=0;
                                         for(i=0;i < k;i++){
                                              lcd.setCursor(i,1);
                                              lcd.print(arrayam1[j]);
                                              if(arrayam1[j] > 9){i=i+1;k=k+1;}
                                              j++;
                                              delay(100);
                                             }
                                      delay(4000);
                                      lcd.clear();
                                      delay(3000);
                                      lcd.setCursor(9,0);
                                      lcd.print("-PM-");
                                      j=0;
                                      k=12; 
                                           for(i=0;i < k ;i++){
                                              lcd.setCursor(i,1);
                                              lcd.print(arraypm1[j]);
                                              if(arraypm1[j] > 9){i=i+1;k=k+1;}
                                              j++;
                                              delay(100);
                                           }
                                 delay(4000); 
                                 lcd.clear();
                                 delay(3000);
                                 lcd.setCursor(1,0);
                                 lcd.print("WANT TO SAVE");
                                 lcd.setCursor(0,1);
                                 lcd.print("-OK");
                                 lcd.setCursor(5,1);
                                 lcd.print("or");            
                                 lcd.setCursor(9,1);
                                 lcd.print("BACK");
                                      do{
                                        valueloop = getButton();
                                        if(valueloop == 'A'){goto exittime1;}
                                      }while( valueloop != 'D');
                    
                          
                         updateEeprom(arrayam1,arraypm1); 
                         copy(arrayam1,arrayam,len1);
                         copy(arraypm1,arraypm,len2);
                          exittime1:
                         
                                      for(i=0;i < 12;i++){
                                            lcd.setCursor(i+1,0);
                                            lcd.print("****");
                                            delay(40);
                                          }
                                     for(i=0;i < 12 ;i++){
                                      lcd.setCursor(i+1,1);
                                      lcd.print("****");
                                      delay(40);
                                       } 
                              lcd.noBlink(); 
                              firstPagedisplay();
        
   }
  
   //-----------------clock setting----------------------------------------

      if(valueloop == 'B'){
              
                    gotohour:
                    lcd.clear();
                    delay(3000);
                    char valueup;
                    byte hour1, hour2;
                    byte minute1, minute2;
                    lcd.setCursor(0,0);
                    lcd.print("HOUR");
                    lcd.setCursor(7,0);
                    lcd.print("MINUTE");
                    lcd.setCursor(13,1);
                    lcd.print("AM");
                    byte ctmp1=0;
                    byte dtmp1=0;
                    lcd.setCursor(ctmp1,1);

        //---------------------hour----------------
        
                               do{          
                                       valueup = getButton();
                                         
                                            if((valueup != NO_KEY) && (valueup != 'B')&& (valueup != 'C')&& (valueup != 'A')&& (valueup != 'D')&& (valueup != '*')&& (valueup != '0')&& (valueup != '#')){
                                                  lcd.setCursor(ctmp1,1);
                                                  lcd.print(valueup);
                                                  hour1 = valueup -'0';           
                                                  ////delay(5000);
                                              if(ctmp1 == 1){
                                                hour2=hour2*10;hour2 += hour1;
                                                    if( hour2 > 12){
                                                    //delay(5000);
                                                    //valueup=0;hour2=0;ctmp1=0;dtmp1=0;continue;}
                                                    goto gotohour;}
                                                 break;}
                                                  hour2=hour1;
                                                  ctmp1++;
                                                  dtmp1++;
                                                }
                                                if(ctmp1 < 1){
                                             
                                                if(valueup == '*'){lcd.setCursor(ctmp1,1);
                                                lcd.print("10");hour2=10;
                                               
                                                //delay(1000);
                                                break;}
                                               if(valueup == '0'){lcd.setCursor(ctmp1,1);
                                                lcd.print("11");hour2=11;
                                             
                                                //delay(1000);
                                                break;}
                                               if(valueup == '#'){lcd.setCursor(ctmp1,1);
                                                lcd.print("12");hour2=12;
                                               
                                                //delay(1000);
                                                break;}
                                                } 
                                               lcd.blink();
                                               if((valueup == 'A') && (ctmp1 !=0)) 
                                                {   if(hour2 > 9){ ctmp1--;ctmp1--;dtmp1--;}
                                                         else{ctmp1--;dtmp1--;}
                                                    
                                                    lcd.setCursor(ctmp1,1); 
                                                    lcd.print("  ");
                                                    lcd.setCursor(ctmp1,1);
//                                                       
                                                    hour2=0;
                                                }    
                            
                                                if ((millis() - timeThen) >= 120000){
                                                  timeThen = millis();
                                                  goto gotominute;
                                                              
                                                            }
                                        }while(valueup != 'D');


     
          //------------------------minute -------------------------------------------
                gotominute:
                ctmp1=0;
                ctmp1=0;
                lcd.setCursor(13,1);
                lcd.print("AM");
                lcd.setCursor(ctmp1+9,1);
                
               do{
                
                   valueup = getButton();
                     
            
                         if((valueup != NO_KEY) && (valueup != 'B')&& (valueup != 'C')&& (valueup != 'A')&& (valueup != 'D')&& (valueup != '*')&& (valueup != '0')&& (valueup != '#'))
                                {
                                 lcd.setCursor(ctmp1+9,1);
                                 lcd.print(valueup);
                                 minute1 = valueup - '0';
                                //delay(5000);
                                        if(ctmp1 == 1){minute2=minute2*10;minute2 =minute2+minute1;
                                                   if( minute2 > 60){lcd.print("Invalid Minute");delay(50);lcd.setCursor(8,1);lcd.print("        "); goto gotominute;}
                                                    
                                
                                               break;}
                                minute2=minute1;
                                ctmp1++;dtmp1++;
                               }
                      
                         if(ctmp1 <1)
                         {
                               if(valueup == '*'){lcd.setCursor(ctmp1+9,1);lcd.print("10");minute2=10; break;}
                               if(valueup == '0'){lcd.setCursor(ctmp1+9,1);lcd.print("11");minute2=11; break;}
                               if(valueup == '#'){lcd.setCursor(ctmp1+9,1); lcd.print("12");minute2=12;break;}
                            }
                 
                          if(valueup == "A"){
                         if(minute2 > 9){ ctmp1--;ctmp1--;dtmp1--;}
                               else{ctmp1--;dtmp1--;}
                          lcd.setCursor(ctmp1,1); 
                          lcd.print(" ");     
                          minute2=0;
                          }
                           if ((millis() - timeThen) >= 60000){
                            timeThen = millis();
                             break;
                                         
                                                }
                         
                 }while(valueup != 'D');
        
       bool btmp1 = false;
       char chartmp;
       byte hourtmp2=hour2;        
       lcd.setCursor(15,1);
       delay(50);
       change = false;
       
        do{
          
                valueup = getButton();
                
                     if(valueup == 'C')
                      {
                         if(btmp1 == true){lcd.setCursor(13,1);lcd.print("AM");change = false;}
                         
                         if( btmp1 == false){lcd.setCursor(13,1);lcd.print("PM");
                            if(hour2 !=12){hourtmp2=hour2+12;} 
                                else{hourtmp2=12;} 
                                change =true;
                              }
                          btmp1 = !btmp1;
                      }
                      
                      if((change == false)&&(hour2 == 12)){hourtmp2 =0;}
                      if ((millis() - timeThen) >= 60000){
                        timeThen = millis();
                         break;
                                          
                                        }
        }while(valueup != 'D');
        
         lcd.noBlink();
         lcd.clear();
         delay(3000);
             lcd.setCursor(1,0);
             lcd.print("CLOCK SETTING");
             lcd.setCursor(0,1);
             lcd.print("-OK");
             lcd.setCursor(5,1);
             lcd.print("or");            
             lcd.setCursor(9,1);
             lcd.print("BACK");
            do{
              valueloop = getButton();
              if(valueloop == 'A'){goto exittime;}
                                 
            }while( valueloop != 'D');
    
              hour2 = hourtmp2;
            setDS3231time(00,minute2,hour2,4,15,5,18);   
            exittime:
            readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);
            firstPagedisplay();
            
       }

              
}

void keypadEvent(KeypadEvent valueloop){

  if(myKeypad.getState() == HOLD){
    
      if( valueloop == 'A'){
                    //lcd.println("key1");
                    digitalWrite(buzzerPin,HIGH);
                    delay(3000);
                    digitalWrite(buzzerPin,LOW);
                    lcd.clear();
                    lcd.setCursor(3,1);
                    lcd.print("Resetting ...");
                    for(i=0;i< EEPROM.length(); i++)
                      { EEPROM.write(i,0);}
                         delay(1000);
                         resetFunc();
                     }
                       
            }
    }


void updateEeprom(int arrayam[12],int arraypm[12])
  { 
    byte k=1;
    byte i=2;
    byte j=0;
        while((arrayam[i-2] != 0) && (k<13)){ EEPROM.write(i+1,arrayam[i-2]);i++;k++;}
    EEPROM.write(1,i-2);
       k=1;
         while((arraypm[j] != 0) && (k<13)){ EEPROM.write(i+1,arraypm[j]); i++; j++;k++;}    
    EEPROM.write(2,i-2);

//   i=2;
//   j=0;
//   int value11=0;
//   lcd.clear();
//   // lcd.setCursor(0,0);
//   lcd.setCursor(5,0);
//  lcd.print(EEPROM.read(1));
//  lcd.setCursor(5,1);   
//  lcd.print(EEPROM.read(2));
//        while((arrayam[i-2] != 0) && (k<13)){value11 = EEPROM.read(i+1);lcd.setCursor(0,0);lcd.print(value11);delay(1000);i++;k++;}
//        
//       value11 =0;k=1;
//         while((arraypm[j] != 0) && (k<13)){value11 = EEPROM.read(i+1);lcd.setCursor(0,1);lcd.print(value11);delay(1000); i++; j++;k++;} 
//        
//    lcd.print("finis");
//   delay(2000);
      
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
    lcd.setCursor(1,2);
     if (hour<10){lcd.print(" ");}
    lcd.print(hour);
    lcd.print(":");
        if (minute<10){lcd.print("0");}
    lcd.print(minute);
//    lcd.print(":");
//        if (second<10){lcd.print("0");}
//    lcd.print(second);
//    lcd.setCursor(13,0);
  
}

//void displayTime()
//{
//  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
//  // retrieve data from DS3231
//  readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month,
//                 &year);
//  // send it to the serial monitor
//  Serial.print(hour, DEC);
//  // convert the byte variable to a decimal number when displayed
//  Serial.print(":");
//  if (minute < 10)
//  {
//    Serial.print("0");
//  }
//  Serial.print(minute, DEC);
//  Serial.print(":");
//  if (second < 10)
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
//  switch (dayOfWeek) {
//    case 1:
//      Serial.println("Sunday");
//      break;
//    case 2:
//      Serial.println("Monday");
//      break;
//    case 3:
//      Serial.println("Tuesday");
//      break;
//    case 4:
//      Serial.println("Wednesday");
//      break;
//    case 5:
//      Serial.println("Thursday");
//      break;
//    case 6:
//      Serial.println("Friday");
//      break;
//    case 7:
//      Serial.println("Saturday");
//      break;
//  }
//}


char getButton()
{
  //delay(500);
  char keypressed = myKeypad.getKey();
  if((keypressed != NO_KEY) &&(myKeypad.getState()== PRESSED)){return keypressed;} 
  
   return NO_KEY;
}


byte inputvalue(String display1,int arrayvalue[] ){
  byte ctmp1=0;
  byte dtmp1=0;
  byte len;
  char value; 
    lcd.clear();
    delay(3000);
    lcd.setCursor(0,0);
    lcd.print(display1); 
        do{ 
           value = getButton();
               if ((millis() - timeThen) >= 120000){
                                          
                                          flag99 = 13;
                                          timeThen = millis();
                                          return flag99;
                                        }
                 if((value != NO_KEY) && (value != 'B')&& (value != 'C')&&(value != 'A')&& (value != 'D') && (value != '*')&& (value != '0')&& (value != '#'))
                 {    
                      arrayvalue[dtmp1] = value -'0';
                      lcd.setCursor(ctmp1,1);
                      lcd.print(arrayvalue[dtmp1]);         
                      //delay(20);
                      dtmp1++;
                      ctmp1++;
                 }
                 if(value == '*'){arrayvalue[dtmp1] = 10;lcd.setCursor(ctmp1,1); lcd.print(arrayvalue[dtmp1]);ctmp1++;ctmp1++;dtmp1++;}
                   
                 if(value == '0'){arrayvalue[dtmp1] = 11;lcd.setCursor(ctmp1,1);lcd.print(arrayvalue[dtmp1]);ctmp1++;ctmp1++;dtmp1++;}
                    
                 if(value == '#'){arrayvalue[dtmp1] = 12;lcd.setCursor(ctmp1,1); lcd.print(arrayvalue[dtmp1]);ctmp1++;ctmp1++;dtmp1++;}
             if((value == 'A') && (ctmp1 !=0)) 
              {   if(arrayvalue[dtmp1-1] > 9){ ctmp1--;ctmp1--;dtmp1--;}
                       else{ctmp1--;dtmp1--;}
                  lcd.setCursor(ctmp1,1); 
                  lcd.print("  ");     
                  arrayvalue[dtmp1]=0;
              } 
             if(dtmp1 >= 12){break;}
             
          }while(value != 'D');

            
//      lcd.print("dt");
//      lcd.print(dtmp1);
//    delay(100);
    return dtmp1;  
}

void copy(int src[], int dst[], int len) 
  {
   // lcd.clear();
//   lcd.print(len);
//   delay(100); 
    
    for(i=0;i < 12;i++){ dst[i] = src[i]; 
     // lcd.print(dst[i]);
      //delay(100);
    }
}
void firstPagedisplay(){
      readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);
     //  updatetime();
      lcd.clear();
      delay(3000);
      lcd.setCursor(3,0);
      lcd.print("POON-FARM");
      lcd.setCursor(9,2);
      lcd.print("TMP:");
      lcd.setCursor(15,2);
      lcd.print("c");
}

// void SendMessage()
//{
//  if(flagsms == 0){
//  Serial.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
//  delay(1000);  // Delay of 1000 milli seconds or 1 second
//  Serial.println("AT+CMGS=\"+9779856014626\"\r"); // Replace x with mobile number
//  delay(1000);
//  Serial.println("Smart Farm - Light On");// The SMS text you want to send
//  delay(100);
//  Serial.println((char)26);// ASCII code of CTRL+Z
//  delay(1000);
//  flagsms = 1;
//  }
//}
