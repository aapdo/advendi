#include <string>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

using namespace std;

LiquidCrystal_I2C lcd(0x27,20,4);
#define FLOWSENSORPIN 2                     // 핀 번호 설정
volatile uint16_t pulses = 0;               // 데이터 유형 설정
volatile uint8_t lastflowpinstate;
volatile uint32_t lastflowratetimer = 0;
volatile float flowrate;
int change_button = 10;
int flow_button = 11;
int waterRateSettingValue = 75;
String displayStr = "loading...";

void setup(){
    Serial.begin(9600);
    pinMode(FLOWSENSORPIN, INPUT);
    pinMode(change_button, INPUT);
    pinMode(flow_button, INPUT);

  
    digitalWrite(FLOWSENSORPIN, HIGH);
    lastflowpinstate = digitalRead(FLOWSENSORPIN);
    useInterrupt(true);

    lcd.begin();  // LCD초기 설정
  
    lcd.backlight(); // LCD초기 설정  

    lcd.setCursor(0,0);
    lcd.print("25%  ");
    lcd.print("50%  ");
    lcd.print("75%"); 

    lcd.setCursor(0,1);
    lcd.print("Choose amount!");
  
  
  //처음 텍스트가 LCD에 나타날 위치  lcd.setCursor(열, 행)
  //밑에 사진을 통해 출력시작 위치를 확인할 수 있습니다.
}


/**
 * @brief level1 
 * purpose: 버튼 클릭에 따른 출수 비율 변환 함수
 * @param waterRateSettingValue 
 * @return int 
 */
int changeWaterRateSetting(int waterRateSettingValue){
    switch (waterRateSettingValue)
    {
    case 75:
        waterRateSettingValue = 25;
        break;

    case 50:
        waterRateSettingValue = 75;
        break; 

    case 25:
        waterRateSettingValue = 50;
        break; 

    default:
        break;
    }
    return waterRateSettingValue;
}


/**
 * @brief level2
 * purpose: 압력 펌프를 가동시켜서 물을 끌어올리는 함수
 * 
 */
void turnOnPump();
/**
 * @brief level2
 * purpose: 압력 펌프를 가동 중지시키는 함수
 * 
 */
void turnOffPump();
/**
 * @brief level2
 * purpose: 현재 출력된 물의 양이 얼마인지 유량 측정 센서를 통해 측정하는 함수
 * 
 * @return long 
 */
void getCurrentWaterVolume() {
    float liters = pulses;
    liters /= 7.5;
    liters /= 60.0;
    Serial.print(liters*1000);
    Serial.println("ml");
    delay(100);
 }
/**
 * @brief level1
 * purpose: lcd에 어떤 문자를 출력할지 정해주는 함수.
 * 기본 값으로 출수 퍼센트를 표시함.
 * 오버라이딩으로 출수 버튼을 눌렀을 때 단계에 따라 측정 중, 출수 중 등의 메세지를 띄우기.
 */
void displayLCD(int waterRateSettingValue) {
    lcd.clear();
    lcd.print(waterRateSettingValue);
    lcd.print("%");
}

void displayLCD(String displayStr) {
    lcd.clear();
    lcd.print(waterRateSettingValue);
    lcd.print("% ");
    lcd.print("START!");
}


//반대편 아두이노로 정보를 출력하는 함수
void outputData(int data);
void outputData(string data);
void outputData(long data);

SIGNAL(TIMER0_COMPA_vect) {
    uint8_t x = digitalRead(FLOWSENSORPIN); // 유량측정센서 값을 디지털로 읽음
    if (x == lastflowpinstate) {
        lastflowratetimer++;
        return;
    }
    if (x == HIGH) { // x값이 들어오면 pulse값을 1 증가시킴
        pulses++;
    }
    lastflowpinstate = x;
    flowrate = 1000.0;
    flowrate /= lastflowratetimer;
    lastflowratetimer = 0;
}

void useInterrupt(boolean v) { // bool값에 따른 출력 설정
    if (v) {
        OCR0A = 0xAF;
        TIMSK0 |= _BV(OCIE0A);
    } else {
        TIMSK0 &= ~_BV(OCIE0A);
    }
}




void loop(){
    //디폴트는 컵 용량의 75%로 설정되어 있음.
    int waterRateSettingValue = 75;
    getCurrentWaterVolume();

    if(digitalRead(change_button) == HIGH){
        changeWaterRateSetting();
        displayLCD(waterRateSettingValue);
    }

    if(digitalRead(flow_button) == HIGH) {
        displayLCD(displayStr);
    }
}
