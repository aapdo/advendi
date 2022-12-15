#include<Stepper.h>

using namespace std;

double DISTANCE_WIDTH = 15;//정수기 전원을 켰을 때 측정하고 값을 변경하지 않음.

//오른쪽 초음파 센서 
const int ULTRA_RIGHT_TRIG = 11;//초음파 발사
const int ULTRA_RIGHT_ECHO = 12;//초음파 받음

//왼쪽 초음파 센서
const int ULTRA_LEFT_TRIG = 10;//초음파 발사
const int ULTRA_LEFT_ECHO = 13;//초음파 받음

//스텝 모터 한번당 회전하는 각도.
//Stepper 객체 레퍼런스
const int stepsPerRevolution = 256;

const int stepR_IN1 = 2;
const int stepR_IN2 = 4;
const int stepR_IN3 = 3;
const int stepR_IN4 = 5;

const int stepL_IN1 = 6;
const int stepL_IN2 = 8;
const int stepL_IN3 = 7;
const int stepL_IN4 = 9;

Stepper stepRight(stepsPerRevolution, stepR_IN1, stepR_IN2, stepR_IN3, stepR_IN4);
Stepper stepLeft(stepsPerRevolution, stepL_IN1, stepL_IN2, stepL_IN3, stepL_IN4);

//필요시 자료형 변환할 것.
double STEPPER_ANGLE = 11.25;

void setup(){
    //위 전역변수 값 설정
    Serial.begin(9600);

    stepRight.setSpeed(30); 
    stepLeft.setSpeed(30); 

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
int stepCount = 0;
double getVolume(){
    /*
    vector<double> heightDevided;
    vector<double radius[40];
    */
    double heightDevided[2] = {0, 0};
    //double heightDevided = 0.7;
    double radius[2] = {0, 0};

    //왼쪽 초음파, 오른쪽 초음파에서 구한 값이 된다.
    double distanceL, distanceR; 
    //스텝 모터가 몇 번 돌아갔는지 세는 변수.
    double cupVolume = 0.0;
    while (true)//정수기 가로길이보다 초음파 결과값이 크면 멈춰야함.
    {
        //distanceR = getDistance(ULTRA_RIGHT_TRIG, ULTRA_RIGHT_ECHO);
        distanceL = getDistance(ULTRA_LEFT_TRIG, ULTRA_LEFT_ECHO);
        /*
        Serial.print(stepCount);
        Serial.print(" left: ");
        Serial.print(distanceL);
        Serial.print(" right: ");   
        Serial.print(distanceR);
        */
     
        if (distanceL >= DISTANCE_WIDTH)
        {
            break;
        }
        radius[0] = radius[1];
        radius[1] = getRadius(distanceL, distanceL);

        heightDevided[0] = heightDevided[1];
        heightDevided[1] = 0.7*(stepCount);
/*
        Serial.print(" r: ");
        Serial.print(radius[1]);
        Serial.print(" h: ");
        Serial.print(heightDevided[1]);
        */

        //stepUp(stepRight);
        stepUp(stepLeft);
        ++stepCount;

        cupVolume += integralVolume(radius, heightDevided);
        /*
        Serial.print(" volume: ");
        Serial.print(cupVolume);

        Serial.println();
        */
    }
    return cupVolume;
}

/**
 * @brief level2
 * purpose: 스텝모터를 통해 센서를 조금씩 올려주는 함수
 * STEPPER_ANGLE을 참고하여 1call 당 1STEPPER_ANGLE씩 돌려야함.
 * @param Stpper 객체 레퍼런스를 사용함.
 */
void stepDown(Stepper stepper){
    stepper.step(stepsPerRevolution);
}
void stepUp(Stepper stepper){
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
    return DISTANCE_WIDTH - (Rdistance + Ldistance);
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
  return stepCount * 0.7;
}

//높이에 대한 반지름의 직선을 x축 회전시켜 얻은 회전체의 단면
double areaFunction(double x, double *heightDevided, double *radius) 
{
    return pow( (((radius[1] - radius[0]) / (heightDevided[1]-heightDevided[0])) * ( x - heightDevided[0]) + radius[0]) ,2)*PI; 
}
//위 함수에서 구한 원기둥들을 합하여 하나의 회전체의 부피를 구함.
double integrationVolume(double from, double to, double delta, double *heightDevided, double *radius)
{
    double sum = 0.;
    for (double x = from; x < to; x += delta) {
        sum += ((areaFunction(x, heightDevided, radius) + areaFunction(x + delta, heightDevided, radius)) / 2.);
        Serial.print(" sum: ");
        Serial.print(sum);
    }
    return abs(sum*delta);
}
//컵을 직선으로 분할해서 부피를 구함.
double integralVolume(double *heightDevided, double *radius)
{
    double delta = 0.25;

    double from = heightDevided[0];
    double to = heightDevided[1];

    return integrationVolume(from, to, delta, heightDevided, radius);
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

void reset(){
    for (int i = 0; i < stepCount; i++)
    {
        //stepDown(stepRight);
        stepDown(stepLeft);
    }
    stepCount = 0;
    
}

void loop(){
    //컵의 용량
    double cupVolume;
    //출수 버튼이 눌렸는가?
    bool getStartButtonDown = false;
    

    delay(500000);

    if(Serial.available()){
        bool getStartButtonDown = (bool)Serial.read();
        if(getStartButtonDown){
            cupVolume = getVolume();
            outputData(cupVolume);
            reset();
        }
    }
}
