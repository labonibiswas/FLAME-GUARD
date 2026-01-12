#include <SoftwareSerial.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);
SoftwareSerial mySerial(9, 10);

const int red = 3;
const int green = 4;
const int buzzer = 13;
const int flame = 6;
const int smoke = A0;
const int sprinkler = 5;

int thresh= 200;
int status = true;
String alertMsg;
String mob1="+919230128898";

void setup()
{
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(smoke, INPUT);
  pinMode(flame, INPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(sprinkler, OUTPUT);   // ✅ Initialize sprinkler as OUTPUT
  
  lcd.init();  
  lcd.clear();  
  lcd.backlight();

  mySerial.begin(9600);  
  Serial.begin(9600);    
  delay(100);
}

void siren(int buzzer){
  for(int hz = 440; hz < 1000; hz++){
    tone(buzzer, hz, 50);
    delay(5);
  }

  for(int hz = 1000; hz > 440; hz--){
    tone(buzzer, hz, 50);
    delay(5);
  }
}

void loop()
{
  Serial.println("Gas Val: "+String(analogRead(smoke))+", Flame state: "+String(!digitalRead(flame)));

  if (digitalRead(flame)== LOW || analogRead(smoke)>thresh)  
  {
    digitalWrite(red, HIGH);
    siren(buzzer);
    digitalWrite(green, LOW);

    if(digitalRead(flame)== LOW){
      digitalWrite(sprinkler, HIGH);
      lcd.setCursor(2, 1);
      lcd.write(1);
      lcd.setCursor(4,1); 
      alertMsg= "FIRE HIGH";
      lcd.print(alertMsg);
      lcd.setCursor(4,0);   
      lcd.print("SMOKE:"+String(analogRead(smoke)));
    }

    if(analogRead(smoke)>thresh){
      lcd.setCursor(2, 0);
      lcd.write(1);
      lcd.setCursor(4,0); 
      alertMsg= "SMOKE HIGH";
      lcd.print(alertMsg);
      lcd.setCursor(4,1);   
      lcd.print("FIRE:"+String(digitalRead(flame)==LOW?"HIGH":"LOW"));
    }

    Serial.println(alertMsg);
    if(status){
      status = false;
      String msg= "Alert Type: "+alertMsg;
      SendMessage(msg,mob1);  
    }
  }
  else{
    status = true;
    lcd.setCursor(4,0);   
    lcd.print("SMOKE:"+String(analogRead(smoke)));
    lcd.setCursor(4,1);   
    lcd.print("FIRE:"+String(digitalRead(flame)==LOW?"HIGH":"LOW"));

    digitalWrite(red, LOW);
    noTone(buzzer);
    digitalWrite(green, HIGH);
    digitalWrite(sprinkler, LOW);
  }

  delay(500);
  lcd.clear();
}

void SendMessage(String msg, String mob)
{
  Serial.println(msg);

  mySerial.println("AT+CMGF=1");    
  delay(1000);  
  mySerial.println("AT+CMGS=\""+mob+"\"\r"); 
  delay(1000);
  mySerial.println(msg);
  delay(100);
  mySerial.println((char)26);
  delay(1000);
}

