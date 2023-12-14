

/*bibliotek för relay, keypad, Rtc, oledskärm, */
#include <Relay.h>
#include <Keypad.h>
#include <RtcDS3231.h>
#include <Wire.h>
#include <U8glib.h> 

U8GLIB_SSD1306_128X64 oled(U8G_I2C_OPT_NO_ACK);

/* berättar vilken pin min relay är på och om den ska vara på eller av i neutralläget*/




/*säger hur jag vill att datorn läser av min keypad*/

const int ROW_NUM = 4; //four rows
const int COLUMN_NUM = 3; //three columns

char keys[ROW_NUM][COLUMN_NUM] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

byte pin_rows[ROW_NUM] = {10, 9, 8, 7}; //connect to the row pinouts of the keypad
byte pin_column[COLUMN_NUM] = {6, 5, 4}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );

int alarm_hour;

 


RtcDS3231<TwoWire> rtcModule(Wire);
 int hour;
  int minute;
  int second;
  
int value=0;


void setup() {
pinMode(12, OUTPUT);
  pinMode(2,OUTPUT);

Serial.begin(9600);

 oled.setFont(u8g_font_helvB10);
 
  Wire.begin();

RtcDateTime compiled = RtcDateTime(__DATE__,__TIME__);
rtcModule.SetDateTime(compiled);
 
}





void loop() {

  
  
updateRTC();

updateOled(String(hour)+":"+ String(minute)+":"+ String(second));



updateKeypad();


if(alarm_hour==hour){
  
  updatePeizo();
  updateMotor();
  }
 else{

digitalWrite(12, LOW);     
    
  }
  
}

void updateRTC() {
RtcDateTime now = rtcModule.GetDateTime();

hour = now.Hour();
minute = now.Minute();
second = now.Second();

}

void updateOled(String text)  {
  
  oled.firstPage();
  do{
    
      oled.drawStr(0,12,text.c_str());
    }while(oled.nextPage());

}

void updateKeypad() {
char key = keypad.getKey();
 if (key != NO_KEY)
   {
    if ( (key >= '0') && (key<= '9') )
      {
       value = value *10;
       value = value + key -'0';
      }
      if( key=='*'){
        alarm_hour=-1;
          }
    if ( key == '#' )
     {
      //do stuff with value
      //for demonstration purposes, for now we'll just print it
      Serial.println(value);
      if(value<=24){

        alarm_hour=value;
         updateOled ("Alarm set to" + String(alarm_hour));
         delay(1000);
        }
        else{
          updateOled ("Ogiltigt klockslag");
          delay(1000);
          }
          

      value=0;//Now reset ready for next input
      
        
     } 
   }
}


void updatePeizo()  {
  
  tone(2,1000);
  delay (1000);
  noTone(2);
  delay(1000);
   }



   void updateMotor(){
    
    digitalWrite(12, HIGH); 
    
    }
   
