//*****************************************************************************
//*
//*                          Arduino Sketch File 
//*
//*
//*     FILE NAME      : AmmeterClock
//*     DATE OF ORIGIN : July 2019
//*     PROJECT        : AmmeterClock
//*
//*     Clock will read HH : MM : SS
//*
//*****************************************************************************
//* Serial     : 0 (RX), 1 (TX)
//* Interrupts : 2, 3. 
//* PWM        : 3, 5, 6, 9, 10, 11. Provide 8-bit PWM output with the analogWrite() function.
//* I2C        : A4 (SDA), A5 (SCL). 
//* SPI        : 10 (SS), 11 (MOSI), 12 (MISO), 13 (SCK).
//* LED        : 13 
//* Reset      : Bring this line LOW to reset the microcontroller.
//*
//*****************************************************************************
//*

// Includes:
//********************
#include <Wire.h>
#include "RTClib.h"


// Definitions:
//********************
// Arduino UNO I/O port Definitions:
// AnalogWrite pins on UNO 3, 5, 6, 9, 10, and 11 for disply meters.
#define PIN_HR10S    11    // Hours  10's position
#define PIN_HR1S     10    // Hours  1's  position
#define PIN_MIN10S    9    // Minute 10's position
#define PIN_MIN1S     6    // Minute 1's  position
#define PIN_SEC10S    5    // Second 10's position
#define PIN_SEC1S     3    // Second 1's  position

#define DAMPING_VALUE 10

// DIO pins on UNO 2, 4, 7, 8, 12, 13 for LEDs.


// Global Variables:
//********************
RTC_Millis rtc;
DateTime currentTime;

byte hour10sCount      = 0;
byte hour1sCount       = 0;
byte min10sCount       = 0;
byte min1sCount        = 0;
byte sec10sCount       = 0;
byte sec1sCount        = 0;

byte hour10sValueStart = 0;
byte hour1sValueStart  = 0;
byte min10sValueStart  = 0;
byte min1sValueStart   = 0;
byte sec10sValueStart  = 0;
byte sec1sValueStart   = 0;

byte hour10sValueEnd   = 0;
byte hour1sValueEnd    = 0;
byte min10sValueEnd    = 0;
byte min1sValueEnd     = 0;
byte sec10sValueEnd    = 0;
byte sec1sValueEnd     = 0;
  

//********************************************************************************
//*                 Required setup routine, program start
//********************************************************************************
void setup() 
{
    // Open the serial port at 9600 bps.
    Serial.begin( 9600 );

    // The following line sets the RTC to the date & time the sketch was compiled.
    rtc.begin( DateTime(__DATE__, __TIME__) );  
}



//********************************************************************************
//*         Main program loop            
//********************************************************************************
void loop() 
{
    // Read current time from RTC (real time clock)
    readRtc();

    // Was used for debugging
    printTimeToConsole();

    // Convert time values to corresponding PWM value.
    mapPwmValues();

    // Send the PWM values to the reapective meters.
    writeToMeters();

    // Set display LED pattern.
    writeToLeds();
              
}


//********************************************************************************
//*                     
//********************************************************************************
void readRtc()
{
    // Read RTC and load current time values into time count.

    currentTime = rtc.now();
   
    hour10sCount = currentTime.hour()   / 10;
    hour1sCount  = currentTime.hour()   % 10;
    min10sCount  = currentTime.minute() / 10;
    min1sCount   = currentTime.minute() % 10;
    sec10sCount  = currentTime.second() / 10;
    sec1sCount   = currentTime.second() % 10;
}



//********************************************************************************
//*                     
//********************************************************************************
void mapPwmValues()
{
    // Map the individual time position value (0-9) to a PWM output value (0-255)
    hour10sValueEnd = map( hour10sCount, 0, 10, 0, 255 );
    hour1sValueEnd  = map( hour1sCount,  0, 10, 0, 255 ); 
    min10sValueEnd  = map( min10sCount,  0, 10, 0, 255 );
    min1sValueEnd   = map( min1sCount,   0, 10, 0, 255 );
    sec10sValueEnd  = map( sec10sCount,  0, 10, 0, 255 );
    sec1sValueEnd   = map( sec1sCount,   0, 10, 0, 255 );
}



