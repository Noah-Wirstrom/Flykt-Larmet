/* Inkludera nödvändiga bibliotek */
#include <Relay.h>
#include <Keypad.h>
#include <RtcDS3231.h>
#include <Wire.h>
#include <U8glib.h> 

U8GLIB_SSD1306_128X64 oled(U8G_I2C_OPT_NO_ACK);

/* Definiera knappsatsens layout */
const int ROW_NUM = 4; // Antal rader
const int COLUMN_NUM = 3; // Antal kolumner

char keys[ROW_NUM][COLUMN_NUM] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

byte pin_rows[ROW_NUM] = {10, 9, 8, 7}; // Kopplas till raderna på knappsatsen
byte pin_column[COLUMN_NUM] = {6, 5, 4}; // Kopplas till kolumnerna på knappsatsen

Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );

int alarm_hour;

RtcDS3231<TwoWire> rtcModule(Wire);
int hour;
int minute;
int second;

int value=0;

void setup() {
  /* Konfigurera utgångspinnar för relä och summer */
  pinMode(12, OUTPUT);
  pinMode(2, OUTPUT);

  Serial.begin(9600);

  oled.setFont(u8g_font_helvB10);

  Wire.begin();

  /* Sätt RTC-tiden till kompileringstiden */
  RtcDateTime compiled = RtcDateTime(__DATE__,__TIME__);
  rtcModule.SetDateTime(compiled);
}

void loop() {
  /* Uppdatera RTC (Real Time Clock) */
  updateRTC();

  /* Uppdatera OLED-skärmen med aktuell tid */
  updateOled(String(hour) + ":" + String(minute) + ":" + String(second));

  /* Uppdatera knappsatsinmatning */
  updateKeypad();

  /* Kontrollera om det är dags att aktivera alarmet */
  if(alarm_hour == hour) {
    updatePeizo(); // Aktivera summer
    updateMotor(); // Aktivera relä (antaget att det är en motorstyrning)
  } else {
    digitalWrite(12, LOW); // Stäng av relä om det inte är dags för alarm
  }
}

/* Funktion för att uppdatera RTC-tiden */
void updateRTC() {
  RtcDateTime now = rtcModule.GetDateTime();

  hour = now.Hour();
  minute = now.Minute();
  second = now.Second();
}

/* Funktion för att uppdatera texten på OLED-skärmen */
void updateOled(String text)  {
  oled.firstPage();
  do {
    oled.drawStr(0, 12, text.c_str());
  } while(oled.nextPage());
}

/* Funktion för att uppdatera knappsatsinmatningen */
void updateKeypad() {
  char key = keypad.getKey();
  if (key != NO_KEY) {
    if ( (key >= '0') && (key <= '9') ) {
      value = value * 10;
      value = value + key - '0';
    }
    if ( key == '*' ) {
      alarm_hour = -1; // Återställ alarmtiden om "*" trycks
    }
    if ( key == '#' ) {
      // Gör något med värdet (för närvarande bara skriv ut det)
      Serial.println(value);
      if(value <= 24) {
        alarm_hour = value;
        updateOled("Alarm set to " + String(alarm_hour));
        delay(1000);
      } else {
        updateOled("Ogiltigt klockslag");
        delay(1000);
      }
      value = 0; // Återställ värdet för nästa inmatning
    } 
  }
}

/* Funktion för att aktivera summerljudet */
void updatePeizo()  {
  tone(2, 1000);
  delay(1000);
  noTone(2);
  delay(1000);
}

/* Funktion för att aktivera motorn (antaget att det är en motorstyrning) */
void updateMotor(){
  digitalWrite(12, HIGH); 
}

   
