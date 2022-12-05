#include <string>
#include<Servo.h>
#include<Stepper.h>

using namespace std;

double PI = 3.141592;
long DISTANCE_WIDTH;//정수기 전원을 켰을 때 측정하고 값을 변경하지 않음.
long fixedServoAngle;//서보모터를 돌리면서 마지막으로 고정된 서보모터의 각도

//오른쪽 초음파 센서 
int ULTRA_RIGHT_TRIG;//초음파 발사
int ULTRA_RIGHT_ECHO;//초음파 받음

//왼쪽 초음파 센서
int ULTRA_LEFT_TRIG;//초음파 발사
int ULTRA_LEFT_ECHO;//초음파 받음

//스텝 모터 한번당 회전하는 각도.
//필요시 자료형 변환할 것.
int STEPPER_ANGLE;
//Stepper 객체 레퍼런스
Stepper stepRight;
Stepper stepLeft;

Servo servoL;
Servo servoR;


/*
 * @brief 공통부
 * 
 * @return long 
 */

//purpose: 초음파 센서로 거리 측정하고 그 값을 리턴.
long getDistance(int TRIG_PIN_NUMBER, int ECHO_PIN_NUMBER){
    long duration, distance;
    digitalWrite(TRIG_PIN_NUMBER, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN_NUMBER, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN_NUMBER, LOW);

    duration = pulseIn(ECHO, HIGH); //물체에 반사되어돌아온 초음파의 시간을 변수에 저장합니다.

    //cm 로 환산
    distance = duration * 17 / 1000;

    return distance;
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
 * @brief level1 
 * purpose: Servo motor의 각도를 셋팅해주는 함수
 * 
 * @include 
 * global: getDistance()
 * level2: isChangedInclination()
 * level3: turnServo()
 * 
 * @return long fixedServoAngle
 */
void settingServoAngle(){
    /**
     * global value
     * long fixedServoAngle;
     * 
     */
    //local value
    long distanceR, distanceL;

    //서보모터 작동 시작
    turnServo(95);//서보모터를 95도로 설정
    distanceR = getDistance();
    turnServo(85);//서보모터를 85도로 설정
    distanceL = getDistance();
        
        //좌우 비교 시작
    if(distanceL > distanceR)//오른쪽이 작다면 오른쪽으로 돌아야함
    {
        if(isChangedInclination(90, 180)){//90도~180도 범위를 측정 true가 나온 순간 경향이 바뀌기 시작함.
            fixedServoAngle;//이 때 당시의 앵글을 반환해줘야함. 전역변수를 쓰거나 포인터를 쓰거나.
        }
    }else if (distanceL < distanceR)//왼쪽이 작으면 왼쪽으로 돌려야함
    {
        if(isChangedInclination(90, 0)){//90~0도 범위를 측정 true가 나온 순간 경향이 바뀌기 시작함.
            fixedServoAngle;//이 때 당시의 앵글을 반환해줘야함. 전역변수를 쓰거나 포인터를 쓰거나.
        }
    }else{
        fixedServoAngle = 90;
    }
    turnServo(fixedServoAngle);//위에서 픽스된 각도로 서보모터를 이동시킴.
}

/**
 * @brief level2
 * purpose: 서보모터를 돌리면서 거리를 측정하는데 그 값의 경향이 바뀌었는가?
 * @param startAngle 
 * @param endAngle 
 * @return true 
 * @return false 
 * 
 * @include 
 * turnServo()
 * 
 * 증감이 바뀌는 순간 global 변수 fixedServoAngle 에 그 각도를 저장하고 return true 반환.
 */
bool isChangedInclination(long startAngle, long endAngle);

/**
 * @brief level3 
 * purpose: angle 각도로 서보모터를 돌림.
 * 
 * @param angle 
 */
void turnServo(Servo servo, int angle){
    servo.write(angle);
}

/**
 * @brief level1 컵의 총 부피를 구하는 함수
 * 
 * @return long 
 * @include
 * global: getDistance
 * level2: upSensorByMotor()
 * 
 */
long getVolume(){
    /**
     * global: 
     * DISTANCE_WIDTH: 정수기의 가로 길이
     * 초음파 핀 번호
     * ULTRA_RIGHT_TRIG
     * ULTRA_RIGHT_ECHO
     * ULTRA_LEFT_TRIG
     * ULTRA_LEFT_ECHO
     * 
     */
    long distanceL, distanceR; //왼쪽 초음파, 오른쪽 초음파에서 구한 값이 된다.
    long radiusCup;//컵의 반지름
    long heightCup;//컵 높이
    int cnt;//서보모터가 몇 번 움직였는지
    while ((distanceR >= DISTANCE_WIDTH))//정수기 가로길이보다 초음파 결과값이 크면 멈춰야함.
    {
        distanceL = getDistance(ULTRA_LEFT_TRIG, ULTRA_LEFT_ECHO);
        distanceR = getDistance(ULTRA_RIGHT_TRIG, ULTRA_RIGHT_ECHO);
        upSensorByMotor(stepRight);
        upSensorByMotor(stepLeft);

        //반지름을 구함
        radiusCup += getRadius(distanceL, distanceR);
        ++cnt;
    }

    //반지름 공식 추가.
    radiusCup = radiusCup/cnt;

    //높이 구하기
    heightCup = getHeight(cnt);
    
    //pi * r^2 * height => 부피
    return PI *(radiusCup*radiusCup)*heightCup;
    
}

/**
 * @brief level2
 * purpose: 스텝모터를 통해 센서를 조금씩 올려주는 함수
 * STEPPER_ANGLE을 참고하여 1call 당 1STEPPER_ANGLE씩 돌려야함.
 * @param Stpper 객체 레퍼런스를 사용함.
 */
void upSensorByMotor(Stepper *stepperL, Stepper *stepperR);

//global 변수 fixedServoAngle을 사용하여 계산할 것.
//미구현된 사항: 서보모터 각도에 따른 길이 계산 공식 미적용 추가 바람.
/**
 * @brief level2
 * purpose: 좌우의 초음파 센서에서 측정된 값을 바탕으로 컵의 반지름을 구하는 함수.
 * @param distanceL 
 * @param distanceR 
 * @return long 
 */
long getRadius(long distanceL, long distanceR);

//cnt 와
//전역변수: STEPPER_ANGLE 로 
//높이를 구하는 함수.
/**
 * @brief level2
 * purpose: 스텝모터의 회전당 올라가는 높이와 회전 수를 곱하여 컵의 높이를 구하는 함수.
 * @param cnt 
 * @return long 
 */
long getHeight(int cnt);

/**
 * @brief level1 
 * purpose: 정수기의 출수를 담당하는 함수
 * @return int 
 * 
 * @include
 * 
 */
int turnOnWater(long totalWaterVolume){
    //유수 측정 센서에서 
    long outWaterVolume = 0;

    turnOnPump();
    //총 추출해야하는 물의 양보다 현재 유수 측정 센서에 감지된 양이 적다면
    while (outWaterVolume <= totalWaterVolume)
    {
        outWaterVolume = getCurrentWaterVolume();
    }
    turnOffPump();
    
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
    servoL.attach(servoLPin);
    servoR.attach(servoRPin);

}


void loop(){
    //디폴트는 컵 용량의 75%로 설정되어 있음.
    int waterRateSettingValue = 75;
    //컵의 용량
    long cupVolume;
    //따라야하는 물의 용량
    long waterVolume;
    //출수 버튼이 눌렸는가?
    bool getStartButtonDown;
    //설정 변경 버튼이 눌렸는가?
    bool getChangeButtonDonw;
    //출수 버튼이 눌렸는지 감지하는 함수
    getStartButtonDown = false;
    //설정 변경 버튼이 눌렸는지 감지하는 함수
    getChangeButtonDown = false;

    //출수 버튼이 눌린 것이 감지되었을때
    if(getStartButtonDown)
    {
        /**
         * @exception fixedServoAngle 이 물구멍에서 벗어나는 각도에 컵이 있으면 에러 발생시키기.

         */
        settingServoAngle();

        /**스텝모터 작동 시작
        * 내부에 upSensorByMotor 함수 작동
        * 초음파 센서를 위로 올리면서 컵의 평균 반지름 측정
        * 높이를 측정하여 부피 계산하는 함수
        */
        cupVolume = getVolume();

        //출수 시작 전 물의 양 결정
        waterVolume = cupVolume * (waterRateSettingValue/100);

        //정해진 물 용량을 출력하는 함수
        turnOnWater(waterVolume);
        
    }
    if (getChangeButtonDown)
    {
        waterRateSettingValue = changeWaterRateSetting(waterRateSettingValue);
    }
}
