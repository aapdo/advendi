#include <iostream>
#include <string>
using namespace std;


float function_x (float x){
    printf("return: %f\n", (0.1667*x)*(0.1667*x));
    //return (0.5 *x)*(0.5*x)*(3.141592);
    return (0.1667*x+5)*(0.1667*x+5)*3.141592;
}

float inte_0D(float from, float to, float delta){
    float sum = 0;
    for (float x = from; x < to; x += delta){
        printf("x: %f", x);
        sum += (function_x(x));
    }
    return sum*delta;
}

float inte_1D(float from, float to, float delta){
    float sum = 0;
    for (float x = from; x < to; x += delta){
        sum += ( (function_x(x) + function_x(x+delta)) /2  );
    }
    return sum*delta;
}

int main(){
    float from = 0.;
    float to = 10.;
    float delta = 0.25;

    printf("%f", inte_0D(from, to, delta));

/*
    for (delta = 1.; delta > 0.0001; delta /= 2){
        printf("delta = %f, 0: %f, 1d: %f\n", delta, inte_0D(from, to, delta), inte_1D(from, to, delta));
    }
*/
    return 0;
    
}
