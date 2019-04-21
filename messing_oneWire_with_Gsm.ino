

// 19, 18, 17, 16, 15, 14 atmega328p pin to
// rs, en, d4, d5, d6, d7 lcd pin  
//28 ard - 28 atmega  --> SCL
//27 ard - 27 atmega  --> SDA
//9 ard  - 13 atmega --> relay logic
//       9-10 atmega --> oscillator
//A2 - pot
//A3 -button

#include "Wire.h"
#include <EEPROM.h>
#include <Keypad.h>
#include <avr/wdt.h>
#include <LiquidCrystal.h>
#include <OnewireKeypad.h>
#include <SoftwareSerial.h>
SoftwareSerial mySerial(10, 9);
LiquidCrystal lcd(2, 3, 5, 6, 7, 8);
bool firstOff = !(EEPROM.read(28));
//LiquidCrystal lcd(13, 12, 8, 9, 10, 11);
//LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
#define DS3231_I2C_ADDRESS 0x68
#define DS3231_TEMPERATURE_ADDR 0x11
#define relayPin 12
#define relay1 13
//#define relayPin 2
//#define potPin A2
#define buzzerPin 4
#define tempPin A3
///////////////////////////////////////////////////////////////////////////////////////////////////

#define gsmReset 11
enum DataType {
    CMD     = 0,
    DATA    = 1,
    CALL = 2
};
bool blcReturn = true;
bool call;
bool getPack = false;
char sajalnum[10];
bool initialized = false;
unsigned int balance;

bool notinitialized = true;
unsigned int previousIndex = 0;
bool change;
bool msgbucket = 0, sentbucket = 0, sentbucketcall = 0;
char clientNum[10] = "9840898572";
char pack[4] = "1415";
bool checkBalanceTrue = false;
//byte second, minute, hour;
unsigned int strength, actual = EEPROM.read(29);
bool heat = EEPROM.read(31);
byte limit = EEPROM.read(30);

bool btmp1;
char chartmp;
byte hourtmp2;

float Rout;
unsigned long timeClear;
int flag99 = 0;
unsigned long timeThen;
unsigned long timeNow;
const byte numRows   = 4;
const byte numCols   = 4;
byte flag1=0,flag2=0;
//byte flagsms = 0;
int i;
int rtcdelaytime;

