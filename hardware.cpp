// This header has all the (extern) declarations of the globals.
// "extern" means "this is instantiated somewhere, but here's what the name
// means.
#include "globals.h"
#include "SparkfunAnalogJoystick.h"
#include "mpr121.h"
#include "SDFileSystem.h"
#include "hardware.h"
#include <cstdlib>

#define M_PI 3.14159265358979323846

// We need to actually instantiate all of the globals (i.e. declare them once
// without the extern keyword). That's what this file does!
DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);
DigitalOut led4(LED4);
// Hardware initialization: Instantiate all the things!
uLCD_4DGL uLCD(p13,p14,p11);             // LCD Screen (tx, rx, reset)
//SDFileSystem sd(p5, p6, p7, p8, "sd");  // SD Card(mosi, miso, sck, cs)
Serial pc(USBTX,USBRX);                 // USB Console (tx, rx)
MMA8452 acc(p28, p27, 100000);        // Accelerometer (sda, sdc, rate)


DigitalIn button1(p21);                 // Pushbuttons (pin)
DigitalIn button2(p22);
DigitalIn button3(p23);


AnalogOut DACout(p18);                  // Speaker (pin)
wave_player waver(&DACout);

SparkfunAnalogJoystick joystick(p17,p19,p20);
SDFileSystem sd(p5, p6, p7, p8, "sd");

I2C i2c(p9, p10);
InterruptIn interrupt(p26);
Mpr121 mpr121(&i2c, Mpr121::ADD_VSS);


int button;

// Some hardware also needs to have functions called before it will set up
// properly. Do that here.
int hardware_init()
{    
    //Initialize pushbuttons
    button1.mode(PullUp); 
    button2.mode(PullUp);
    button3.mode(PullUp);
    
    return ERROR_NONE;
}

void fallInterrupt() {
    int key_code=0;
    int i=0;
    int value=mpr121.read(0x00);
    value +=mpr121.read(0x01)<<8;
    // LED demo mod by J. Hamblen
    //pc.printf("MPR value: %x \r\n", value);
    i=0;
    // puts key number out to LEDs for demo
    for (i=0; i<12; i++) {
        led1 = 1;
        if (((value>>i)&0x01)==1) key_code=i+1;
        }
    button = key_code;
    led4=key_code & 0x01; // demo doesn't work? touchpad values don't show on LEDs
    led3=(key_code>>1) & 0x01;
    led2=(key_code>>2) & 0x01;
    led1=(key_code>>3) & 0x01;
    
}


GameInputs read_inputs() 
{
    GameInputs in;
    in.b1 = !button1; // Inverted, because low voltage means "pressed"
    in.b2 = !button2;
    in.b3 = !button3;
    // Read Accelerometer and record ax, ay, az in the GameInputs structure.
    // Complete this function (hint: lookup mbed Hardware > Components > find the accelerometer you are using)
    // Creates varaibles that we will pass into the readGravity function which we will use as input of main().
    float x;
    x=-joystick.xAxis();
    if (x < 0.5) {
        led1 = 1;
    } if (x > 0.4995) {
        led2 = 1;
    } if (x > 0.499) {
        led3 = 1;
    } if (x > 0.6) {
        led4 = 1;
    }
    in.x = x;
    int joystickButton; 
    joystickButton = joystick.button();
    in.joystickButton = joystickButton;
    interrupt.fall(&fallInterrupt);
    interrupt.mode(PullUp);
    in.touchpadValue = button;
    return in;
}

