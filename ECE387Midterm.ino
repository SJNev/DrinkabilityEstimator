#include <LiquidCrystal.h>
#include <math.h>
#define aref_voltage 5


LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

int tempPinAir = 0;        //the analog pin the TMP36's Vout (sense) pin is connected to
                        //the resolution is 10 mV / degree centigrade with a
                        //500 mV offset to allow for negative temperatures
int tempPinDrink = 1;
int tempReadingAir;        // the analog reading from the sensor
int tempReadingDrink;
float T0 = 0;
int optimalTemp = 85;    // 136F is the optimal drinking temperature; occurs when air above coffee is at about 85F
int unsafeTemp = 110;     //drinking beverages above 160F can cause scalding or burning; occurs at about 110F air temperature
float timeToOptimal;

void setup(void) {
  Serial.begin(9600);   
  lcd.begin(16, 2);
  //lcd.display();
}
 
 
void loop(void) {
  
  delay(3000);
  
  int airTemp = analogRead(tempPinAir);
  int drinkTemp = analogRead(tempPinDrink);

  float airTempF = fahrenheit(airTemp);
  float drinkTempF = fahrenheit(drinkTemp);
  Serial.println(drinkTempF);

  //Serial.print("Air = "); Serial.println(airTempF);
  //Serial.print("Drink = "); Serial.println(drinkTempF);

  if(T0 == 0){    //Write a new value to T0 only if T0 has not been set
    T0 = drinkTempF;
  }

  int optimalTime = newton(T0, airTempF);
  
  if ((millis()/1000) > 20) {
    if (drinkTempF > unsafeTemp) {
      Serial.print("Unsafe to drink! ");
      Serial.print("Wait "); Serial.print(optimalTime); Serial.println(" seconds.");
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Unsafe to drink! ");
      lcd.setCursor(0,1);
      lcd.print("Wait "); lcd.print(optimalTime); lcd.print(" seconds.");
    }
    else if (optimalTime <= 0 || drinkTempF <= optimalTemp) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Optimal temp");
      lcd.setCursor(0,1);
      lcd.print("reached!");
    }
    else if (optimalTime > 0) {
      //Serial.print("Wait "); Serial.print(timeToOptimal); Serial.print(" seconds for best temperature.");
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Drinkable, but");
      lcd.setCursor(0,1);
      lcd.print("could be better;");
      delay(5000);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Wait "); lcd.print(optimalTime); lcd.print(" secs");
      lcd.setCursor(0,1);
      lcd.print("for best temp");
      delay(5000);
    }
  } else {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Calibrating.");
    lcd.setCursor(0,1);
    lcd.print("Please wait...");
  }

}

float newton(float T0, float airTempF) {
  int calTempAnalog = analogRead(tempPinDrink); 
  float calTime = millis()/1000.0;    //20 seconds for calibration
  float calTempF = fahrenheit(calTempAnalog);
  float k = - log((calTempF - airTempF)/(T0 - airTempF)) / calTime;
  if (k == 0){
    return 0.0;           //this will indicate to the main loop that the desired temperature has been reached
  }
  //Serial.print("calTempF "); Serial.println(calTempF);
  //Serial.print("T0 "); Serial.println(T0);
  float optimalTime = - log((optimalTemp - airTempF)/(T0 - airTempF)) / k;
  optimalTime = int (optimalTime);
  return optimalTime;
}

float fahrenheit(int analog) {    //converts analog input from temp sensor to degrees F
  float voltage = analog * aref_voltage;
  voltage /= 1024.0;
  float tempC = (voltage - 0.5) * 100;
  float tempF = (tempC * 9.0/5.0) + 32.0;
  return tempF;
}

