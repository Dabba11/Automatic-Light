//ST7920 128*64 display model 12864B V2.0 with library U8g2lib
//Connections Required (I2C SPI mode):
//1 = GND == GND
//2 = +5 volts == +5v
//3 = (Vo) Contrast in == Wiper (middle pin) of contrast pot
//4 = RS acts as CS pin ==2
//5 = R_W acts as Serial Data SID pin == 4
//6 = EN  acts as Serial Clock SCLK pin == 6
//15 = PSB set to 0 by Ground == GND
//18 = (Vee/Vout) Contrast == 1 leg of Contrast pot
//19 = Backlight == Anode (one resistor end) == 220ohm R == 5V
//20 = Backlight == Cathode (other resistor end) == GND

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
U8G2_ST7920_128X64_F_SW_SPI u8g2(U8G2_R0, 2, 4, 6); //u8g2(U8G2_R0, En, Rw, Rs)


#define DS3231_I2C_ADDRESS 0x68
#define DS3231_TEMPERATURE_ADDR 0x11
#define relayPin 7
//#define relayPin 2
#define potPin A2
#define buzzerPin 5

int flag99 = 0;
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

byte rowPins[numRows] = {39,41,43,45}; //Rows 0 to 3
byte colPins[numCols]= {31,33,35,37}; //Columns 0 to 3
Keypad myKeypad= Keypad(makeKeymap(keymap), rowPins, colPins, numRows, numCols);

byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;

float balance;

void setup(void) {
    
    Wire.begin();
    u8g2.begin();
    u8g2.setFontPosTop();
    myKeypad.setHoldTime(80000);
    myKeypad.setDebounceTime(200);
    myKeypad.addEventListener(keypadEvent);
    u8g2.clearBuffer(); 
    u8g2.setFont(u8g2_font_ncenB18_te);
    /*  NOTE :::: u8g2.sendBuffer() should always be called before a delay is applied or else only buffer will be written with delay, display will be all at once*/
    u8g2.drawStr(17, 5 ,"d");
    u8g2.drawStr(37, 5 ,"a");
    u8g2.drawStr(57, 5 ,"b");
    u8g2.drawStr(77, 5 ,"b");
    u8g2.drawStr(97, 5 ,"a");
    u8g2.sendBuffer();
    delay(500);
    u8g2.setFont(u8g2_font_5x7_tf);
    u8g2.drawStr(17, 24, "everything's inside");
    u8g2.sendBuffer();
    delay(4000);
    u8g2.setFont(u8g2_font_chikita_tr);
    u8g2.drawStr(40, 34, "LOADING...");
    u8g2.sendBuffer();
    
    //Loading bar from here
    for(int j=0; j<10; j++){
        u8g2.drawFrame(19 + 9*j, 43, 8, 8);
    }
    u8g2.setFont(u8g2_font_press_mel_tr);
    u8g2.drawStr(45, 54, "0%");
    u8g2.sendBuffer();
    delay(1000);
    int i = 0;
    int count = 0;
    while(i < 88){
        u8g2.drawBox(19+i, 43, 8, 8);
        count = count + 10;
        setpixels(45,54, 40, 10);
        u8g2.setCursor(45,54);
        u8g2.print(count); 
        u8g2.print("%");
        u8g2.sendBuffer();
        i = i + 9;
        delay(100);
        if (i>35 && i<61){
          delay(400);
        }
    }
    //Loading bar complete
    
    pinMode(relayPin,OUTPUT);
    pinMode(buzzerPin,OUTPUT);
    digitalWrite(relayPin,LOW);
    delay(1000);
    u8g2.clearDisplay();
    delay(1000);

    byte firstdigit  = EEPROM.read(1);
    byte seconddigit = EEPROM.read(2);
    u8g2.setFont(u8g2_font_6x10_tf);
    u8g2.drawStr(8, 1, "Hours of lighting : ");
    u8g2.sendBuffer();
    delay(1000);
    u8g2.setCursor(15, 20);
    u8g2.setFont(u8g2_font_lucasarts_scumm_subtitle_o_tf);
    u8g2.print("1st - ");
    u8g2.sendBuffer();
    delay(500);
    //    if (firstdigit < 10){
    //        u8g2.print("0");
    //    }
    u8g2.print(firstdigit);
    u8g2.print(" hrs");
    u8g2.sendBuffer();
    delay(500);
    u8g2.setCursor(15, 40);
    u8g2.print("2nd - ");
    u8g2.sendBuffer();
    delay(500);
    u8g2.print(seconddigit);
    u8g2.print(" hrs");
    u8g2.sendBuffer();
    delay(1000);
    u8g2.clearDisplay();
    delay(1000);

    i=3;
    byte j =0;
    u8g2.setFont(u8g2_font_6x10_tf);
    u8g2.drawStr(8, 1, "Times of lighting : ");
    u8g2.sendBuffer();
    delay(1000);
    u8g2.setFont(u8g2_font_lucasarts_scumm_subtitle_o_tf);
    if(firstdigit == 0 && seconddigit == 0){
        u8g2.drawStr(26, 20, "NOT SET");
        u8g2.drawStr(35, 40, "YET !");
        u8g2.sendBuffer();
        delay(1500);
    }
    for(i = 3;i <firstdigit+3; i++){
        arrayam[i-3] = EEPROM.read(i);
        setpixels(32, 20, 90, 15); 
        u8g2.setCursor(32, 20); 
        if(arrayam[i-3]<10){
            u8g2.print("0");
        }
        u8g2.print(arrayam[i-3]);
        u8g2.print(" AM");
        u8g2.sendBuffer(); 
        delay(1000);                
    }
    delay(500);
    while(j < seconddigit){
        arraypm[j] = EEPROM.read(i);
        setpixels(32, 40, 90, 15); 
        u8g2.setCursor(32,40);
        if(arraypm[j]<10){
            u8g2.print("0");
        }
        u8g2.print(arraypm[j]);
        u8g2.print(" PM");
        u8g2.sendBuffer();
        j++; i++; 
        delay(1000);                
    }
    u8g2.clearDisplay();
    delay(1000);

    digitalWrite(buzzerPin,HIGH);
    delay(700);
    digitalWrite(buzzerPin,LOW);
    firstpagedisplay();
    rtcdelaytime = millis();
    displaytime();
    tower();
}