//int buttonTimer,buttonRecord = 0;
int arrayam[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
int arraypm[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
int arrayam1[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
int arraypm1[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
//char keymap[numRows][numCols]= { 
//  {'1', '2', '3', 'A'},
//  {'4', '5', '6', 'B'},
//  {'7', '8', '9', 'C'},
//  {'*', '0', '#', 'D'} };
//
//byte rowPins[numRows] = {2,3,4,11}; //Rows 0 to 3
//byte colPins[numCols]= {6,8,16,17}; //Columns 0 to 3
//Keypad myKeypad= Keypad(makeKeymap(keymap), rowPins, colPins, numRows, numCols);

byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;


// Convert normal decimal numbers to binary coded decimal
byte decToBcd(byte val){return( (val/10*16) + (val%10) );}
// Convert binary coded decimal to normal decimal numbers
byte bcdToDec(byte val){return( (val/16*10) + (val%16) );}



//char KEYS[]= { 
//  0, '1', '4', '7', '*',
//     '2', '3', 'A', '5',
//     '6', 'B', '8', '9',
//     '0', 'C', '#', 'D' };
     
char KEYS[]= { 
  0, '1', '4', '7', '*',
     '2', '3', 'A', '5',
     '6', 'B', '8', '9',
     '0', 'C', '#', 'D' };
OnewireKeypad <Print, 16 > KP2(Serial, KEYS, 4, 4, A1, 4700, 1000 );

/*Tower characters*/
byte pole[8] = {
    B11111,
    B01010,
    B01010,
    B00100,
    B00100,
    B00100,
    B00100,
    B00100,
};
byte bar1[8] = {
    B11111,
    B01010,
    B01010,
    B00100,
    B00100,
    B00100,
    B00100,
    B00101,
};
byte bar2[8] = {
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
    B10000,
    B10000,
    B10000,
};
byte bar3[8] = {
    B00000,
    B00000,
    B00000,
    B00100,
    B00100,
    B10100,
    B10100,
    B10100,
};
byte bar4[8] = {
    B00000,
    B00001,
    B00001,
    B00101,
    B00101,
    B10101,
    B10101,
    B10101,
};

void setup()
{
  delay(5000);
  lcd.begin(16,2);
  mySerial.begin(9600);
  Wire.begin();
  pinMode(gsmReset,OUTPUT);
  digitalWrite(gsmReset, LOW);
  lcd.createChar(0, pole);
  lcd.createChar(1, bar1);
  lcd.createChar(2, bar2);
  lcd.createChar(3, bar3);
  lcd.createChar(4, bar4); 
  timeClear = millis();
  //Serial.begin(9600);
  delay(100);
  pinMode(relayPin,OUTPUT);
  //pinMode(lcdVcc,OUTPUT);
  //digitalWrite(lcdVcc,HIGH);
  pinMode(buzzerPin,OUTPUT);
  pinMode(relay1, OUTPUT);
  delay(1000);

//KP2.setHoldTime(80000);
//  myKeypad.setDebounceTime(200);
//  myKeypad.addEventListener(keypadEvent);
    lcd.setCursor(3,1);
    lcd.print("BOOTING ...");
    lcd.setCursor(2,0);
    lcd.print("DABBA FARM");
    KP2.SetKeypadVoltage(5.18);
    delay(3000);
    lcd.clear();
    digitalWrite(relayPin,LOW);
    digitalWrite(relay1, LOW);
    byte firstdigit  = EEPROM.read(1);
    byte seconddigit = EEPROM.read(2);
    lcd.setCursor(9,0);
    lcd.print("FD:  ");
    lcd.print(firstdigit);
    lcd.setCursor(9,1);
    lcd.print("SD:  ");
    lcd.print(seconddigit);
    delay(2000);
    lcd.clear(); 
    byte i=3;
    byte j =0;

    lcd.setCursor(0,0);
    lcd.print("AM: ");
    lcd.setCursor(0,1);
    lcd.print("PM: ");
    lcd.setCursor(3,0);
    lcd.print("     ");
    lcd.setCursor(3,1);
    lcd.print("     ");
    lcd.setCursor(4,0);
   
      for(i = 3;i <firstdigit+3; i++){
        arrayam[i-3] = EEPROM.read(i); 
          //if(arrayam[i-3]<10){lcd.print("0");}
          lcd.print(arrayam[i-3]); 
          delay(1000);
          // lcd.clear();      
      }
      delay(1000);
      lcd.setCursor(4,1);
      while(i < seconddigit+3){
         arraypm[j] = EEPROM.read(i);
          //if(arraypm[j]<10){lcd.print("0");}
          lcd.print(arraypm[j]);
          delay(1000);
         j++;i++;
      }
      delay(2000);
    

//      for(i = 1;i <EEPROM.length(); i++){
//        int sa = EEPROM.read(i);
//        lcd.setCursor(9,0);   
//        lcd.print(sa); 
//        delay(1000);
//       lcd.clear();      
//      }

      digitalWrite(buzzerPin,HIGH);
      delay(1000);
      digitalWrite(buzzerPin,LOW);

     if(checkPower()){
        
        mySerial.write("AT+CSCLK=0\r");
        delay(500);
        mySerial.write("ATE0\r");
       checkWith("AT+CLIP=1\r\n","OK\r\n",200,DATA);
       //mySerial.write("AT+CLIP=1\r\n");

       delay(1000);
    }
    delay(1000);
    if(checkWith("AT+CSQ\r\n","+CSQ: ",2000,CMD)){
            strength = balance;
            delay(1000);
            mySerialFlush();
            delay(6000);
    }

    firstPagedisplay();
      
    //readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);
    //updatetime();
      
    // pinMode(interruptpin,INPUT_PULLUP);
    //attachInterrupt(digitalPinToInterrupt(interruptpin),changeTime,LOW);
    // set the initial time here:
    // DS3231 seconds, minutes, hours, day, date, month, year
    // setDS3231time(00,58,17,3,14,1,18);
    rtcdelaytime = (millis()/60000)+1;
    mySerialFlush();
    //wtd_disable();
    displayTime(); 
    if (((getTemp() > limit) && (heat == 1))||((getTemp() < limit)&&(heat == 0))){
        digitalWrite(relay1, HIGH);
    }
    else {
        digitalWrite(relay1, LOW);
    }
    delay(3000);
    
}

void(* resetFunc) (void) = 0;//declare reset function at address 0
bool checked = false;
byte count = 0;
unsigned int tempsum = 0;
unsigned long newTime = millis();
unsigned int tempavg;
void loop()
{
    if ((millis() - newTime) >= 9999){
        tempsum += getTemp();
        count++;
        newTime = millis();
        if (count = 10){
            tempavg = tempsum/10;
            count = 0;
            tempsum = 0;
        }
    }
    //delay(3000);
    timeThen = millis()/60000;
    if((millis() - rtcdelaytime) >= 1){
        displayTime(); // display the real-time clock data on the Serial Monitor,
        rtcdelaytime = millis()/60000;

              if(checkWith("AT+CSQ\r\n","+CSQ: ",2000,CMD)){  /*Signal Strength extraction*/
            strength = balance;
            delay(1000);
            mySerialFlush();
        }
        if(checkBalanceTrue == true){
            if(checkWith("AT+CUSD=1,\"*400#\"\r\n","Rs ",15000,CMD)){
                /*Balance update*/
                if (balance < actual){
                    getPack = true;
                }
                if(getPack){
                    sendSMS(pack);
                }
                blcReturn = true;
                mySerialFlush();
                checkBalanceTrue = false;
                actual = balance;
                EEPROM.write(29, actual);
            }
        else{
            blcReturn = false;
            }
        }
        
        byte flag3=0,flag4=0;
     
        if(hour <12){
            for( byte i=0;i<12;i++){
                if(((arrayam[i] == hour) && (arrayam[i] !=0)) ||( (12-arrayam[i] == hour) && (arrayam[i] == 12)) ){
                    digitalWrite(relayPin,HIGH);
                    delay(1000);
                    flag3 = 1;
                    if(flag1 == 0){
                        digitalWrite(buzzerPin,HIGH);
                        msgbucket = 1;
                        sentbucket = 1;
                        delay(1500);
                        digitalWrite(buzzerPin,LOW);
                        delay(1000);
                        flag1=1;
                    }
                    if (firstOff == 1){
                        sentbucket = 0;
                        firstOff = 0;
                    } 
                }
            }
            if(flag3 == 0){
                digitalWrite(relayPin,LOW);
                if(flag1 == 1){
                    digitalWrite(buzzerPin,HIGH);
                    msgbucket = 0;
                    sentbucket = 1;
                    delay(1500);
                    flag1 = 0;
                    digitalWrite(buzzerPin,LOW);
                    delay(1000);
                }
            }
        }
              
        if(hour >11){
            for( byte i=0;i<12;i++){
                if((((arraypm[i] +12 == hour) || (arraypm[i] == hour))) &&(arraypm[i] !=0)){
                    digitalWrite(relayPin,HIGH);
                    delay(2000);
                    flag4 = 1;
                    if(flag2 == 0){
                        digitalWrite(buzzerPin,HIGH);
                        msgbucket = 1;
                        sentbucket = 1;
                        delay(2000);
                        digitalWrite(buzzerPin,LOW);
                        flag2=1;
                        delay(2000);
                    }
                    if (firstOff == 1){
                        sentbucket = 0;
                        firstOff = 0;
                    }
                }
            }
            if(flag4 == 0){
                digitalWrite(relayPin,LOW);
                if (flag2 == 1){
                    digitalWrite(buzzerPin,HIGH);
                    msgbucket = 0;
                    sentbucket = 1;
                    flag2 = 0;
                    digitalWrite(buzzerPin,LOW);
                    delay(2000);
                }
            }
        }
        if (((tempavg > limit) && (heat == 1))||((tempavg < limit)&&(heat == 0))){
            digitalWrite(relay1, HIGH);
        }
        else {
            digitalWrite(relay1, LOW);
        }
        if(sentbucket == 1){
            sendSMS(clientNum);
            mySerialFlush();
        }
    
        if(sentbucketcall == 1){
            sendSMS(sajalnum);
        }

   }

    EEPROM.write(28, sentbucket);
//    if(Serial.available()>0){
//        mySerial.write(Serial.read());
//    }
    while(mySerial.available()>0){
        char in_char = mySerial.read();
        if (in_char == 'R'){
            delay(10);
            in_char = mySerial.read();
            if(in_char == 'I'){
                delay(10);
                in_char = mySerial.read();
                if(in_char == 'N'){
                    delay(10);
                    in_char = mySerial.read();
                    if(in_char == 'G'){
                        delay(10);
                        call = true;
                        
                        if(checkWith("","+CLIP: \"",100,CMD)){ 
                            mySerial.println("ATH");   /*Hanging up the call*/
                            mySerialFlush();
                            delay(5000);
                            if (!sendSMS(sajalnum)){
                                sentbucketcall = 1;
                            }
                            else{sentbucketcall = 0; }
                            call = false;
                            break;
                        }
                    }
                }
            }
        }
    }
 
 
    

    byte len1;
    byte len2;
    byte k=12;
    byte j=0;
    delay(800);
    char valueloop = getButton();

        byte ctmp1 = 6;
        char valueup;
    if(valueloop == 'A'){
        timeThen = millis();
        lcd.clear();
        delay(500);
        lcd.setCursor(2,0);
        lcd.print("Set  Temp :");      
        do{
            delay(800);
            valueup = getButton();
            if((valueup != NO_KEY) && (valueup != 'B')&& (valueup != 'C')&& (valueup != 'A')&& (valueup != 'D')&& (valueup != '*')&& (valueup != '#') && (ctmp1 < 8)){
                lcd.setCursor(ctmp1,1);
                lcd.print(valueup);
                limit = valueup - '0';
                ctmp1++;
            }
            if(valueup == 'A'){
                ctmp1 = 6;
                limit = 0;
                lcd.setCursor(6,1);
                lcd.print("        ");
            }
            if (((millis() - timeThen) >= 120000)){
                timeThen = millis();
                goto lasttime;
            }
        }while(valueup != 'D');
        lcd.clear();
        delay(3000);
        lcd.setCursor(0,0);
        lcd.print("Choose Set.(C)");
        lcd.setCursor(3, 1);
        lcd.print("Heater");
        heat = 1;
        timeThen = millis();
        delay(3000);
        do{
            delay(800);
            char valueup = getButton();
            if (valueup == 'C'){
                if (heat == 1){
                    lcd.setCursor(3,1);
                    lcd.print("Cooler");
                    heat = 0;
                }
                if (heat == 0){
                    lcd.setCursor(3,1);
                    lcd.print("Heater");
                    heat = 1;
                }
            }
            if (((millis() - timeThen) >= 120000)){
                timeThen = millis();
                goto lasttime;
            }
        }while(valueup != 'D');
        delay(1000);
       lasttime:
        lcd.clear();
        delay(3000);
       lcd.setCursor(1,0);
       lcd.print("SAVE SETTING?");
       lcd.setCursor(0,1);
       lcd.print("-OK");
       lcd.setCursor(5,1);
       lcd.print("or");            
       lcd.setCursor(9,1);
       lcd.print("BACK");
       timeThen = millis();
        do{
            delay(800);
            valueup = getButton();
            if(valueup == 'A'){
                if (heat == 0){
                    limit = 100; 
                }
                else{
                    limit = 0;
                }
                goto nosavelimit;
            }
            if ((millis() - timeThen) >= 60000){
                timeThen = millis(); timeClear = millis();
                goto nosavelimit;
            }
        }while(valueup != 'D');
        EEPROM.write(30, limit);
        EEPROM.write(31, heat);
        nosavelimit:
        lcd.clear();
        delay(2000);
        firstPagedisplay();
    }
 
    if(valueloop == 'C'){  
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
                                 lcd.print("SAVE SCHEDULE");
                                 lcd.setCursor(0,1);
                                 lcd.print("-OK");
                                 lcd.setCursor(5,1);
                                 lcd.print("or");            
                                 lcd.setCursor(9,1);
                                 lcd.print("BACK");
                                 timeThen = millis();
                                      do{
                                       delay(800);
                                        valueloop = getButton();
                                        if((valueloop == 'A') || ((millis() - timeThen)>=120000)){timeThen = millis();timeClear = millis();goto exittime1;}
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
        timeThen = millis();
              
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
                                          delay(800);
                                          valueup = getButton();
                                            if((valueup != NO_KEY) && (valueup != 'B')&& (valueup != 'C')&& (valueup != 'A')&& (valueup != 'D')&& (valueup != '*')&& (valueup != '0')&& (valueup != '#')){
                                                  lcd.setCursor(ctmp1,1);
                                                  lcd.print(valueup);
                                                  hour1 = valueup -'0';           
                                                  ////delay(5000);
                                              if(ctmp1 == 1){hour2=hour2*10;hour2 += hour1;
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
                                                   if((valueup == 'A')&& (ctmp1!=0)) // or hour2!=0 rakhda ni hunxa hola
                                                {   if(hour2 > 9){ ctmp1--;ctmp1--;dtmp1--;}
                                                         else{ctmp1--;dtmp1--;}
                                                    
                                                    lcd.setCursor(ctmp1,1); 
                                                    lcd.print("  ");
                                                    lcd.setCursor(ctmp1,1);
//                                                       
                                                    hour2=0;
                                                }
                                                if (((millis() - timeThen) >= 130000)){
                                                  timeThen = millis();
                                                  goto exittime;
                                                              
                                                            } 
                                                 
                            }while(valueup != 'D');


     
          //------------------------minute -------------------------------------------
                gotominute:
                ctmp1=0;
                dtmp1=0;
                timeThen = millis();
                lcd.setCursor(13,1);
                lcd.print("AM");
                lcd.setCursor(ctmp1+9,1);
                
               do{
                  delay(800);
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
                 if((valueup == 'A')&&(ctmp1 != 0)) {
                 if(minute2 > 9){ ctmp1--;ctmp1--;dtmp1--;}
                               else{ctmp1--;dtmp1--;}
                          lcd.setCursor(ctmp1+9,1); 
                          lcd.print("  ");
                          lcd.setCursor(ctmp1+9,1);      
                          minute2=0;
                 }
                           if (((millis() - timeThen) >= 130000) ){
                            timeThen = millis();
                             goto exittime;
                                 }        
                                            
                 
                 
                 
                 
                 
                 }while(valueup != 'D');
                  //---------------------------AM/PM--------------------------------------
        
       btmp1 = false;
       char chartmp;
       hourtmp2=hour2;       
       lcd.setCursor(15,1);
       delay(50);
       change = false;
       timeThen = millis();
       
        do{
          delay(800);
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
                       if (((millis() - timeThen) >= 120000)){
                        timeThen = millis();
                         goto exittime;
                                          
                                        }
        }while(valueup != 'D');
        
         lcd.noBlink();
         lcd.clear();
         delay(3000);
             lcd.setCursor(1,0);
             lcd.print("SAVE CLOCK?");
             lcd.setCursor(0,1);
             lcd.print("-OK");
             lcd.setCursor(5,1);
             lcd.print("or");            
             lcd.setCursor(9,1);
             lcd.print("BACK");
             timeThen = millis();
            do{
              delay(800);
              valueloop = getButton();
              if((valueloop == 'A') || ((millis()-timeThen)>=60000)){timeThen = millis(); timeClear = millis();goto exittime;}
            }while( valueloop != 'D');
    
              hour2 = hourtmp2;
            setDS3231time(00,minute2,hour2,4,15,5,18);   
            exittime:
            readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);
            firstPagedisplay();
       }
       
       lcd.noBlink();

       if(millis() - timeClear >= 120000){
        lcd.setCursor(2,0);
        delay(1000);
        lcd.begin(16,2);
        timeClear = millis();
        delay(1000);
        firstPagedisplay();
        displayTime();
       }
       
}

//void keypadEvent(KeypadEvent valueloop){
//
//  if(myKeypad.getState() == HOLD){
//    
//      if( valueloop == 'A'){
//                    //lcd.println("key1");
//                    digitalWrite(buzzerPin,HIGH);
//                    delay(3000);
//                    digitalWrite(buzzerPin,LOW);
//                    lcd.clear();
//                    lcd.setCursor(3,1);
//                    lcd.print("Resetting ...");
//                    for(i=0;i< EEPROM.length(); i++)
//                      { EEPROM.write(i,0);}
//                         delay(1000);
//                         resetFunc();
//                     }
//                       
//            }
//    }


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
    takeagain:
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
    lcd.setCursor(1,1);
     if (hour<10){lcd.print(" ");}
    lcd.print(hour);
    lcd.print(":");
        if (minute<10){lcd.print("0");}
    lcd.print(minute);
//    if((millis() - timeThen) >= 600000){
//      lcd.clear();
//      delay(1000);
//      timeThen = millis();
//    }
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


char getButton(){
    //char keypressed = KP2.Getkey();
//    delay(100);
    if (char key = KP2.Getkey()){
        return key;
    } 
    return NO_KEY;
}


byte inputvalue(String display1,int arrayvalue[] ){
   timeThen = millis();
  byte ctmp1=0;
  byte dtmp1=0;
  byte len;
  char value; 
    lcd.clear();
    delay(3000);
    lcd.setCursor(0,0);
    lcd.print(display1); 
        do{ 
          delay(800);
           value = getButton();
           if (((millis() - timeThen) >= 120000) && value == NO_KEY ){
                                          
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
                      timeThen = millis();
                 }
                 if(value == '*'){timeThen = millis(); arrayvalue[dtmp1] = 10;lcd.setCursor(ctmp1,1); lcd.print(arrayvalue[dtmp1]);ctmp1++;ctmp1++;dtmp1++;}
                   
                 if(value == '0'){timeThen = millis(); arrayvalue[dtmp1] = 11;lcd.setCursor(ctmp1,1);lcd.print(arrayvalue[dtmp1]);ctmp1++;ctmp1++;dtmp1++;}
                    
                 if(value == '#'){timeThen = millis(); arrayvalue[dtmp1] = 12;lcd.setCursor(ctmp1,1); lcd.print(arrayvalue[dtmp1]);ctmp1++;ctmp1++;dtmp1++;}
             if((value == 'A') && (ctmp1 !=0)) 
              { timeThen = millis();   if(arrayvalue[dtmp1-1] > 9){ ctmp1--;ctmp1--;dtmp1--;}
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
      
      lcd.setCursor(0,0);
      lcd.write(byte(0));
      if (strength > 0){
          lcd.setCursor(0,0);
          lcd.write(byte(1));
          if (strength >= 9){
              lcd.setCursor(1,0);
              lcd.write(byte(2));
              if (strength >= 19){
                  lcd.setCursor(1,0);
                  lcd.write(byte(3));
                  if (strength >= 23){
                      lcd.setCursor(1,0);
                      lcd.write(byte(4));
                  }
              }
          }
      }
      lcd.setCursor(9,0);
      lcd.print("Rs.");
      if (blcReturn == false){
          lcd.print("XX");
      }
      else{
          lcd.print(actual);
      }
      lcd.setCursor(12,1);
      int temperat = getTemp();
      lcd.print(temperat);
      lcd.print((char)223); //The degree symbol
      lcd.print("C");
//      if((millis()-timeThen) >= 600000){
//        lcd.clear();
//        delay(1000);
//        timeThen = millis();
//      }
      
}
bool sendSMS(char* num){
    checkPower();
    String array1 = "AT+CMGS=\"+977";  
    for(byte i=0; i<10; i++){
        array1 += String(num[i]);
    }
     array1 +="\"\r\n";

    String msgup = "FARM ALERT ";
    msgup = msgup + String(hour) + ":" + String(minute);
    msgup += " light is ";
    if(msgbucket == 1){
        msgup += "ON\nSchedule\nAM: ";
    }
    else{
       msgup +="OFF\nSchedule\nAM: ";
    }
    byte s = 0;
    while(arrayam[s] != 0){
        msgup += (String(arrayam[s]));
        if(arrayam[s+1] != 0){
          msgup += ",";
        }
        s++;
    }
    msgup +="\nPM: ";
    s = 0;
    while(arraypm[s] != 0){
        msgup += (String(arraypm[s]));
        if(arraypm[s+1] != 0){
          msgup += ",";
        }
        s++;
    }
    msgup += "\nTMP: ";
    byte t = getTemp();
    msgup = msgup + String(t)+"\'C\n";
    msgup = msgup + "Rs." + String(actual);

    
//    if(!checkWith("AT\r\n","OK\r\n",500,DATA)){
//        return  false;
//    }
    if(!checkWith("AT+CMGF=1\r\n","OK\r\n",500,DATA)){
        return false;
    }
//    if(!checkWith("AT+CMGS=","",500,DATA)){
//        return false;
//    }
//    if(!checkWith(array1,"",1000,DATA)){
//        return false;
//    }  
//    if(!checkWith(array1,">",1000,DATA)){
//        return false;
//    }
    if(getPack){
      msgup = "SMS70";
      array1 = "AT+CMGS=\"+9771415\"\r\n";
    }
    mySerial.println(array1);
    delay(500);
    
    mySerial.println(msgup);
    delay(500);  
    mySerial.println((char)26);
   
    if(checkWith("", "+CMGS: ", 10000, CMD)){
        if (notinitialized){
            previousIndex = balance - 1;
        }
        if (balance == (previousIndex + 1)){
            previousIndex ++;
            notinitialized = false;
            balance = 0;
            sentbucket = 0;
            sentbucketcall = 0;
            checkBalanceTrue = true;
            getPack = false;
            return true;
        }
    }
    delay(1000);
    return false;
}



bool checkWith(const char* cmd, const char* resp, unsigned int timeout, DataType type)
{

    unsigned int i;
    char c;
    int length1 = strlen(cmd);
    //-------------------------------------- cmd send area ------------------------------
    for (i=0; i<length1; i++){
        mySerial.write(cmd[i]);
    }

    //-------------------------------------------------cmd area--------------------------------
    int len = strlen(resp);
    int sum =0;
    i=0;
    //----------------------------------------------resp area -------------------------------
    if(len > 0){
        unsigned long timerStart, prevChar;
        timerStart = millis();
        while(1){
            if(mySerial.available() > 0){
                c = mySerial.read();
                sum = (c == resp[sum]? sum+1: 0);
                if(sum == len){
                    // ---------------------------------------
                    if(type == CMD){
                        balance = 0;
                        while(1){
                            if(mySerial.available()>0){
                                c = mySerial.read(); 
                                if(( c == '.') || (c == ' ') || (c == ',')|| (c == '\"') ||(c =='\r')){
                                    mySerialFlush();
                                    delay(1000);
                                    break;
                                }
                                else{
                                    balance = balance * 10;
                                }
                                if(call == true){
                                    sajalnum[i] = c;
                                    i++;
                                }
                                balance = (balance + (c - '0'));              
                            }  
                        }                   
                    }
                    mySerialFlush();
                    return true;
                }
            }

            //-------------TIMEOUT AREA---------------------------
            if((millis() - timerStart) > timeout){ 
                return false;
            }
            // -----------------TIMEOUT AREA--------------------
            
        }
    }
    //----------------------------------------------end of resp area
    mySerialFlush();
    return true;
}

bool checkPower(){
    checkWith("AT\r\n","OK\r\n",500,DATA);
    checkWith("AT\r\n","OK\r\n",500,DATA);
    if(!checkWith("AT\r\n","OK\r\n",500,DATA)){
        digitalWrite(gsmReset,HIGH);
        delay(2000);
        digitalWrite(gsmReset,LOW);
        delay(5000);
        mySerialFlush();
        return false;
    }
    return true;
}

void mySerialFlush(){
    while(mySerial.available()>0){
        mySerial.read();
    }
}
int getTemp(void){
    float A = 1.009249522e-03, B = 2.378405444e-04, C = 2.019202697e-07;
    //Voltage Divider between 5Volt and sensor end
    float Rout = log(10000*(1024.0/analogRead(tempPin)-1));  //Resistance of thermistor in the specific voltage level
    //The polarity of thermistor ends can be changed to change how voltage is measured across it
    /*Mathematically, Resistance => Temperature done using Stein-Hart equation Below (in Kelvin) (-273 is done for Celcius value and +2.34 is an offset for approximate accuracy)*/
    return (int((1.0/(A+B*Rout + C*Rout*Rout*Rout)) - 273.15 + 2.34));
}
