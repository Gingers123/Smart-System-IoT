// Blynk Credentials
#define BLYNK_TEMPLATE_ID "TMPL6F5uVFXqs"
#define BLYNK_TEMPLATE_NAME "UAP"
#define BLYNK_AUTH_TOKEN "3bku5HzR1_l-hu5xEIO-jN6m42eZ2D5W"

#include <DHT.h>
#include <ESP32Servo.h>
#include <LiquidCrystal.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Wokwi-GUEST";
char pass[] = "";

// PIN DEFINITION
#define FLAME_PIN 34
#define GAS_PIN 35
#define DHT_PIN 32
#define RELAY_PIN 25
#define SERVO_PIN 26
#define BUZZER_PIN 27
#define LED_PIN 14

// LCD non-I2C PIN Mapping
#define LCD_RS 5
#define LCD_EN 18
#define LCD_D4 19
#define LCD_D5 21
#define LCD_D6 22
#define LCD_D7 23

#define DHTTYPE DHT22
DHT dht(DHT_PIN, DHTTYPE);
Servo servoMotor;
LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

BlynkTimer timer;

void setup() {
  Serial.begin(115200);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  timer.setInterval(1000L, sendSensor);

  pinMode(FLAME_PIN, INPUT);
  pinMode(GAS_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  digitalWrite(RELAY_PIN, LOW);
  noTone(BUZZER_PIN);
  digitalWrite(LED_PIN, LOW);

  servoMotor.attach(SERVO_PIN);
  servoMotor.write(0);

  dht.begin();

  lcd.begin(20, 4);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Sistem Pencegah Api");
  delay(2000);
}

void loop() {
  Blynk.run();
  timer.run();
}

void sendSensor() {
  int flame = digitalRead(FLAME_PIN); // Bisa juga analogRead jika sensor mendukung
  int gas = analogRead(GAS_PIN);      // 0 - 4095
  float suhu = dht.readTemperature();

  bool bahaya = false;
  String statusPompa = "OFF";

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Suhu: ");
  lcd.print(suhu);
  lcd.print(" C");

  if (flame == LOW || gas > 1000 || suhu > 45.0) {
    bahaya = true;
  }

  if (bahaya) {
    tone(BUZZER_PIN, 500);
    digitalWrite(LED_PIN, HIGH);
    digitalWrite(RELAY_PIN, HIGH);
    servoMotor.write(90);
    statusPompa = "ON";
  } else {
    noTone(BUZZER_PIN);
    digitalWrite(LED_PIN, LOW);
    digitalWrite(RELAY_PIN, LOW);
    servoMotor.write(0);
  }

  // LCD Display
  lcd.setCursor(0, 1);
  lcd.print("Gas: ");
  lcd.print(gas);

  lcd.setCursor(0, 2);
  lcd.print("Flame: ");
  lcd.print(flame == LOW ? "1 (Api)" : "0 (Aman)");

  lcd.setCursor(0, 3);
  lcd.print("Pompa: ");
  lcd.print(statusPompa);

  // Blynk Update
  Blynk.virtualWrite(V0, suhu);
  Blynk.virtualWrite(V1, gas);          // tampilkan nilai gas mentah
  Blynk.virtualWrite(V2, flame == LOW ? 1 : 0); // 1: api, 0: aman
  Blynk.virtualWrite(V3, statusPompa == "ON" ? 1 : 0);
}