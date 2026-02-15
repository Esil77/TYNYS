#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

int R1_PWM = 4;   // маленький кулер
int L1_PWM = 3;
int R2_PWM = 6;   // большой кулер
int L2_PWM = 5;

int dustPin = A0;   
int ledPin = 7;  

float dustDensity = 0;

void setup()
{
  Serial.begin(9600);

  pinMode(R1_PWM, OUTPUT);
  pinMode(L1_PWM, OUTPUT);
  pinMode(R2_PWM, OUTPUT);
  pinMode(L2_PWM, OUTPUT);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);

  lcd.init();
  lcd.backlight();
  lcd.clear();
}

void loop()
{
  digitalWrite(ledPin, LOW);
  delayMicroseconds(280);

  int raw = analogRead(dustPin);

  delayMicroseconds(40);
  digitalWrite(ledPin, HIGH);
  delayMicroseconds(9680);

  float voltage = raw * (5.0 / 1023.0);
  dustDensity = (voltage - 0.9) / 0.005;
  if (dustDensity < 0) dustDensity = 0;

  Serial.println(dustDensity);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("PM2.5: ");
  lcd.print(dustDensity, 1);
  lcd.print(" ug/m3");

  if (dustDensity <= 9)
  {
    lcd.setCursor(0, 1);
    lcd.print("Status: GOOD");
    stopFans();
    delay(2000);
  }
  else if (dustDensity <= 35.4)
  {
    lcd.setCursor(0, 1);
    lcd.print("Status: MODERATE");
    delay(2000);
  }
  else
  {
    lcd.setCursor(0, 1);
    lcd.print("Status: UNHEALTHY");
    delay(2000);
    startCleaning();
  }
}

void startCleaning()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("IN PROCESS...");
  lcd.setCursor(0, 1);
  lcd.print("Cleaning air");

  analogWrite(R1_PWM, 255);
  analogWrite(L1_PWM, 0);
  delay(8000);
  stopFans();

  delay(15000);

  analogWrite(R2_PWM, 255);
  analogWrite(L2_PWM, 0);
  delay(7000);
  stopFans();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("PM2.5: ");
  lcd.print(dustDensity, 1);
  lcd.print(" ug/m3");

  lcd.setCursor(0, 1);
  lcd.print("AIR CLEANED");

  delay(6000);
}

void stopFans()
{
  analogWrite(R1_PWM, 0);
  analogWrite(L1_PWM, 0);
  analogWrite(R2_PWM, 0);
  analogWrite(L2_PWM, 0);
}