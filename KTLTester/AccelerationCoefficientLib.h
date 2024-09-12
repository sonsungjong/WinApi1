#ifndef ACCELERATION_COEFFICIENT_LIB_H_
#define ACCELERATION_COEFFICIENT_LIB_H_

#define BoltzmannEVPerK					8.617333262145e-5
#define INIPATH									"./set.ini"

extern "C" __declspec(dllexport) double runAcceleration(double dbInputTemp, double dbInputHumidity, double dbInputVibration, double dbInputImpulse);


#endif				ACCELERATION_COEFFICIENT_LIB_H_