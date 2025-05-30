#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"

#define DHT_TYPE DHT22
#define DHT_PIN 13        // Sesuai gambar
#define PIR_PIN 7         // Sesuai gambar
#define HC_TRIG_PIN 12    // Sesuai gambar
#define HC_ECHO_PIN 2     // Sesuai gambar
#define BUZZER_PIN 4      // Sesuai gambar
#define DOOR_MAX_RANGE 100

DHT dht(DHT_PIN, DHT_TYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Alamat LCD 0x27, ukuran 16x2

float duration_us, distance_cm;

void setup() {
  Serial.begin(115200);
  dht.begin();

  pinMode(PIR_PIN, INPUT);
  pinMode(HC_TRIG_PIN, OUTPUT);
  pinMode(HC_ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  //pinMode(LED_PIN, OUTPUT); // LED dihapus
  pinMode(DHT_PIN, INPUT);

  lcd.init();
  lcd.backlight();
  lcd.clear();

  delay(3000);
}

void loop() {
  // membaca sensor DHT22
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  bool dhtValid = !isnan(t) && !isnan(h);

  // membaca sensor gerak (PIR)
  int pirState = digitalRead(PIR_PIN);

  // membaca sensor pintu (ultrasonik)
  // memulai pengukuran baru
  digitalWrite(HC_TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(HC_TRIG_PIN, LOW);

  // membaca hasil pengukuran
  duration_us = pulseIn(HC_ECHO_PIN, HIGH);
  distance_cm = duration_us / 58.0;

  // Logika alarm
  if (pirState == HIGH || distance_cm > DOOR_MAX_RANGE) {
    tone(BUZZER_PIN, 200);
    if (pirState == HIGH) {
      Serial.println("Gerakan terdeteksi!");
    }
    if (distance_cm > DOOR_MAX_RANGE) {
      Serial.println("Pintu terbuka");
      Serial.println("Jarak pintu: " + String(distance_cm) + " cm");
    }
  } else {
    noTone(BUZZER_PIN);
    Serial.println("Tidak ada gerakan terdeteksi");
    Serial.println("Pintu tertutup");
    Serial.println("Jarak pintu: " + String(distance_cm) + " cm");
  }

  // Output ke Serial Monitor
  if (dhtValid) {
    Serial.println("Temperatur: " + String(t, 2) + " °C");
    Serial.println("Kelembapan: " + String(h, 1) + " %");
  } else {
    Serial.println("Gagal membaca sensor DHT22");
  }

  // Tampilkan di LCD
  lcd.clear();
  if (dhtValid) {
    lcd.setCursor(0, 0);
    lcd.print("T:");
    lcd.print(t, 1);
    lcd.print("C H:");
    lcd.print(h, 0);
    lcd.print("%");
  } else {
    lcd.setCursor(0, 0);
    lcd.print("DHT22 Error");
  }

  lcd.setCursor(0, 1);
  if (pirState == HIGH) {
    lcd.print("Gerak ");
  } else {
    lcd.print("No Gerak ");
  }

  if (distance_cm > DOOR_MAX_RANGE) {
    lcd.print("Pintu Terbuka");
  } else {
    lcd.print("Pintu Tertutup");
  }

  delay(1000); // jeda 1 detik sebelum pengukuran berikutnya
}