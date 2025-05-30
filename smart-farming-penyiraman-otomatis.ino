#include <DHT.h>
#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Pin definisi
#define DHTPIN 2
#define DHTTYPE DHT22
#define SOIL_PIN A0
#define RELAY_PIN 3
#define LED_PIN 4
#define SERVO_PIN 5

DHT dht(DHTPIN, DHTTYPE);
Servo myServo;
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(9600);
  dht.begin();
  myServo.attach(SERVO_PIN);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  digitalWrite(RELAY_PIN, HIGH); // Relay off (aktif LOW)
  digitalWrite(LED_PIN, LOW);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Smart Farming");
  delay(2000);
  lcd.clear();
}

void loop() {
  int soilValue = analogRead(SOIL_PIN);
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Serial output
  Serial.print("Soil Moisture: ");
  Serial.print(soilValue);
  Serial.print(" | Temperature: ");
  Serial.print(temperature);
  Serial.print(" C | Humidity: ");
  Serial.print(humidity);
  Serial.print(" % | ");

  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(temperature, 1);
  lcd.print("C ");
  lcd.print("H:");
  lcd.print(humidity, 0);
  lcd.print("% ");

  lcd.setCursor(0, 1);
  if (soilValue < 400) {
    lcd.print("Tanah: Kering  ");
    digitalWrite(RELAY_PIN, HIGH);   // Pompa nyala
    digitalWrite(LED_PIN, HIGH);
    myServo.write(90);

    Serial.println("Status: Tanah Kering | Pompa: ON");
  } else {
    lcd.print("Tanah: Lembab  ");
    digitalWrite(RELAY_PIN, LOW);  // Pompa mati
    digitalWrite(LED_PIN, LOW);
    myServo.write(0);

    Serial.println("Status: Tanah Lembab | Pompa: OFF");
  }

  delay(2000);
}
