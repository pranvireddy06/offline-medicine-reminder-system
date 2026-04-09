#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>

RTC_DS3231 rtc;
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

int buzzer = 6;

struct Medicine {
  int hour;
  int minute;
  char name[10];
  int taken;
};

#define MAX_MED 5
Medicine meds[MAX_MED];
int medCount = 0;

void loadData();
void saveData();
void checkMedicine();
void alertUser(Medicine &m);

void setup() {
  Serial.begin(9600);
  Wire.begin();
  rtc.begin();

  lcd.begin(16, 2);
  pinMode(buzzer, OUTPUT);

  lcd.print("Medicine System");
  delay(2000);
  lcd.clear();

  loadData();
}

void loop() {
  DateTime now = rtc.now();

  lcd.setCursor(0, 0);
  lcd.print("Time: ");
  lcd.print(now.hour());
  lcd.print(":");
  lcd.print(now.minute());

  checkMedicine();

  delay(1000);
}

void loadData() {
  EEPROM.get(0, medCount);
  if (medCount > MAX_MED) medCount = 0;

  for (int i = 0; i < medCount; i++) {
    EEPROM.get(10 + i * sizeof(Medicine), meds[i]);
  }
}

void saveData() {
  EEPROM.put(0, medCount);

  for (int i = 0; i < medCount; i++) {
    EEPROM.put(10 + i * sizeof(Medicine), meds[i]);
  }
}
void checkMedicine() {
  DateTime now = rtc.now();

  for (int i = 0; i < medCount; i++) {
    if (meds[i].hour == now.hour() &&
        meds[i].minute == now.minute() &&
        meds[i].taken == 0) {

      alertUser(meds[i]);
    }
  }
}

void alertUser(Medicine &m) {
  int attempts = 0;

  while (attempts < 3) {
    lcd.clear();
    lcd.print("Take: ");
    lcd.print(m.name);

    lcd.setCursor(0, 1);
    lcd.print("Press key!");

    digitalWrite(buzzer, HIGH);
    delay(1000);
    digitalWrite(buzzer, LOW);

    if (Serial.available()) {
      char input = Serial.read();

      if (input == '1') {
        m.taken = 1;
        lcd.clear();
        lcd.print("Dose Taken");
        delay(2000);
        return;
      }
    }

    attempts++;
    delay(3000);
  }

  lcd.clear();
  lcd.print("Dose Missed");
  delay(2000);
}
