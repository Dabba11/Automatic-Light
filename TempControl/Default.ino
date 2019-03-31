#define tempPin A3
#define ledYellow A4
#define relayPin A5
#define ledRed 11
void setup() {
    pinMode(tempPin, INPUT);
    pinMode(relayPin, OUTPUT);
    pinMode(ledRed, OUTPUT);
    pinMode(ledYellow, OUTPUT);
}
void loop() {
    float sumTemp = 0;
    for (int i = 0; i < 60; i++){
        sumTemp += getTemp();
        delay(2000);
    }
    if (sumTemp/60 < 25){
        digitalWrite(relayPin, HIGH);
        digitalWrite(ledYellow, HIGH);
        digitalWrite(ledRed, LOW);
    }
    else if (sumTemp/60 >33){
        digitalWrite(relayPin, HIGH);
        digitalWrite(ledYellow, LOW);
        digitalWrite(ledRed, HIGH);
    }
//    else if(sumTemp/60 > 35){
//        //digitalWrite(moreLED, HIGH);
//        if (buzz == 0){
//            digitalWrite(buzzerPin, HIGH);
//            delay(2000);
//            digitalWrite(buzzerPin, LOW);
//            buzz = 1;
//        }
//    }
    else{
        digitalWrite(relayPin, LOW);
        digitalWrite(ledYellow, LOW);
        digitalWrite(ledRed, LOW);
    }
    delay(1000);
}

/*Data pin of temp sensor to tempPin, from tempPin put 10K resistor to Ground, other end of sensor to 5Volt*/
float getTemp(void){
    float A = 1.009249522e-03, B = 2.378405444e-04, C = 2.019202697e-07;
    //Voltage Divider between 5Volt and sensor end
    float Rout = log(10000*(1024.0/analogRead(tempPin)-1));  //Resistance of thermistor in the specific voltage level
    //The polarity of thermistor ends can be changed to change how voltage is measured across it
    /*Mathematically, Resistance => Temperature done using Stein-Hart equation Below (in Kelvin) (-273 is done for Celcius value and +2.34 is an offset for approximate accuracy)*/
    return ((1.0/(A+B*Rout + C*Rout*Rout*Rout)) - 273.15 + 2.34);
}
