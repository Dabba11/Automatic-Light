//ST7920 128*64 display model 12864B V2.0 with library U8g2lib
//Connections Required (I2C SPI mode):
//1 = GND == GND
//2 = +5 volts == +5v
//3 = (Vo) Contrast in == Wiper (middle pin) of contrast pot
//4 = RS acts as CS pin == 6
//5 = R_W acts as Serial Data SID pin == 4
//6 = EN  acts as Serial Clock SCLK pin == 2
//15 = PSB set to 0 by Ground == GND
//18 = (Vee/Vout) Contrast == 1 leg of Contrast pot
//19 = Backlight == Anode (one resistor end) == 220ohm R == 5V
//20 = Backlight == Cathode (other resistor end) == GND
//Contrast Pot not required, built in pot for 12864B V2.0
#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif
#include <EEPROM.h>
#include <Keypad.h>
#include "Wire.h"
/*Initialize for the display*/
U8G2_ST7920_128X64_1_SW_SPI u8g2(U8G2_R0, 3, 5, 2); //u8g2(U8G2_R0, En, Rw, Rs) (2, 4, 6);
//MicroController mapping, u8g2(U8G2_R0, 3, 5, 2)


#define DS3231_I2C_ADDRESS 0x68
#define relayPin A4
#define buzzerPin A3

