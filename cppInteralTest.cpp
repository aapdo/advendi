#include <iostream>
#include <string>
#include <cmath>
using namespace std;
float heightDevided[4] = { 0,1,2,3 }; //x좌표
float radius[4] = {1,1,1,1}; //y좌표
double PI = 3.141592;
int i = 0;

float function_x(float x) 
{
    return pow(((radius[i+1] - radius[i]) / (heightDevided[i+1]-heightDevided[i])) * ( x - heightDevided[i]) + radius[i],2)PI; 
}

float integration_1D(float from, float to, float delta)
{
    float sum = 0.;
    for (float x = from; x < to; x += delta) {
        sum += ((function_x(x) + function_x(x + delta)) / 2.);
    }
    return abs(sum*delta);
}

float getVolume()
{

    float sum = 0;
    float delta = 0.0001;
    for(i = 0;i<(sizeof(radius)/sizeof(*radius))-1; i++)
    {
        float from = heightDevided[i];
        float to = heightDevided[i + 1];
        float a = integration_1D(from, to, delta);
        sum = sum + a;

    }

    return sum;

}

int main(){
    cout << getVolume << endl;
    return 0;
}