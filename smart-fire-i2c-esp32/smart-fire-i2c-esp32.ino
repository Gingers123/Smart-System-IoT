#include <DHT.h>
#include <ESP32Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// PIN DEFINITION
#define FLAME_PIN 34
#define GAS_PIN 35
#define DHT_PIN 32
#define RELAY_PIN 25
#define SERVO_PIN 26
#define BUZZER_PIN 27
#define LED_PIN 14

#define DHTTYPE DHT22
DHT dht(DHT_PIN, DHTTYPE);
Servo servoMotor;

// Inisialisasi LCD I2C (alamat umum 0x27, ukuran 20x4)
LiquidCrystal_I2C lcd(0x27, 20, 4);

void setup() {
  Serial.begin(115200);

  pinMode(FLAME_PIN, INPUT);
  pinMode(GAS_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  digitalWrite(RELAY_PIN, LOW);
  noTone(BUZZER_PIN);
  digitalWrite(LED_PIN, LOW);

  servoMotor.attach(SERVO_PIN);
  servoMotor.write(0); // Pintu darurat tertutup

  dht.begin();

  lcd.init();            // Inisialisasi LCD
  lcd.backlight();       // Nyalakan backlight
  lcd.setCursor(0, 0);
  lcd.print("Sistem Pencegah Api");
  delay(2000);
  lcd.clear();
}

void loop() {
  int flame = digitalRead(FLAME_PIN);   // 0 = api terdeteksi
  int gas = analogRead(GAS_PIN);        // 0 - 4095
  float suhu = dht.readTemperature();   // dalam Celcius

  bool bahaya = false;
  String statusPompa = "OFF";

  Serial.println("==== SENSOR STATUS ====");
  Serial.print("Suhu: ");
  Serial.println(suhu);
  Serial.print("Gas: ");
  Serial.println(gas);
  Serial.print("Flame: ");
  Serial.println(flame == LOW ? "Api" : "Aman");

  if (flame == LOW || gas > 1000 || suhu > 45.0) {
    bahaya = true;
  }

  if (bahaya) {
    tone(BUZZER_PIN, 500);
    digitalWrite(LED_PIN, HIGH);
    digitalWrite(RELAY_PIN, HIGH);   // Pompa ON
    servoMotor.write(90);            // Pintu darurat buka
    statusPompa = "ON";
  } else {
    noTone(BUZZER_PIN);
    digitalWrite(LED_PIN, LOW);
    digitalWrite(RELAY_PIN, LOW);   // Pompa OFF
    servoMotor.write(0);            // Pintu darurat tutup
  }

  // Tampilkan ke LCD I2C
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Suhu : ");
  lcd.print(suhu);
  lcd.print(" C");

  lcd.setCursor(0, 1);
  lcd.print("Gas  : ");
  lcd.print(gas);

  lcd.setCursor(0, 2);
  lcd.print("Flame: ");
  lcd.print(flame == LOW ? "Api" : "Aman");

  lcd.setCursor(0, 3);
  lcd.print("Pompa: ");
  lcd.print(statusPompa);

  delay(2000); // Delay antar pembacaan
}