#pragma once
#include <cmath>

#define EPSINON        0.00001
#define FloatEQ(a, b)  (std::fabs((a) - (b)) < EPSINON)  //Equal
#define FloatNE(a, b)  (!FloatEQ((a), (b)))         //Not Equal
#define FloatGT(a, b)  ((a) - (b) > EPSINON)        //Greater Than
#define FloatLT(a, b)  ((a) - (b) < -EPSINON)       //Less Than
#define FloatGE(a, b)  ((a) - (b) > -EPSINON)       //Greater Than or Equal
#define FloatLE(a, b)  ((a) - (b) < EPSINON)        //Less Than or Equal