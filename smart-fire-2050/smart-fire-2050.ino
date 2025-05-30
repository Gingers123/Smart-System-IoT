#include <DHT.h>
#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// PIN DEFINITION untuk Arduino UNO
#define FLAME_PIN 2        // Digital pin (gunakan sensor flame digital)
#define GAS_PIN A0         // Analog pin A0
#define DHT_PIN 3          // Digital pin untuk DHT
#define RELAY_PIN 4        // Digital output
#define SERVO_PIN 5        // PWM pin
#define BUZZER_PIN 6       // PWM/Output pin
#define LED_PIN 7          // Output LED

#define DHTTYPE DHT22
DHT dht(DHT_PIN, DHTTYPE);
Servo servoMotor;

// LCD I2C (alamat umum 0x27 untuk 20x4)
LiquidCrystal_I2C lcd(0x27, 20, 4);

void setup() {
  Serial.begin(9600);

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

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Sistem Pencegah Api");
  delay(2000);
  lcd.clear();
}

void loop() {
  int flame = digitalRead(FLAME_PIN);   // 0 = api terdeteksi
  int gas = analogRead(GAS_PIN);        // 0 - 1023 di Arduino UNO
  float suhu = dht.readTemperature();

  bool bahaya = false;
  String statusPompa = "OFF";

  Serial.println("==== SENSOR STATUS ====");
  Serial.print("Suhu: ");
  Serial.println(suhu);
  Serial.print("Gas: ");
  Serial.println(gas);
  Serial.print("Flame: ");
  Serial.println(flame == LOW ? "Api" : "Aman");

  if (flame == LOW || gas > 400 || suhu > 45.0) {
    bahaya = true;
  }

  if (bahaya) {
    tone(BUZZER_PIN, 500);
    digitalWrite(LED_PIN, HIGH);
    digitalWrite(RELAY_PIN, HIGH);
    servoMotor.write(90);  // Buka pintu darurat
    statusPompa = "ON";
  } else {
    noTone(BUZZER_PIN);
    digitalWrite(LED_PIN, LOW);
    digitalWrite(RELAY_PIN, LOW);
    servoMotor.write(0);   // Tutup pintu darurat
  }

  // Tampilkan ke LCD
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

  delay(2000);
}