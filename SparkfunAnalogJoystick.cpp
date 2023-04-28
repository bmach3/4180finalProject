/** Analog Joystick APIs
 *  Used as a user interface for Sparkfun Analog Joystick Module
 *  @file SparkfunAnalogJoystick.cpp
 *  @author Yuan,ZHANG; Jiajie,YANG
 */

#include "SparkfunAnalogJoystick.h"
#include "math.h"

#define M_PI 3.14159265358979323846

/** Constructor of Joystick object
 */
SparkfunAnalogJoystick::SparkfunAnalogJoystick(PinName vert, PinName horz, PinName sel): VERT(vert), HORZ(horz), SEL(sel)
{
    SEL.mode(PullUp);
}

//! Get the button status
int SparkfunAnalogJoystick::button(){
    return 1-SEL;
}

//! X axis value, reverse the value to get the x value in a normal Cartesian coordinate system
float SparkfunAnalogJoystick::xAxis(){
    return -(HORZ-0.5)*2;
}

//! Y axis value
float SparkfunAnalogJoystick::yAxis(){
    return (VERT-0.5)*2;
}

//! Calculate the angle value in a polar coordinate system
float SparkfunAnalogJoystick::angle(){
    float horz=-(HORZ-0.5)*2;
    float vert=(VERT-0.5)*2;
    float angle=0;
    if (vert==0&&horz>0) return 0;
    if (vert==0&&horz<0) return 180;
    if (horz==0&&vert>0) return 90;
    if (horz==0&&vert<0) return 270;
    if (horz>0) angle=atan(vert/horz)*180/M_PI;
    else angle=180+atan(vert/horz)*180/M_PI;
    if (angle<0) angle+=360;
    return angle;
}

//! Calculate the normalized distance value in a polar coordinate system
float SparkfunAnalogJoystick::distance(){
    float horz=-(HORZ-0.5)*2;
    float vert=(VERT-0.5)*2;
    float angle=SparkfunAnalogJoystick::angle();
    float oneAxis=tan(angle*M_PI/180.0);
    if (oneAxis<0) oneAxis=-oneAxis;
    if (oneAxis>1) oneAxis=1/oneAxis;
    float maxdistance=sqrt(1+oneAxis*oneAxis);
    float temp=horz*horz+vert*vert;
    return sqrt(temp)/maxdistance;
    
}