//********************************************************************************
//*                     
//********************************************************************************
void writeToMeters()
{
    byte loop;
    float calcVal;


    // Note: A useful side effect of the rollover from max PWM value to 0 is
    // the offset calculations become negative. When added to the current
    // PWM value the meter slowly returns to 0 instead of slaming the needle.
     
    // Create a needle movement dampening loop to prevent meter damage.
    // Larger dampening values slow meter movement dampend movement.
    for( loop = 0; loop < 10; loop++ )
    {
      
        // Set hours
        calcVal = ( float(hour10sValueEnd - hour10sValueStart) / 10.0 );
        analogWrite( PIN_HR10S,  (calcVal * float(loop)) + float(hour10sValueStart ) );
    
        calcVal = ( float(hour1sValueEnd - hour1sValueStart) / 10.0 );
        analogWrite( PIN_HR1S,  (calcVal * float(loop)) + float(hour1sValueStart ) );


        // Set minutes
        calcVal = ( float(min10sValueEnd - min10sValueStart) / 10.0 );
        analogWrite( PIN_MIN10S,  (calcVal * float(loop)) + float(min10sValueStart ) );
    
        calcVal = ( float(min1sValueEnd - min1sValueStart) / 10.0 );
        analogWrite( PIN_MIN1S,  (calcVal * float(loop)) + float(min1sValueStart ) );


        // Set seconds
        calcVal = ( float(sec10sValueEnd - sec10sValueStart) / 10.0 );
        analogWrite( PIN_SEC10S,  (calcVal * float(loop)) + float(sec10sValueStart ) );
    
        calcVal = ( float(sec1sValueEnd - sec1sValueStart) / 10.0 );
        Serial.println( (calcVal * float(loop)) + float(sec1sValueStart ), DEC);
        analogWrite( PIN_SEC1S,  (calcVal * float(loop)) + float(sec1sValueStart ) );

       
        delay( 100 );
    }

    // Final value may be off slightly due to integer math in loop calculations above.
    // Make final adjustments. 
    analogWrite( PIN_HR10S,  hour10sValueEnd );
    analogWrite( PIN_HR1S,   hour1sValueEnd );
    analogWrite( PIN_MIN10S, min10sValueEnd );
    analogWrite( PIN_MIN1S,  min1sValueEnd );
    analogWrite( PIN_SEC10S, sec10sValueEnd );
    analogWrite( PIN_SEC1S,  sec1sValueEnd );

    // The start of the next meter PWM setting is the end of the current PWM setting.
    hour10sValueStart = hour10sValueEnd;
    hour1sValueStart  = hour1sValueEnd;
    min10sValueStart  = min10sValueEnd;
    min1sValueStart   = min1sValueEnd;
    sec10sValueStart  = sec10sValueEnd;
    sec1sValueStart   = sec1sValueEnd;
    
}


//********************************************************************************
//*                     
//********************************************************************************
void printTimeToConsole()
{
    // This routine was used for debugging. 
    // May be needed as features are added.
    Serial.print( currentTime.hour(),   DEC );
    Serial.print( " : ");
    Serial.print( currentTime.minute(), DEC );
    Serial.print( " : ");
    Serial.print( currentTime.second(), DEC );
/*
    Serial.print( " >>>>> ");
    
    Serial.print( hour10sCount, DEC );
    Serial.print( hour1sCount,  DEC );
    Serial.print( " : ");
    Serial.print( min10sCount,  DEC );
    Serial.print( min1sCount,   DEC );
    Serial.print( " : ");
    Serial.print( sec10sCount,  DEC );
    Serial.print( sec1sCount,   DEC );
*/
    Serial.println();
}

    
//********************************************************************************
//*                     
//********************************************************************************
void writeToLeds()
{
    // This routine makes some LEDs blink on the front panel.     

}

 

//********************************************************************************
//*                 End of File
//********************************************************************************

