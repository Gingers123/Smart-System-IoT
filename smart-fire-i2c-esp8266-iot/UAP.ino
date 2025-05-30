// Blynk Credentials
#define BLYNK_TEMPLATE_ID "TMPL6F5uVFXqs"
#define BLYNK_TEMPLATE_NAME "UAP"
#define BLYNK_AUTH_TOKEN "3bku5HzR1_l-hu5xEIO-jN6m42eZ2D5W"

#include <DHT.h>
#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// WiFi & Blynk credentials
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Lex.";
char pass[] = "104073826";

// PIN DEFINITION
#define FLAME_PIN D0
#define GAS_PIN D1
#define DHT_PIN D2
#define RELAY_PIN D3
#define SERVO_PIN D4
#define BUZZER_PIN D5
#define LED_PIN D6

// Inisialisasi Sensor dan Modul
#define DHTTYPE DHT22
DHT dht(DHT_PIN, DHTTYPE);
Servo servoMotor;
LiquidCrystal_I2C lcd(0x27, 20, 4); // alamat I2C 0x27 umum dipakai LCD 20x4

BlynkTimer timer;

void setup() {
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);
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

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Sistem Pencegah Api");
  delay(2000);
  lcd.clear();
}

void loop() {
  Blynk.run();
  timer.run();
}

void sendSensor() {
  int flame = digitalRead(FLAME_PIN); // LOW jika ada api
  int gas = analogRead(GAS_PIN);      // 0 - 4095
  float suhu = dht.readTemperature();

  bool bahaya = false;
  String statusPompa = "OFF";

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

  // LCD Display I2C
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Suhu  : ");
  lcd.print(suhu);
  lcd.print(" C");

  lcd.setCursor(0, 1);
  lcd.print("Gas   : ");
  lcd.print(gas);

  lcd.setCursor(0, 2);
  lcd.print("Flame : ");
  lcd.print(flame == LOW ? "Api" : "Aman");

  lcd.setCursor(0, 3);
  lcd.print("Pompa : ");
  lcd.print(statusPompa);

  // Blynk update
  Blynk.virtualWrite(V0, suhu);
  Blynk.virtualWrite(V1, gas);
  Blynk.virtualWrite(V2, flame == LOW ? 1 : 0); // 1: api, 0: aman
  Blynk.virtualWrite(V3, statusPompa == "ON" ? 1 : 0);
}