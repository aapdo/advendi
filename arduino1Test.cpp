#include<Stepper.h>

using namespace std;

double DISTANCE_WIDTH = 20;//정수기 전원을 켰을 때 측정하고 값을 변경하지 않음.

//오른쪽 초음파 센서 
int ULTRA_RIGHT_TRIG = 11;//초음파 발사
int ULTRA_RIGHT_ECHO = 12;//초음파 받음

//왼쪽 초음파 센서
int ULTRA_LEFT_TRIG = 10;//초음파 발사
int ULTRA_LEFT_ECHO = 13;//초음파 받음

//스텝 모터 한번당 회전하는 각도.
//Stepper 객체 레퍼런스
const int stepsPerRevolution = 64;  //원래 64;

Stepper stepRight(stepsPerRevolution, 2, 3, 4, 5);
Stepper stepLeft(stepsPerRevolution, 6, 7, 8, 9);

//필요시 자료형 변환할 것.
double STEPPER_ANGLE = 11.25;

void setup(){
    //위 전역변수 값 설정
    Serial.begin(9600);

    stepRight.setSpeed(14); 
    stepLeft.setSpeed(14); 

    pinMode(ULTRA_RIGHT_TRIG, OUTPUT);
    pinMode(ULTRA_RIGHT_ECHO, INPUT);
    pinMode(ULTRA_LEFT_TRIG, OUTPUT);
    pinMode(ULTRA_LEFT_ECHO, INPUT);
}


/*
 * @brief 공통부
 * 
 * @return long 
 */

//purpose: 초음파 센서로 거리 측정하고 그 값을 리턴.
double getDistance(int TRIG_PIN_NUMBER, int ECHO_PIN_NUMBER){
    double duration, distance;
    digitalWrite(TRIG_PIN_NUMBER, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN_NUMBER, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN_NUMBER, LOW);

    duration = pulseIn(ECHO_PIN_NUMBER, HIGH); //물체에 반사되어돌아온 초음파의 시간을 변수에 저장합니다.

    //cm 로 환산
    distance = duration * 17 / 1000;

    return distance;
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
double getVolume(){
    /*
    vector<double> heightDevided;
    vector<double radius[40];
    */
    double heightDevided[50];
    double radius[50];

    //왼쪽 초음파, 오른쪽 초음파에서 구한 값이 된다.
    double distanceL, distanceR; 
    //스텝 모터가 몇 번 돌아갔는지 세는 변수.
    int stepCount = 0;
    while (true)//정수기 가로길이보다 초음파 결과값이 크면 멈춰야함.
    {
        distanceR = getDistance(ULTRA_RIGHT_TRIG, ULTRA_RIGHT_ECHO);
        distanceL = getDistance(ULTRA_LEFT_TRIG, ULTRA_LEFT_ECHO);
        if (distanceR >= DISTANCE_WIDTH)
        {
            break;
        }
        
        radius[stepCount] = getRadius(distanceL, distanceR);
        heightDevided[stepCount] = getHeight(stepCount);

        stepUp(stepRight);
        stepUp(stepLeft);
        ++stepCount;

        delay(1000);
    }
    return integralVolume(heightDevided, radius, stepCount);
    
}

/**
 * @brief level2
 * purpose: 스텝모터를 통해 센서를 조금씩 올려주는 함수
 * STEPPER_ANGLE을 참고하여 1call 당 1STEPPER_ANGLE씩 돌려야함.
 * @param Stpper 객체 레퍼런스를 사용함.
 */
// 한 번 작동시마다 11.25도만큼 움직임 ( 각도 변환 가능 )
void stepUp(Stepper stepper){
    // 360/32 = 30 도씩 돌아감. 
    // 원주가 19니까 19/(30/360) = 1.5cm 
    // 작동 한번에 0.59cm
    stepper.step(stepsPerRevolution);
}
void stepDown(Stepper stepper){
    stepper.step(-stepsPerRevolution);
    delay(300);
}
/**
 * @brief level2
 * purpose: 좌우의 초음파 센서에서 측정된 값을 바탕으로 컵의 반지름을 구하는 함수.
 * @param distanceL 
 * @param distanceR 
 * @return long 
 */
double getRadius(double Rdistance, double Ldistance){
    return 20.0 - (Rdistance + Ldistance);
    
}

//cnt 와
//전역변수: STEPPER_ANGLE 로 
//높이를 구하는 함수.
/**
 * @brief level2
 * purpose: 스텝모터의 회전당 올라가는 높이와 회전 수를 곱하여 컵의 높이를 구하는 함수.
 * @param stepCount 
 * @return long 
 */
double getHeight(int stepCount){
  return stepCount * 0.59;
}



int i = 0;

//부피를 구하기 위한 직선 함수.
double volumeFunction(double x, double *heightDevided, double *radius) 
{
    return pow( (((radius[i+1] - radius[i]) / (heightDevided[i+1]-heightDevided[i])) * ( x - heightDevided[i]) + radius[i]) ,2)*PI; 
}
//
double integrationVolume(double from, double to, double delta, double *heightDevided, double *radius)
{
    double sum = 0.;
    for (double x = from; x < to; x += delta) {
        sum += ((volumeFunction(x, heightDevided, radius) + volumeFunction(x + delta, heightDevided, radius)) / 2.);

    }
    return abs(sum*delta);
}
//컵을 직선으로 분할해서 부피를 구함.
double integralVolume(double *heightDevided, double *radius, int cnt)
{
    double sum = 0;
    double delta = 0.25;
    for(i = 0;i<cnt-1; i++)
    {
        double from = heightDevided[i];
        double to = heightDevided[i + 1];
        double a = integrationVolume(from, to, delta, heightDevided, radius);
        sum = sum + a;
    }

    return sum;
}

//반대편 아두이노로 정보를 출력하는 함수
void outputData(int data){
    Serial.print(data);
}
void outputData(bool data){
    Serial.print(data);
}
void outputData(double data){
    Serial.print(data);
}

void loop(){
    //컵의 용량
    double cupVolume;
    //출수 버튼이 눌렸는가?
    bool getStartButtonDown = false;

    //출수 버튼이 눌린 것이 감지되었을때
    if(Serial.available()){
        bool getStartButtonDown = (bool)Serial.read();
        if(getStartButtonDown){
            cupVolume = getVolume();
            Serial.print(0);
            outputData(cupVolume);
        }
    }
}
