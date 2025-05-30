#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

// Konfigurasi pin
#define DHTPIN 2
#define DHTTYPE DHT22
#define SOIL_PIN A0
#define WATER_PIN A1
#define RELAY_PIN 3
#define LED_PIN 4
#define SERVO_PIN 5
// #define BUZZER 6

// Inisialisasi sensor dan perangkat
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo myServo;

void setup() {
  Serial.begin(9600);

  dht.begin();
  lcd.begin(16, 2);
  lcd.backlight();

  pinMode(SOIL_PIN, INPUT);
  pinMode(WATER_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  // pinMode(BUZZER, OUTPUT);

  myServo.attach(SERVO_PIN);
  myServo.write(0); // posisi awal

  lcd.print("Smart Farming");
  delay(2000);
  lcd.clear();
}

void loop() {
  // Baca nilai sensor
  int soilValue = analogRead(SOIL_PIN);
  int waterLevel = analogRead(WATER_PIN);
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  // Interpretasi nilai
  bool soilDry = soilValue < 400;      // ambang batas kering
  bool waterAvailable = waterLevel > 300; // air cukup

  // Output ke Serial
  Serial.println("======================================");
  Serial.print("Soil Value     : "); Serial.println(soilValue);
  Serial.print("Water Level    : "); Serial.println(waterLevel);
  Serial.print("Temperature    : "); Serial.print(temp); Serial.println(" °C");
  Serial.print("Humidity       : "); Serial.print(hum); Serial.println(" %");

  // Update LCD
  lcd.setCursor(0, 0);
  lcd.print("T:"); lcd.print(temp); lcd.print("C ");
  lcd.print("H:"); lcd.print(hum); lcd.print("%");

  lcd.setCursor(0, 1);
  if (soilDry) {
    lcd.print("Soil: Dry  ");
    digitalWrite(LED_PIN, HIGH);
    // tone(BUZZER_PIN, 500);

    if (waterAvailable) {
      digitalWrite(RELAY_PIN, HIGH); // Pompa ON
      lcd.print("Pump: ON ");
      myServo.write(90); // Buka saluran

      Serial.println("Soil Status    : Kering");
      Serial.println("Water Status   : Cukup");
      Serial.println("Pompa          : ON");
      Serial.println("Katup Servo    : TERBUKA (90°)");
    } else {
      lcd.print("No Water!");
      digitalWrite(RELAY_PIN, LOW); // Pompa OFF

      Serial.println("Soil Status    : Kering");
      Serial.println("Water Status   : TIDAK CUKUP");
      Serial.println("Pompa          : OFF");
      Serial.println("Katup Servo    : TETAP (90°)");
    }

  } else {
    // lcd.clear();
    lcd.print("Soil: OK   ");
    digitalWrite(RELAY_PIN, LOW); // Pompa OFF
    digitalWrite(LED_PIN, LOW);
    // noTone(BUZZER_PIN);
    myServo.write(0); // Tutup saluran
    lcd.print("Pump: OFF");

    Serial.println("Soil Status    : Lembab");
    Serial.println("Pompa          : OFF");
    Serial.println("Katup Servo    : TERTUTUP (0°)");
  }

  Serial.println("======================================\n");
  delay(3000);
}