void(* resetFunc) (void) = 0; //declare reset function at address 0

void loop(void) {  
    timeThen = millis();
    if((millis() - rtcdelaytime) >= 60000){
        displaytime(); // display the real-time clock data on the Serial Monitor,
        rtcdelaytime = millis();
    }
    delay(500); 
    int tempC = DS3231_get_treg();  // Reads the temperature as an int, to save memory
    setpixels(93, 26, 20, 10);
    u8g2.setCursor(93,26);
    u8g2.setFont(u8g2_font_smart_patrol_nbp_tr);
    u8g2.print(tempC);
    u8g2.sendBuffer();
    tower();
    
    //--------------- switch section -----------//
    byte flag3 = 0, flag4 = 0;
    if(hour <12){
        for(byte i=0; i<12; i++){
            if(((arrayam[i] == hour) && (arrayam[i] !=0)) || ((12-arrayam[i] == hour) && (arrayam[i] == 12))){
                digitalWrite(relayPin,HIGH);
                delay(1000);
                flag3 = 1;
                if(flag1 == 0){
                    digitalWrite(buzzerPin,HIGH);
                    //turning on SendMessage();
                    setpixels(0, 38, 128, 30);
                    u8g2.setFont(u8g2_font_helvB08_tf);
                    u8g2.drawStr(12, 43, "Turning Light ON..");
                    u8g2.sendBuffer();
                    delay(2000);
                    digitalWrite(buzzerPin,LOW);
                    delay(2000);
                    setpixels(0, 38, 128, 30);
                    
                    u8g2.sendBuffer();
                    toShowSchedule();
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
                setpixels(0, 38, 128, 30);
                u8g2.setFont(u8g2_font_helvB08_tf);
                u8g2.drawStr(12, 43, "Turning Light OFF..");
                u8g2.sendBuffer();
                delay(2000);
                flag1 = 0;
                digitalWrite(buzzerPin,LOW);
                delay(2000);
                setpixels(0, 38, 128, 30);
                u8g2.sendBuffer();
                toShowSchedule();
            }
        }
    }
    if(hour >11){
        for(byte i=0; i<12; i++){
            if((((arraypm[i] +12 == hour) || (arraypm[i] == hour))) &&(arraypm[i] !=0)){
                digitalWrite(relayPin,HIGH);
                delay(2000);
                flag4 = 1;
                if(flag2 == 0){
                    digitalWrite(buzzerPin,HIGH);
                    //turning on SendMessage();
                    setpixels(0, 38, 128, 30);
                    u8g2.setFont(u8g2_font_helvB08_tf);
                    u8g2.drawStr(12, 43, "Turning Light ON..");
                    u8g2.sendBuffer();
                    delay(2000);
                    digitalWrite(buzzerPin,LOW);
                    delay(2000);
                    setpixels(0, 38, 128, 30);
                    u8g2.sendBuffer();
                    toShowSchedule();
                    flag2=1;
                }
            }
        }
        if(flag4 == 0){
            digitalWrite(relayPin,LOW);
            if (flag2 == 1){
                digitalWrite(buzzerPin,HIGH);
                //turning off message
                setpixels(0, 38, 128, 30);
                u8g2.setFont(u8g2_font_helvB08_tf);
                u8g2.drawStr(12, 43, "Turning Light OFF..");
                u8g2.sendBuffer();
                delay(2000);
                flag2 = 0;
                digitalWrite(buzzerPin,LOW);
                delay(2000);
                setpixels(0, 38, 128, 30);
                u8g2.sendBuffer();
                toShowSchedule();
            }
        }
    }
    
    byte len1;
    byte len2;
    byte k=12;
    byte j=0;
    char valueloop = getButton();
    delay(1500);
    /*------------LIGHT SCHEDULE SETTING--------------*/  
    if(valueloop == 'C'){
        len1 = inputvalue("Enter AM Schedule:", arrayam1);
        if (len1 == 13){
            goto exittime1;
        }
        setpixels(0, 26, 128, 40);
        u8g2.sendBuffer(); //The cleared pixels have to be transferred from buffer to the display before any delay
        delay(500);
        len2 = inputvalue("Enter PM Schedule:", arraypm1);
        if (len2 == 13){
            goto exittime1;
        }
        for(i=len1; i <12; i++){ arrayam1[len1] = 0; len1++; }
        for(i=len2; i <12; i++){ arraypm1[len2] = 0; len2++; }
        setpixels(0, 26, 128, 40);
        u8g2.sendBuffer();
        delay(2000);
        u8g2.setFont(u8g2_font_chikita_tr);
        u8g2.drawStr(38, 56, "Processing...");
        u8g2.setFont(u8g2_font_timR10_tf);
        u8g2.drawStr(48, 30, "-AM-");
        j = 0;
        u8g2.setFont(u8g2_font_missingplanet_tn);
        for (i = 13; j < 12; i = i+8){
            u8g2.setCursor(i, 45);
            u8g2.print(arrayam1[j]);
            u8g2.sendBuffer();
            if (arrayam1[j] > 9){
                i = i + 8;
            }
            j++;
            delay(200);
        }
        delay(3000);
        setpixels(0, 26, 128, 30);
        u8g2.sendBuffer();
        delay(2000);
        u8g2.setFont(u8g2_font_timR10_tf);
        u8g2.drawStr(48, 30, "-PM-");
        j = 0;
        u8g2.setFont(u8g2_font_missingplanet_tn);
        for (i = 13; j < 12; i = i+8){
            u8g2.setCursor(i, 45);
            u8g2.print(arraypm1[j]);
            u8g2.sendBuffer();
            if (arraypm1[j] > 9){
                i = i + 8;
            }
            j++;
            delay(200);
        }
        delay(3000);
        u8g2.clearDisplay();
        delay(2000);
        u8g2.setFont(u8g2_font_repress_mel_tr);
        u8g2.drawStr(10, 10, "SAVE SETTING?");
        u8g2.sendBuffer();
        delay(500);
        u8g2.setFont(u8g2_font_crox2hb_tf);
        u8g2.drawStr(4, 26, "Press D for Yes");
        u8g2.drawStr(6, 40, "Press A for No");
        u8g2.sendBuffer();
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
        u8g2.clearDisplay();
        u8g2.drawStr(10, 33, "Updating.");
        u8g2.sendBuffer();
        delay(1000);
        u8g2.clearDisplay();
        u8g2.drawStr(10, 33, "Updating..");
        u8g2.sendBuffer();
        delay(1000);
        u8g2.clearDisplay();
        u8g2.drawStr(10, 33, "Updating...");
        u8g2.sendBuffer();
        delay(1000);
        exittime1:
            firstpagedisplay();
            displaytime();
            tempC = DS3231_get_treg();  // Reads the temperature as an int, to save memory
            setpixels(93, 26, 20, 10);
            u8g2.setCursor(93,26);
            u8g2.setFont(u8g2_font_smart_patrol_nbp_tr);
            u8g2.print(tempC);
            u8g2.sendBuffer();
            delay(1000);
    }
    
    /*-------------CLOCK SETTING-------------*/
    if(valueloop == 'B'){
        
        setpixels(0, 26, 128, 40); //clearing space below SMART FARM
        u8g2.sendBuffer();
        delay(2000);
        char valueup;
        byte hour1, hour2;
        byte minute1, minute2;
        u8g2.setFont(u8g2_font_8x13_mf);
        u8g2.drawStr(2, 26, "Hour");
        u8g2.drawVLine(35, 26, 23);
        u8g2.drawStr(40, 26, "Minute");
        u8g2.drawVLine(93, 26, 23);
        u8g2.drawStr(95, 26, "A/PM");
        u8g2.drawHLine(0, 37, 128);
        u8g2.drawHLine(0, 25, 128);
        u8g2.drawHLine(0, 49, 128);
        gotohour:  
          u8g2.setFont(u8g2_font_cu12_tr);
          u8g2.drawStr(5, 37, ">");
          u8g2.drawHLine(11, 43, 2);
          u8g2.setFont(u8g2_font_amstrad_cpc_extended_8f);
          u8g2.drawStr(105, 39, "AM");
          u8g2.setFont(u8g2_font_chroma48medium8_8u);
          u8g2.drawStr(1, 53, "D=NEXT, A=DELETE");
          u8g2.sendBuffer();
          byte ctmp1 = 14;
          u8g2.setFont(u8g2_font_missingplanet_tn);
          u8g2.setCursor(ctmp1, 38);
          //------HOUR SETUP------//
          do{
              valueup = getButton();
              if((valueup != NO_KEY) && (valueup != 'B')&& (valueup != 'C')&& (valueup != 'A')&& (valueup != 'D')&& (valueup != '*')&& (valueup != '0')&& (valueup != '#')){
                  u8g2.setCursor(ctmp1, 38);
                  u8g2.print(valueup);
                  u8g2.sendBuffer();
                  hour1 = valueup -'0';
                  if(ctmp1 == 20){
                      hour2 = hour2 * 10;
                      hour2 += hour1;
                      if( hour2 > 12){
                          setpixels(14, 38, 14, 11);
                          u8g2.sendBuffer();
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
                      u8g2.setCursor(ctmp1, 38);
                      u8g2.print("10");
                      u8g2.sendBuffer();
                      hour2 = 10;
                      goto check;
                  }
                  if(valueup == '0'){
                      u8g2.setCursor(ctmp1, 38);
                      u8g2.print("11");
                      u8g2.sendBuffer();
                      hour2 = 11;
                      goto check;
                  }
                  if(valueup == '#'){
                      u8g2.setCursor(ctmp1, 38);
                      u8g2.print("12");
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
        gotominute:
          ctmp1=61;
          u8g2.setFont(u8g2_font_cu12_tr);
          u8g2.drawStr(52, 37, ">");
          setpixels(6, 38, 8, 11);
          u8g2.drawHLine(58, 43, 2);
          u8g2.sendBuffer();
          check:
              if((valueup == '*')|| (valueup =='0')||(valueup == '#')){
                  do{
                      valueup = getButton();
                      if (valueup == 'A'){
                          setpixels(14, 38, 13, 11);
                          u8g2.sendBuffer();
                          goto gotohour;
                      }
                      if (valueup == 'D'){
                          goto gotominute;
                      }
                      if ((millis() - timeThen) >= 90000){
                          timeThen = millis();
                          goto exittime;
                      }   
                  }while(1);
              }
          u8g2.setFont(u8g2_font_missingplanet_tn);
          u8g2.setCursor(ctmp1, 38);
          do{
              valueup = getButton();
              if((valueup != NO_KEY) && (valueup != 'B')&& (valueup != 'C')&& (valueup != 'A')&& (valueup != 'D')&& (valueup != '*')&& (valueup != '0')&& (valueup != '#')){
                  u8g2.setCursor(ctmp1, 38);
                  u8g2.print(valueup);
                  u8g2.sendBuffer();
                  minute1 = valueup - '0';
                  if(ctmp1 == 67){
                      minute2 = minute2 * 10;
                      minute2 = minute2 + minute1;
                      if(minute2 > 59){
                          setpixels(61, 38, 14, 11);
                          u8g2.sendBuffer();
                          goto gotominute;
                      }
                  }
                  if(ctmp1 == 61){
                      minute2 = minute1;
                  }
                  ctmp1 = ctmp1 + 6;
              }
              if(ctmp1 < 61){
                  if(valueup == '*'){
                      u8g2.drawStr(ctmp1, 38, "10");
                      u8g2.sendBuffer();
                      minute2 = 10;
                      goto check2;
                  }
                  if(valueup == '0'){
                      u8g2.drawStr(ctmp1, 38, "11");
                      u8g2.sendBuffer();
                      minute2 = 11;
                      goto check2;
                  }
                  if(valueup == '#'){
                      u8g2.drawStr(ctmp1, 38, "10");
                      u8g2.sendBuffer();
                      minute2 = 12;
                      goto check2;
                  }
              }
              if(valueup == 'A'){
                  if(minute2 > 9){
                      ctmp1 -= 12;
                      minute2=0;
                      setpixels(ctmp1, 38, 13,11);
                      u8g2.sendBuffer(); 
                  }
                  else{
                      ctmp1 -= 6;
                      setpixels(ctmp1, 38, 13,11);
                      minute2 = 0;  
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
        gotochange:
          bool btmp1 = false;
          char chartmp;
          byte hourtmp2=hour2;
          u8g2.setFont(u8g2_font_cu12_tr);
          u8g2.drawStr(96, 37, ">");
          setpixels(53, 38, 8, 11);
          setpixels(1, 53, 128, 8);
          u8g2.setFont(u8g2_font_chroma48medium8_8u);
          u8g2.drawStr(1, 53, "C=CHANGE,D=DONE");
          u8g2.drawHLine(102, 43, 2);
          u8g2.sendBuffer();
          check2:
              if((valueup == '*')|| (valueup =='0')||(valueup == '#')){
                  do{
                      valueup = getButton();
                      if (valueup == 'A'){
                          setpixels(61, 38, 13, 11);
                          u8g2.sendBuffer();
                          goto gotominute;
                      }
                      if (valueup == 'D'){
                          goto gotochange;
                      }
                      if ((millis() - timeThen) >= 90000){
                          timeThen = millis();
                          goto exittime;
                     }   
                  }while(1);
              }
          change = false;
          u8g2.setFont(u8g2_font_amstrad_cpc_extended_8f);
          do{
              valueup = getButton();
              if(valueup == 'C'){
                  if(btmp1 == true){
                      setpixels(105, 39, 22, 11);
                      u8g2.drawStr(105, 39, "AM");
                      u8g2.sendBuffer();
                      change = false;
                  }
                  if(btmp1 == false){
                      setpixels(105, 39, 22, 11);
                      u8g2.drawStr(105, 39, "PM");
                      u8g2.drawHLine(100, 49, 27);
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
          u8g2.setFont(u8g2_font_repress_mel_tr);
          u8g2.drawStr(10, 10, "SAVE SETTING?");
          u8g2.sendBuffer();
          delay(500);
          u8g2.setFont(u8g2_font_crox2hb_tf);
          u8g2.drawStr(4, 26, "Press D for Yes");
          u8g2.drawStr(6, 40, "Press A for No");
          u8g2.sendBuffer();
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
          setDS3231time(00,minute2,hour2,4,15,5,18);   
          exittime:
            readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);
          firstpagedisplay();
          displaytime();
          tempC = DS3231_get_treg();  // Reads the temperature as an int, to save memory
          setpixels(93, 26, 20, 10);
          u8g2.setCursor(93,26);    
          u8g2.setFont(u8g2_font_smart_patrol_nbp_tr);
          u8g2.print(tempC);
          u8g2.sendBuffer();
    }
    
    tower();
    toShowSchedule();
    tower();
}

// Convert normal decimal numbers to binary coded decimal
byte decToBcd(byte val){return( (val/10*16) + (val%10) );}
// Convert binary coded decimal to normal decimal numbers
byte bcdToDec(byte val){return( (val/16*10) + (val%16) );}




void keypadEvent(KeypadEvent valueloop){
    if(myKeypad.getState() == HOLD){
        if( valueloop == 'A'){
            u8g2.clearDisplay();
            digitalWrite(buzzerPin,HIGH);
            delay(3000);
            digitalWrite(buzzerPin,LOW);
            
            for(int i=0; i< EEPROM.length(); i++)
                { EEPROM.write(i,0);}
            u8g2.setFont(u8g2_font_tenfatguys_tf);
            u8g2.setCursor(10,15);
            u8g2.print("RESETTING...");
            u8g2.sendBuffer();
            
            //Rotating time sand (0x23f3 is  hex for time sand symbol)
            u8g2.setFont(u8g2_font_unifont_t_symbols);
            u8g2.drawGlyph(50, 37, 0x23f3);
            u8g2.sendBuffer();
            delay(1000);
            setpixels(50, 37, 15, 15);
            u8g2.setFontDirection(1); //Writing from top to down (90 degree rotation)
            u8g2.drawGlyph(61, 34, 0x23f3);
            u8g2.sendBuffer();
            delay(1000);
            setpixels(50, 37, 15, 15);
            u8g2.setFontDirection(2);//Writing from right to left (180 degree rotation)
            u8g2.drawGlyph(64, 45, 0x23f3);
            u8g2.sendBuffer();
            delay(1000);
            setpixels(50, 37, 15, 15);
            u8g2.setFontDirection(3); //Writing from down to top (270 degree rotation)
            u8g2.drawGlyph(53, 48, 0x23f3);
            u8g2.sendBuffer();
            delay(600);
            setpixels(50, 37, 15, 15);
            u8g2.setFontDirection(0); //Writing normally (left to right)
            u8g2.drawGlyph(50, 37, 0x23f3);
            u8g2.sendBuffer();
            delay(600);
            setpixels(50, 37, 15, 15);
            u8g2.setFontDirection(1); //Writing from top to down (90 degree rotation)
            u8g2.drawGlyph(61, 34, 0x23f3);
            u8g2.sendBuffer();
            delay(600);
            //Reset Done
            u8g2.setFontDirection(0); //Re-adjusting writing direction i.e. from left to right
            u8g2.clearDisplay();
            delay(2000);
            resetFunc();
        }            
    }
}

void firstpagedisplay(){
    readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);
    u8g2.clearDisplay();
    delay(1000);
    //Smart farm
    u8g2.setFont(u8g2_font_lucasarts_scumm_subtitle_o_tf);
    u8g2.drawStr(2,10,"dabba FARM");
    u8g2.setFont(u8g2_font_smart_patrol_nbp_tr);
    u8g2.drawStr(57, 26, "TMP: ");
    u8g2.drawFrame(115, 26, 3, 3);
    u8g2.drawStr(118,26, "C");
    u8g2.sendBuffer();
    delay(500);
    //Balance
    u8g2.setFont(u8g2_font_chikita_tr);
    u8g2.setCursor(48, 1);
    u8g2.print("Balance : Rs. ");
    u8g2.print(balance);
    u8g2.sendBuffer();
    delay(500);
    u8g2.drawHLine(0, 37, 128);
    u8g2.sendBuffer();
    delay(500);
}

void displaytime(){
    // retrieve data from DS3231
    readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);
    u8g2.setCursor(0, 26);
    u8g2.setFont(u8g2_font_smart_patrol_nbp_tr);
    if (hour < 10){
      u8g2.setCursor(11, 26);
    }
    setpixels(0, 26, 22, 10);
    u8g2.print(hour);
    u8g2.print(" : ");
    setpixels(27, 26, 30, 10);
    if (minute < 10){
      u8g2.print("0");
    }
    u8g2.print(minute);
    u8g2.sendBuffer();
}

byte inputvalue(String display1, int arrayvalue[]){
    byte ctmp1 = 13;
    byte dtmp1 = 0;
    byte len;
    char value;
    setpixels(0, 26, 128, 40); //clearing space below SMART FARM
    u8g2.sendBuffer();
    delay(1000);
    u8g2.setFont(u8g2_font_timR10_tf);
    u8g2.setCursor(0, 30);
    u8g2.print(display1);
    u8g2.setFont(u8g2_font_cu12_tr);
    u8g2.drawStr(0, 44, ">>");
    u8g2.setFont(u8g2_font_chikita_tr);
    u8g2.drawStr(5, 56, "D = confirm, A = delete");
    u8g2.sendBuffer();
    u8g2.setCursor(ctmp1, 45);
    u8g2.setFont(u8g2_font_missingplanet_tn);
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
            u8g2.setCursor(ctmp1, 45);
            u8g2.print(arrayvalue[dtmp1]);
            u8g2.sendBuffer();
            dtmp1++;
            ctmp1 += 8;
        }
        if(value == '*'){
            arrayvalue[dtmp1] = 10;
            u8g2.setCursor(ctmp1,45);
            u8g2.print(arrayvalue[dtmp1]);
            u8g2.sendBuffer();
            ctmp1 += 15; dtmp1++;
        }
        if(value == '0'){
            arrayvalue[dtmp1] = 11;
            u8g2.setCursor(ctmp1,45);
            u8g2.print(arrayvalue[dtmp1]);
            u8g2.sendBuffer();
            ctmp1 += 15; dtmp1++;
        }
        if(value == '#'){
            arrayvalue[dtmp1] = 12;
            u8g2.setCursor(ctmp1,45);
            u8g2.print(arrayvalue[dtmp1]);
            u8g2.sendBuffer();
            ctmp1 += 15; dtmp1++;
        }
        if((value == 'A') && (ctmp1 != 14)){ //In case user wants to erase previously entered value
            if(arrayvalue[dtmp1-1] > 9){
                ctmp1 -= 15; dtmp1--;
                setpixels(ctmp1, 45, 20, 10); //erasing desired value
                u8g2.sendBuffer();
            }
            else{
                ctmp1 -= 8; dtmp1--;
                setpixels(ctmp1, 45, 20, 10);
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



void setDS3231time(byte second, byte minute, byte hour, byte dayOfWeek, byte dayOfMonth, byte month, byte year){
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
    //  *dayOfWeek = bcdToDec(Wire.read());
    //  *dayOfMonth = bcdToDec(Wire.read());
    //  *month = bcdToDec(Wire.read());
    //  *year = bcdToDec(Wire.read());
    *dayOfWeek = 0;
    *dayOfMonth =0;
    *month =0;
    *year = 0;
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

float DS3231_get_treg()
{
    int rv;  // Reads the temperature as an int, to save memory
   
    uint8_t temp_msb, temp_lsb;
    int8_t nint;

    Wire.beginTransmission(DS3231_I2C_ADDRESS);
    Wire.write(DS3231_TEMPERATURE_ADDR);
    Wire.endTransmission();

    Wire.requestFrom(DS3231_I2C_ADDRESS, 2);
    temp_msb = Wire.read();
    temp_lsb = Wire.read() >> 6;

    if ((temp_msb & 0x80) != 0){
        nint = temp_msb | ~((1 << 8) - 1);      // if negative get two's complement
    }
    else{
        nint = temp_msb;}
    rv = 0.25 * temp_lsb + nint;
    return rv;
}

void updateEeprom(int arrayam[12],int arraypm[12]){ 
    byte k=1;
    byte i=2;
    byte j=0;
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

//clear certain portion of screen (int horizontal_pixel_pos, int vertical_pixel_pos, int width, int height)
void setpixels(int a, int b, int c, int d){
          for (int j=0; j<c; j++){
          for(int k=0; k<d; k++){
            u8g2.setDrawColor(0);   //0 to clear pixel i.e black color
            u8g2.drawPixel(a+j,b+k);//in this position
          }
        }
        u8g2.setDrawColor(1); //1 in order to write with white color
}

void toShowSchedule(){
    byte firstdigit  = EEPROM.read(1);
    byte seconddigit = EEPROM.read(2);
    u8g2.setFont(u8g2_font_8x13_mf);
    u8g2.drawStr(0, 38, "A:");
    u8g2.drawHLine(0, 37, 128);
    u8g2.sendBuffer();
    showSchedule(arrayam, firstdigit, 38);
    
    u8g2.setFont(u8g2_font_8x13_mf);
    u8g2.drawStr(0, 51, "P:");
    u8g2.sendBuffer();
    showSchedule(arraypm, seconddigit, 51);
}

void showSchedule(int arrays[], int len, int pxht){
    int j = 15;
    setpixels(15, pxht, 128, 12);
    u8g2.setFont(u8g2_font_sirclive_tr);
    if (arrays[0] == 0){
        u8g2.drawStr(16, pxht+1, "NONE");
        u8g2.drawHLine(0, 37, 128);
    }
    u8g2.setFont(u8g2_font_missingplanet_tn);
    u8g2.sendBuffer();
    for (int i=0; i<len; i++){
        if(arrays[i] != 0){
            u8g2.setCursor(j, pxht+1);
            u8g2.print(arrays[i]);
            u8g2.sendBuffer();
            delay(50);
            j += 8;
            if(arrays[i]>9){
                j += 7;
            }
            if(arrays[i+1] != 0){
              u8g2.drawVLine(j-2, pxht+9, 2);
              u8g2.sendBuffer();
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
    u8g2.drawLine(3, 0, 3, 7);
    u8g2.drawLine(6, 6, 6, 7);
    u8g2.drawLine(9, 4, 9, 7);
    u8g2.drawLine(12, 2, 12, 7);
    u8g2.sendBuffer();
    for (int i = 0; i < 2; i++){
        u8g2.drawLine(15, 0, 15, 7);
        u8g2.sendBuffer();
        delay(50);
        setpixels(15, 0, 1, 8);
        u8g2.sendBuffer();
        delay(60);
    }
}