int flag99 = 0;
bool change;
unsigned long timeThen;
unsigned long timeNow;
const byte numRows   = 4;
const byte numCols   = 4;
byte flag1=0,flag2=0;
byte i;
int rtcdelaytime;
int arrayam[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
int arraypm[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
int arrayam1[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
int arraypm1[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
char keymap[numRows][numCols]= { 
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'} };

byte rowPins[numRows] = {39,41,43,45}; //Rows 0 to 3
byte colPins[numCols]= {31,33,35,37}; //Columns 0 to 3
Keypad myKeypad= Keypad(makeKeymap(keymap), rowPins, colPins, numRows, numCols);

byte second, minute, hour;



void setup(void) {
    pinMode(4, OUTPUT);
    pinMode(relayPin,OUTPUT);
    pinMode(buzzerPin,OUTPUT);
    Wire.begin();
    digitalWrite(4, HIGH);
    u8g2.begin();                                             /*Initialize u8g2*/
    u8g2.setFontPosBottom();                                  /*It is (bottom - 2) by default!! Set the cursor at the bottom left of any character*/
    myKeypad.setHoldTime(80000);
    myKeypad.setDebounceTime(200);
    myKeypad.addEventListener(keypadEvent);
    u8g2.clearDisplay(); 
    /*  NOTE :::: u8g2.sendBuffer() should always be called before a delay is applied or else ONLY buffer will be written with delay, display will be all at once*/
    delay(3500);
    
    /*Single font for all the displayed characters*/
    u8g2.setFont(u8g2_font_bitcasual_tr);
    
    /*Intro Screen*/
    do{
    u8g2.drawStr(37, 40, "LOADING...");
    }while(u8g2.nextPage());
    digitalWrite(relayPin,LOW);
    delay(3000);

    u8g2.clearDisplay();

    byte firstdigit  = EEPROM.read(1);
    byte seconddigit = EEPROM.read(2);
    byte j =0;
    for(i = 3;i <firstdigit+3; i++){
        arrayam[i-3] = EEPROM.read(i);           
    }
    while(j < seconddigit){
        arraypm[j] = EEPROM.read(i);
        j++; i++;                 
    }
    
    delay(2000);
    digitalWrite(buzzerPin,HIGH);
    delay(1500);
    digitalWrite(buzzerPin,LOW);
    rtcdelaytime = millis();
}

void(* resetFunc) (void) = 0; //declare reset function at address 0

void loop(void) {
    /*firstpage()/nextpage() taes less progmem than using setCurrTileRow() for a large number of instructions*/
    /*firstPage() automatically clears the page buffer and the screen at the beginning of each loop (or anytime it is called!!)*/
    u8g2.firstPage();
    do{
        firstpagedisplay();
        displaytime();
        tower();
        toShowSchedule();
    }while(u8g2.nextPage());
    
    timeThen = millis();
    if((millis() - rtcdelaytime) >= 60000){
        displaytime(); // display the real-time clock data on the Serial Monitor,
        rtcdelaytime = millis();
    }
    delay(500); 
    
    //--------------- switch section -----------//
    byte flag3 = 0, flag4 = 0;
    if(hour <12){
        for(i=0; i<12; i++){
            if(((arrayam[i] == hour) && (arrayam[i] !=0)) || ((12-arrayam[i] == hour) && (arrayam[i] == 12))){
                digitalWrite(relayPin,HIGH);
                delay(1000);
                flag3 = 1;
                if(flag1 == 0){
                    digitalWrite(buzzerPin,HIGH);
                    //turning on SendMessage();
                    digitalWrite(buzzerPin,LOW);
                    delay(2000);
                    flag1=1;
                    delay(2000);
                }
            }
        }
        if(flag3 == 0){
            digitalWrite(relayPin,LOW);
            if(flag1 = 1){
                digitalWrite(buzzerPin,HIGH);
                //turning off message
                flag1 = 0;
                digitalWrite(buzzerPin,LOW);
                delay(2000);
            }
        }
    }
    if(hour >11){
        for(i=0; i<12; i++){
            if((((arraypm[i] +12 == hour) || (arraypm[i] == hour))) &&(arraypm[i] !=0)){
                digitalWrite(relayPin,HIGH);
                delay(2000);
                flag4 = 1;
                if(flag2 == 0){
                    digitalWrite(buzzerPin,HIGH);
                    //turning on SendMessage();
                    digitalWrite(buzzerPin,LOW);
                    delay(2000);
                    flag2=1;
                }
            }
        }
        if(flag4 == 0){
            digitalWrite(relayPin,LOW);
            if (flag2 == 1){
                digitalWrite(buzzerPin,HIGH);
                //turning off message
                flag2 = 0;
                digitalWrite(buzzerPin,LOW);
                delay(2000);
            }
        }
    }

    byte len1;
    byte len2;
    byte k=12;
    byte j=0;
    char valueloop = getButton();
    delay(500);
    /*------------LIGHT SCHEDULE SETTING--------------*/  
    if(valueloop == 'C'){
        len1 = inputvalue("AM :", arrayam1);
        delay(1000);
        len2 = inputvalue("PM :", arraypm1);
        for(i=len1; i <12; i++){ arrayam1[len1] = 0; len1++; }
        for(i=len2; i <12; i++){ arraypm1[len2] = 0; len2++; }

        /*to clear below smart farm only, setCurrTileRow() needs a loop taking more memory than firstpage()-nextpage()*/
        u8g2.firstPage();
        do{
            firstpagedisplay();
            displaytime();
            tower();
            u8g2.drawStr(48, 39, "-AM-");
        }while(u8g2.nextPage());
        delay(2000);
        /*until the AM schedule is fully shown, no firstpage()/nextpage() loop allowed or else all other part of display will be cleared by firstpage()*/
        j = 0;
        /*using setBufferCurrTileRow() - write only in one tile, if cursor is set beyond this tile, the parts of characters that donot reach this tile are not displayed*/
        for (i = 5; j < 12; i = i+8){
            u8g2.setBufferCurrTileRow(0); /*Imagine you are drawing on the first Tile of display (i.e. first 8 rows of pixels)*/
            u8g2.setCursor(i, 10);        /*The first tile of display is a PAGE of 128 byte of U8G2lib*/
            u8g2.print(arrayam1[j]);      /*Each tile represents a page in the buffer. for 64dot height display, 8 tiles from 0 to 7*/
            u8g2.setBufferCurrTileRow(6); /*Shifting the tile from 0(first) to 6(seventh) poisition*/
            u8g2.sendBuffer();            /*All of above was done in buffer, now goes to display*/
            if (arrayam1[j] > 9){
                i = i + 9;
           }
           j++;
           delay(200);
        }
        delay(3000);

        /*same for PM schedule*/
        u8g2.firstPage();
        do{
            firstpagedisplay();
            displaytime();
            tower();
            u8g2.drawStr(48, 39, "-PM-");
        }while(u8g2.nextPage());
        delay(2000);
        j = 0;
        for (i = 5; j < 12; i = i+8){
            u8g2.setBufferCurrTileRow(0);
            u8g2.setCursor(i, 10);
            u8g2.print(arraypm1[j]);
            u8g2.setBufferCurrTileRow(6);
            u8g2.sendBuffer();
            if (arraypm1[j] > 9){
                i = i + 9;
            }
            j++;
            delay(200);
        }
        delay(4000);
        u8g2.firstPage();
        do{
            u8g2.drawStr(10, 21, "SAVE SETTING?");
            u8g2.drawStr(4, 37, "D - Yes");
            u8g2.drawStr(6, 51, "A - No");
        }while(u8g2.nextPage());
        do{
            valueloop = getButton();
            if (valueloop == 'A'){
                goto exittime1;
            }
            if ((millis() - timeThen) >= 100000){
                timeThen = millis();
                goto exittime1;
            }
        }while(valueloop != 'D');
        updateEeprom(arrayam1,arraypm1); 
        copy(arrayam1,arrayam,len1);
        copy(arraypm1,arraypm,len2);
        exittime1:
            u8g2.clearDisplay(); /*Because delay() cannot be invoked after firstPage() is called*/
            delay(3000); 
            u8g2.firstPage();
            do{
                firstpagedisplay();
                displaytime();
                tower();
                toShowSchedule();
            }while(u8g2.nextPage());
            delay(1000);
    }
    
    /*-------------CLOCK SETTING-------------*/
    if(valueloop == 'B'){
        u8g2.clearDisplay();
        char valueup;
        byte hour1, hour2;
        byte minute1, minute2;
        u8g2.firstPage();
        do{
            firstpagedisplay();
            displaytime();
            tower();
            setpixels(0, 27, 120, 1);
            u8g2.drawStr(2, 37, "Hr");
            u8g2.drawVLine(35, 26, 23);
            u8g2.drawStr(40, 37, "Min");
            u8g2.drawVLine(93, 26, 23);
            u8g2.drawStr(95, 37, "A/PM");
            u8g2.drawHLine(0, 37, 128);
            u8g2.drawHLine(0, 25, 128);
            u8g2.drawHLine(0, 49, 128);
            u8g2.drawStr(1, 66, "D= Next, A= Delete");
        }while(u8g2.nextPage());
        /*No clearDisplay() or firstPage()/nextPage() at all cost after this until time is completely set*/
        gotohour:
          u8g2.clearBuffer();
          u8g2.setBufferCurrTileRow(0);
          u8g2.drawStr(4, 10, ">");
          u8g2.setBufferCurrTileRow(5);
          u8g2.sendBuffer();
          /*no clearBuffer() until the time has been set or else the > sign will be erased*/
          byte ctmp1 = 14;
          timeThen = millis();
          
          //------HOUR SETUP------//
          do{
              valueup = getButton();
              if((valueup != NO_KEY) && (valueup != 'B')&& (valueup != 'C')&& (valueup != 'A')&& (valueup != 'D')&& (valueup != '*')&& (valueup != '0')&& (valueup != '#')&&(ctmp1 < 22)){
                  u8g2.setBufferCurrTileRow(0);
                  u8g2.setCursor(ctmp1, 10);
                  u8g2.print(valueup);
                  u8g2.setBufferCurrTileRow(5);
                  u8g2.sendBuffer();
                  hour1 = valueup -'0';
                  if(ctmp1 == 20){
                      hour2 = hour2 * 10;
                      hour2 += hour1;
                      if( hour2 > 12){
                          setpixels(14, 38, 14, 10);
                          u8g2.sendBuffer();
                          hour2 = 0;
                          goto gotohour;
                      } 
                  }
                  if(ctmp1 == 14){
                      hour2 = hour1;
                  }
                  ctmp1 += 6;
              }
              if(ctmp1 < 20){
                  if(valueup == '*'){
                      u8g2.setBufferCurrTileRow(0);
                      u8g2.drawStr(ctmp1, 10, "10");
                      u8g2.setBufferCurrTileRow(5);
                      u8g2.sendBuffer();
                      hour2 = 10;
                      goto check;
                  }
                  if(valueup == '0'){
                    u8g2.setBufferCurrTileRow(0);
                    u8g2.drawStr(ctmp1, 10, "11");
                    u8g2.setBufferCurrTileRow(5);
                    u8g2.sendBuffer();
                      hour2 = 11;
                      goto check;
                  }
                  if(valueup == '#'){
                      u8g2.setBufferCurrTileRow(0);
                      u8g2.drawStr(ctmp1, 10, "12");
                      u8g2.setBufferCurrTileRow(5);
                      u8g2.sendBuffer();
                      hour2 = 12;
                      goto check;
                  }
                  
              }
              if((valueup == 'A') && (ctmp1 != 14)){ 
                  if(hour2 > 9){
                      ctmp1 -= 12;
                      setpixels(ctmp1, 38, 13, 11);
                      u8g2.sendBuffer();
                      hour2 = 0;
                  }
                  else{
                      ctmp1 -= 6;
                      setpixels(ctmp1, 38, 13, 11);
                      u8g2.sendBuffer();
                      hour2 = 0;
                      goto gotohour;
                  }
              }
              if ((millis() - timeThen) >= 120000){
                  timeThen = millis();
                  goto gotominute;
              }
          }while(valueup != 'D');
          
        //------MINUTE SETUP-------//
        timeThen = millis();
        gotominute:
          ctmp1=61;

          check:
              if((valueup == '*')|| (valueup =='0')||(valueup == '#')){
                  do{
                      valueup = getButton();
                      if (valueup == 'D'){
                          goto gotominute;
                      }
                      if (valueup == 'A'){
                          setpixels(14, 38, 15, 10);
                          u8g2.sendBuffer();
                          goto gotohour;
                      }
                      if ((millis() - timeThen) >= 90000){
                          timeThen = millis();
                          goto exittime;
                      }   
                  }while(1);
              }
          /*since we cannot use clearBuffer() to erase > on hour side and rewrite on the minute side without erasing everything in that row, I used setpixels() function*/    
          setpixels(4, 38, 8, 10);
          u8g2.sendBuffer();
          u8g2.setBufferCurrTileRow(0);
          u8g2.drawStr(51, 10, ">");
          u8g2.setBufferCurrTileRow(5);
          u8g2.sendBuffer();
          
          do{
              valueup = getButton();
              if((valueup != NO_KEY) && (valueup != 'B')&& (valueup != 'C')&& (valueup != 'A')&& (valueup != 'D')&& (valueup != '*')&& (valueup != '0')&& (valueup != '#')){
                  u8g2.setBufferCurrTileRow(0);
                  u8g2.setCursor(ctmp1, 10);
                  u8g2.print(valueup);
                  u8g2.setBufferCurrTileRow(5);
                  u8g2.sendBuffer();
                  minute1 = valueup - '0';
                  if(ctmp1 == 68){
                      minute2 = minute2 * 10;
                      minute2 = minute2 + minute1;
                      if(minute2 > 59){
                          setpixels(61, 38, 14, 10);
                          u8g2.sendBuffer();
                          goto gotominute;
                      }
                  }
                  if(ctmp1 == 61){
                      minute2 = minute1;
                  }
                  ctmp1 += 7;
              }
//              if(ctmp1 < 64){
//                  if(valueup == '*'){
//                      u8g2.setBufferCurrTileRow(0);
//                      u8g2.drawStr(ctmp1, 10, "10");
//                      u8g2.setBufferCurrTileRow(5);
//                      u8g2.sendBuffer();
//                      minute2 = 10;
//                      goto check2;
//                  }
//                  if(valueup == '0'){
//                      u8g2.setBufferCurrTileRow(0);
//                      u8g2.drawStr(ctmp1, 10, "11");
//                      u8g2.setBufferCurrTileRow(5);
//                      u8g2.sendBuffer();
//                      minute2 = 11;
//                      goto check2;
//                  }
//                  if(valueup == '#'){
//                      u8g2.setBufferCurrTileRow(0);
//                      u8g2.drawStr(ctmp1, 10, "10");
//                      u8g2.setBufferCurrTileRow(5);
//                      u8g2.sendBuffer();
//                      minute2 = 12;
//                      goto check2;
//                  }
//              }
              if(valueup == 'A'){
                  if(minute2 > 9){
                      ctmp1 -= 14;
                      minute2=0;
                      setpixels(ctmp1, 38, 15,10);
                      u8g2.sendBuffer(); 
                  }
                  else{
                      ctmp1 -= 7;
                      minute2 = 0;
                      setpixels(ctmp1, 38, 15,10);
                      u8g2.sendBuffer(); 
                      goto gotominute;
                  }
              }
              if ((millis() - timeThen) >= 90000){
                  timeThen = millis();
                  break;
              }                
          }while(valueup != 'D');

          //---AM\PM Change---//
          timeThen = millis();
        gotochange:
          bool btmp1 = false;
          char chartmp;
          byte hourtmp2=hour2;

          check2:
          /*to rewrite the instructions correctly for AM/PM change clearBuffer() used, but have to rewrite the hour and minute already setup*/
          u8g2.clearBuffer();
          u8g2.setBufferCurrTileRow(0);
          u8g2.drawStr(1, 10, "C= Change,D= Done");
          u8g2.setBufferCurrTileRow(7);
          u8g2.sendBuffer();
          u8g2.clearBuffer(); /*so that C= change part donot appear in time setting tile (Sequence is important!!!!)*/
          u8g2.setBufferCurrTileRow(0);
          u8g2.setCursor(14, 10);
          u8g2.print(hour2);
          u8g2.setCursor(61, 10);
          u8g2.print(minute2);
          u8g2.drawStr(96, 10, "> AM");
          u8g2.setBufferCurrTileRow(5);
          u8g2.sendBuffer();
          
          if((valueup == '*')|| (valueup =='0')||(valueup == '#')){
              do{
                  valueup = getButton();
                  
                  if (valueup == 'D'){
                      goto gotochange;
                  }
                  if (valueup == 'A'){
                      setpixels(61, 38, 15, 10);
                      u8g2.sendBuffer();
                      goto gotominute;
                  }
                  if ((millis() - timeThen) >= 90000){
                      timeThen = millis();
                      goto exittime;
                  }   
              }while(1);
          }
          change = false;
          
          do{
              valueup = getButton();
              if(valueup == 'C'){
                  if(btmp1 == true){
                      u8g2.setBufferCurrTileRow(0);
                      setpixels(96, 0, 32, 10);
                      u8g2.drawStr(96, 10, "> AM");
                      u8g2.setBufferCurrTileRow(5);
                      u8g2.sendBuffer();
                      change = false;
                  }
                  if(btmp1 == false){
                      u8g2.setBufferCurrTileRow(0);
                      setpixels(96, 0, 31, 10);
                      u8g2.drawStr(96, 10, "> PM");
                      u8g2.setBufferCurrTileRow(5);
                      u8g2.sendBuffer();
                      if(hour2 != 12){
                          hourtmp2 = hour2+12;
                      } 
                      else{
                          hourtmp2 = 12;
                      } 
                      change =true;
                  }
                  btmp1 = !btmp1;
              }
              if((change == false)&&(hour2 == 12)){
                  hourtmp2 =0;
              }
              if ((millis() - timeThen) >= 60000){
                  timeThen = millis();
                  break;
              }
          }while(valueup != 'D');

          u8g2.clearDisplay();
          delay(3000);
          u8g2.firstPage();
          do{
          u8g2.drawStr(10, 21, "SAVE SETTING?");
          u8g2.drawStr(4, 37, "D - Yes");
          u8g2.drawStr(6, 51, "A - No");
          }while(u8g2.nextPage());
          
          do{
              valueloop = getButton();
              if(valueloop == 'A'){
                  goto exittime;
              }
              if ((millis() - timeThen) >= 80000){
                  timeThen = millis();
                  goto exittime;
              }
          }while(valueloop != 'D');
          hour2 = hourtmp2;
          setDS3231time(00,minute2,hour2);   
          exittime:
          u8g2.clearDisplay();
          delay(3000);
            readDS3231time(&second, &minute, &hour);
          u8g2.firstPage();
          do{
             firstpagedisplay();
             displaytime();
             tower();
             toShowSchedule();
          }while(u8g2.nextPage());
      }
    delay(3000);
}

// Convert normal decimal numbers to binary coded decimal
byte decToBcd(byte val){return( (val/10*16) + (val%10) );}
// Convert binary coded decimal to normal decimal numbers
byte bcdToDec(byte val){return( (val/16*10) + (val%16) );}




void keypadEvent(KeypadEvent valueloop){
    if(myKeypad.getState() == HOLD){
        if( valueloop == 'A'){
            digitalWrite(buzzerPin,HIGH);
            u8g2.firstPage();
            do{
                u8g2.drawStr(10, 26, "RESETTING...");
            }while(u8g2.nextPage());
            for(i=0; i< EEPROM.length(); i++){
                EEPROM.write(i,0);
            }
            delay(3000);
            digitalWrite(buzzerPin,LOW);
            
            u8g2.clearDisplay();
            delay(2000);
            resetFunc();
        }            
    }
}

void firstpagedisplay(){
    unsigned int balance;
    readDS3231time(&second, &minute, &hour);
    //Smart farm
    u8g2.drawStr(21, 24, "dabba FARM");
    u8g2.setCursor(78, 11);
    u8g2.print("Rs. ");
    u8g2.print(balance);
    u8g2.drawHLine(0, 27, 128);
}

void displaytime(){
    // retrieve data from DS3231
    readDS3231time(&second, &minute, &hour);
    u8g2.setCursor(22, 11);
    if (hour < 10){
      u8g2.setCursor(29, 11);
    }
    setpixels(20, 0, 14, 10);
    u8g2.print(hour);
    u8g2.print(" : ");
    setpixels(43, 0, 30, 10);
    if (minute < 10){
      u8g2.print("0");
    }
    u8g2.print(minute);
}

byte inputvalue(String display1, int arrayvalue[]){
    byte ctmp1 = 14;
    byte dtmp1 = 0;
    byte len;
    char value;
    for(i = 0; i<4; i++){
    u8g2.setBufferCurrTileRow(0);
    setpixels(0, 0, 128, 10); //clearing space below SMART FARM
    u8g2.setBufferCurrTileRow(4+i);
    u8g2.sendBuffer();
    }
    delay(1000);
    u8g2.setBufferCurrTileRow(0);
    u8g2.setCursor(0, 10);
    u8g2.print(display1);
    u8g2.setBufferCurrTileRow(4);
    u8g2.sendBuffer();
    u8g2.setBufferCurrTileRow(0);
    u8g2.clearBuffer();
    u8g2.drawStr(5, 10, "D = OK, A = delete");
    u8g2.setBufferCurrTileRow(7);
    u8g2.sendBuffer();
    u8g2.clearBuffer();
    u8g2.setBufferCurrTileRow(0);
    u8g2.drawStr(0, 10, ">>");
    u8g2.setBufferCurrTileRow(5);
    u8g2.sendBuffer();
    do{
        value = getButton();
        if ((millis() - timeThen) >= 120000){
            flag99 = 13;
            timeThen = millis();
            return flag99;
            break;
        }
        if((value != NO_KEY) && (value != 'B')&& (value != 'C')&&(value != 'A')&& (value != 'D') && (value != '*')&& (value != '0')&& (value != '#')){
            arrayvalue[dtmp1] = value -'0';
            u8g2.setBufferCurrTileRow(0);
            u8g2.setCursor(ctmp1, 10);
            u8g2.print(arrayvalue[dtmp1]);
            u8g2.setBufferCurrTileRow(5);
            u8g2.sendBuffer();
            dtmp1++;
            ctmp1 += 7;
        }
        if(value == '*'){
            arrayvalue[dtmp1] = 10;
            u8g2.setBufferCurrTileRow(0);
            u8g2.setCursor(ctmp1+1,10);
            u8g2.print("10");
            u8g2.setBufferCurrTileRow(5);
            u8g2.sendBuffer();
            ctmp1 += 15; dtmp1++;
        }
        if(value == '0'){
            arrayvalue[dtmp1] = 11;
            u8g2.setBufferCurrTileRow(0);
            u8g2.setCursor(ctmp1+1, 10);
            u8g2.print(arrayvalue[dtmp1]);
            u8g2.setBufferCurrTileRow(5);
            u8g2.sendBuffer();
            ctmp1 += 15; dtmp1++;
        }
        if(value == '#'){
            arrayvalue[dtmp1] = 12;
            u8g2.setBufferCurrTileRow(0);
            u8g2.setCursor(ctmp1+1, 10);
            u8g2.print(arrayvalue[dtmp1]);
            u8g2.setBufferCurrTileRow(5);
            u8g2.sendBuffer();
            ctmp1 += 15; dtmp1++;
        }
        if((value == 'A') && (ctmp1 != 14)){ //In case user wants to erase previously entered value
            if(arrayvalue[dtmp1-1] > 9){
                ctmp1 -= 15; dtmp1--;
                setpixels(ctmp1, 39, 20, 10); //erasing desired value
                u8g2.sendBuffer();
            }
            else{
                ctmp1 -= 7; dtmp1--;
                setpixels(ctmp1, 39, 20, 10);
                u8g2.sendBuffer();
            }    
            arrayvalue[dtmp1]=0;
        } 
        if(dtmp1 >= 12){
            break;
        }    
    }while(value != 'D');
    return dtmp1;
}



void setDS3231time(byte second, byte minute, byte hour){
    // sets time and date data to DS3231
    Wire.beginTransmission(DS3231_I2C_ADDRESS);
    Wire.write(0); // set next input to start at the seconds register
    Wire.write(decToBcd(second)); // set seconds
    Wire.write(decToBcd(minute)); // set minutes
    Wire.write(decToBcd(hour)); // set hours
    Wire.endTransmission();
     
}

void readDS3231time(byte *second,byte *minute,byte *hour)
{
    Wire.beginTransmission(DS3231_I2C_ADDRESS);
    Wire.write(0); // set DS3231 register pointer to 00h
    Wire.endTransmission();
    Wire.requestFrom(DS3231_I2C_ADDRESS, 7);
    // request seven bytes of data from DS3231 starting from register 00h
    *second = bcdToDec(Wire.read() & 0x7f);
    *minute = bcdToDec(Wire.read());
    *hour = bcdToDec(Wire.read() & 0x3f);
}


char getButton(){
    char keypressed = myKeypad.getKey();
    if((keypressed != NO_KEY) &&(myKeypad.getState()== PRESSED)){
        return keypressed;
    } 
    return NO_KEY;
}

void copy(int src[], int dst[], int len){
    for(i=0;i < 12;i++){
        dst[i] = src[i];
    }
}

void updateEeprom(int arrayam[12],int arraypm[12]){ 
    byte j=0, k=1, i=2;
    while((arrayam[i-2] != 0) && (k<13)){
        EEPROM.write(i+1,arrayam[i-2]); i++; k++;
    }
    byte m = i;
    EEPROM.write(1,m-2);
    k=1;
    while((arraypm[j] != 0) && (k<13)){
        EEPROM.write(i+1,arraypm[j]); i++; j++; k++;
    }    
    EEPROM.write(2,i-m);
}

//clear certain portion of screen (int horizontal_pixel_pos, int vertical_pixel_pos, int width, int height (of portion to be erased))
void setpixels(int a, int b, int c, int d){
          for (int j=0; j<c; j++){
          for(int k=0; k<d; k++){
            u8g2.setDrawColor(0);   //0 to clear pixel i.e black color
            u8g2.drawPixel(a+j,b+k);//in this position
          }
        }
        u8g2.setDrawColor(1); //1 in order to write with white color afterwards again
}

void toShowSchedule(){
    byte firstdigit  = EEPROM.read(1);
    byte seconddigit = EEPROM.read(2);
    u8g2.drawStr(0, 49, "A:");
    showSchedule(arrayam, firstdigit, 49);
    u8g2.drawStr(0, 62, "P:");
    showSchedule(arraypm, seconddigit, 62);
}

void showSchedule(int arrays[], int len, int pxht){
    int j = 14;
    //setpixels(14, pxht-11, 128, 12);
    /*Showing Schedule*/
    for (i=0; i<len; i++){
        if(arrays[i] != 0){
            u8g2.setCursor(j, pxht+1);
            u8g2.print(arrays[i]);
            j += 8;
            if(arrays[i]>9){
                j += 7;
            }
            /*Showing , (comma) after each number except the last non-zero in the list*/
            if(arrays[i+1] != 0){
              u8g2.drawVLine(j-2, pxht-2, 2);
            }
        }
    }
}

void tower(){
    //For tower signal
    u8g2.drawLine(0, 0, 6, 0);
    u8g2.drawLine(0, 0, 3, 3);
    u8g2.drawLine(6, 0, 3, 3);
    //if(GSM signal values){}
    u8g2.drawLine(3, 0, 3, 9);
    u8g2.drawLine(6, 6, 6, 9);
    u8g2.drawLine(9, 4, 9, 9);
    u8g2.drawLine(12, 2, 12, 9);
    u8g2.drawLine(15, 0, 15, 9);
}
