#include <string>
#include<Servo.h>
#include<Stepper.h>
#include<math.h>

using namespace std;

double PI = M_PI;
long DISTANCE_WIDTH = 20;//정수기 전원을 켰을 때 측정하고 값을 변경하지 않음.
long fixedServoAngle;//서보모터를 돌리면서 마지막으로 고정된 서보모터의 각도

//오른쪽 초음파 센서 
int ULTRA_RIGHT_TRIG;//초음파 발사
int ULTRA_RIGHT_ECHO;//초음파 받음

//왼쪽 초음파 센서
int ULTRA_LEFT_TRIG;//초음파 발사
int ULTRA_LEFT_ECHO;//초음파 받음

//스텝 모터 한번당 회전하는 각도.
//Stepper 객체 레퍼런스
Stepper stepRight(stepsPerRevolution,4,2,3,1);;
Stepper stepLeft(stepsPerRevolution,8,6,7,5);;

const int STEPS_PER_REVOLUTION = 64;
//필요시 자료형 변환할 것.
int STEPPER_ANGLE;



void setup(){
    //위 전역변수 값 설정
    Serial.begin(9600);

    stepRight.setSpeed(14); 
    stepLeft.setSpeed(14); 
}




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

    }else if (distanceL < distanceR)//왼쪽이 작으면 왼쪽으로 돌려야함
    {

    }else{

    }
}


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
    vector<long> heightDevided;
    vector<long> radius;

    long distanceL, distanceR; //왼쪽 초음파, 오른쪽 초음파에서 구한 값이 된다.
    long cupVolume;//컵의 반지름
    long heightCup;//컵 높이
    //스텝 모터가 몇 번 돌아갔는지 세는 변수.
    int stepCount = 0;
    while ((distanceR >= DISTANCE_WIDTH))//정수기 가로길이보다 초음파 결과값이 크면 멈춰야함.
    {
        distanceL = getDistance(ULTRA_LEFT_TRIG, ULTRA_LEFT_ECHO);
        distanceR = getDistance(ULTRA_RIGHT_TRIG, ULTRA_RIGHT_ECHO);
        stepUp(stepRight);
        stepUp(stepLeft);

        //반지름을 구해서 마지막 원소에 붙임.
        radius.push_back(getRadius(distanceL, distanceR));
        //step이 몇 번 돌아갔는지에 따라 높이가 점점 커지면서 들어갈 것임.
        heightDevided.push_back(getHeight(stepCount));

        ++stepCount;
    }

    return integralVolume(heightDevided, radius);
    
}

/**
 * @brief level2
 * purpose: 스텝모터를 통해 센서를 조금씩 올려주는 함수
 * STEPPER_ANGLE을 참고하여 1call 당 1STEPPER_ANGLE씩 돌려야함.
 * @param Stpper 객체 레퍼런스를 사용함.
 */
void stepUp(Stepper stepper){
    // 360/32 = 30 도씩 돌아감. 
    // 원주가 19니까 19/(30/360) = 1.5cm 
    stepper.step(stepsPerRevolution);
    ++stepCount;
    delay(100);
}
void stepDown(Stepper stepper){
    stepper.step(-stepsPerRevolution);
}
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
 * @param stepCount 
 * @return long 
 */
long getHeight(int stepCount);




//부피를 구하기 위한 직선 함수.
float volumeFunction(float x, vector<long> heightDevided, vector<long> radius) 
{
    return pow(((radius[i+1] - radius[i]) / (heightDevided[i+1]-heightDevided[i])) * ( x - heightDevided[i]) + radius[i],2)PI; 
}
//
float integrationVolume(float from, float to, float delta, vector<long> heightDevided, vector<long> radius)
{
    float sum = 0.;
    for (float x = from; x < to; x += delta) {
        sum += ((volumeFunction(x, heightDevided, radius) + volumeFunction(x + delta, heightDevided, radius)) / 2.);
    }
    // 여기 에러 날거같은데..?
    return abs(sum delta);
}
//컵을 직선으로 분할해서 부피를 구함.
float integralVolume(vector<long> heightDevided, vector<long> radius)
{

    float sum = 0;
    float delta = 0.0001;
    for(int i = 0;i<(sizeof(radius)/sizeof(*radius))-1; i++)
    {
        float from = heightDevided[i];
        float to = heightDevided[i + 1];
        float a = integrationVolume(from, to, delta, heightDevided, radius);
        sum = sum + a;
    }

    return sum;
}


//반대편 아두이노로 정보를 출력하는 함수
void outputData(int data);
void outputData(string data);
void outputData(long data);

void loop(){
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
