#include <string>

using namespace std;



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
long getCurrentWaterVolume();

/**
 * @brief level1
 * purpose: lcd에 어떤 문자를 출력할지 정해주는 함수.
 * 기본 값으로 출수 퍼센트를 표시함.
 * 오버라이딩으로 출수 버튼을 눌렀을 때 단계에 따라 측정 중, 출수 중 등의 메세지를 띄우기.
 */
void displayLCD(int waterRateSettingValue);
void displayLCD(string displayStr);


/**
 * @brief 전역 변수 
 * 서보모터는 사용자 기준 오른쪽을 기준으로 한다.
 * 
 */


void setup(){
    //위 전역변수 값 설정
    Serial.begin(9600);

}


void loop(){
    //디폴트는 컵 용량의 75%로 설정되어 있음.
    int waterRateSettingValue = 75;

}
