/** Analog Joystick APIs
 *  Used as a user interface for Sparkfun Analog Joystick Module
 *  @file SparkfunAnalogJoystick.h
 *  @author Yuan,ZHANG; Jiajie,YANG
 */

#ifndef SPARKFUN_ANALOG_JOYSTICK
#define SPARKFUN_ANALOG_JOYSTICK

#include "mbed.h"

/** Joystick class.
 *  User Interface to use Sparkfun Joystick Module
 */
class SparkfunAnalogJoystick
{
	
public:
	// Constructor of Joystick object
	SparkfunAnalogJoystick(PinName, PinName, PinName);
	// Get the button status, 1 for on and 0 for off
	int button();
	// X axis value
	float xAxis();
	// Y axis value
	float yAxis();
	// Angle value in polar coordinates
	float angle();
	// Distance value in polar coordinates
	float distance();
	
protected:
    AnalogIn VERT;
    AnalogIn HORZ;
    DigitalIn SEL;

};

#endif
