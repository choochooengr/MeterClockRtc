//*****************************************************************************
//*
//*                          Arduino Sketch File 
//*
//*
//*     FILE NAME      : AmmeterClock
//*     DATE OF ORIGIN : January 2018
//*     PROJECT        : AmmeterClock
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

byte hour10sCount = 0;
byte hour1sCount  = 0;
byte min10sCount  = 0;
byte min1sCount   = 0;
byte sec10sCount  = 0;
byte sec1sCount   = 0;

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
  


//*****************************************************************************
//*                 Required setup routine, program start
//*****************************************************************************
void setup() 
{
    // Open the serial port at 9600 bps.
    Serial.begin(9600);

    // The following line sets the RTC to the date & time the sketch was compiled.
    rtc.begin(DateTime(__DATE__, __TIME__));
}



//**************************************
//*         Main program loop            
//**************************************
void loop() 
{
    // Read current time from RTC 
    readRtc();

    //printToConsole();

    // Convert time values to corresponding PWM value.
    processPwmValues();

    // Send the PWM values to the meters
    writeToMeters();

    // Set display LED pattern.
    writeToLeds();
              
}


//**************************************
//*                     
//**************************************
void readRtc()
{
    // Read RTC and load values into time count.

    DateTime now = rtc.now();
   
    hour10sCount =   now.hour() / 10;
    hour1sCount  =   now.hour() % 10;
    min10sCount  = now.minute() / 10;
    min1sCount   = now.minute() % 10;
    sec10sCount  = now.second() / 10;
    sec1sCount   = now.second() % 10;
}



//**************************************
//*                     
//**************************************
void processPwmValues()
{
    hour10sValueEnd = map( hour10sCount, 0, 10, 0, 255 );
    hour1sValueEnd  = map( hour1sCount,  0, 10, 0, 255 ); 
    min10sValueEnd  = map( min10sCount,  0, 10, 0, 255 );
    min1sValueEnd   = map( min1sCount,   0, 10, 0, 255 );
    sec10sValueEnd  = map( sec10sCount,  0, 10, 0, 255 );
    sec1sValueEnd   = map( sec1sCount,   0, 10, 0, 255 );
}



//**************************************
//*                     
//**************************************
void writeToMeters()
{
    byte loop;
    byte dampingCount = 0;


  
    // Loop to dampen meter movement to prevent meter damage.
    // Loop is 100 mS. Larger value further dampend movement.
    for( loop = 0; loop < DAMPING_VALUE; loop++ )
    {
/*       
      for( int dampingCount = lastMeasurement; dampingCount <= newMeasurement; dampingCount++ )
      {
          analogWrite( pwmPin, count2 );
          delay( 10 );
      }

      lastMeasurement = newMeasurement;
 
        if( hour10sValueEnd  == 0 )
            analogWrite( PIN_HR10S,  hour10sValueStart - (((hour10sValueEnd - hour10sValueStart) / DAMPING_VALUE) * loop) );
        else 
            analogWrite( PIN_HR10S,  (((hour10sValueEnd - hour10sValueStart) / DAMPING_VALUE) * loop) + hour10sValueStart );


        if( hour1sValueEnd  == 0 )
            analogWrite( PIN_HR1S,   hour1sValueStart - (((hour1sValueEnd  - hour1sValueStart)  / DAMPING_VALUE) * loop) );
        else 
            analogWrite( PIN_HR1S,   (((hour1sValueEnd  - hour1sValueStart)  / DAMPING_VALUE) * loop) + hour1sValueStart  );


        if( min10sValueEnd  == 0 )
            analogWrite( PIN_MIN10S, min10sValueStart - (((min10sValueEnd  - min10sValueStart)  / DAMPING_VALUE) * loop) );
        else 
            analogWrite( PIN_MIN10S, (((min10sValueEnd  - min10sValueStart)  / DAMPING_VALUE) * loop) + min10sValueStart );


        if( min1sValueEnd  == 0 )
            analogWrite( PIN_MIN1S,  min1sValueStart - (((min1sValueEnd   - min1sValueStart)   / DAMPING_VALUE) * loop) );
        else 
            analogWrite( PIN_MIN1S,  (((min1sValueEnd   - min1sValueStart)   / DAMPING_VALUE) * loop) + min1sValueStart );


        if( sec10sValueEnd  == 0 )
            analogWrite( PIN_SEC10S, sec10sValueStart - (((sec10sValueEnd  - sec10sValueStart)  / DAMPING_VALUE) * loop) );
        else 
            analogWrite( PIN_SEC10S, (((sec10sValueEnd  - sec10sValueStart)  / DAMPING_VALUE) * loop) + sec10sValueStart );
*/

        if( sec1sValueEnd  == 0 )
        {
            analogWrite( PIN_SEC1S,  sec1sValueStart - (((sec1sValueEnd   - sec1sValueStart)   / DAMPING_VALUE) * loop) );
            Serial.print( "Down: ");
            Serial.print( sec1sValueStart - (((sec1sValueEnd   - sec1sValueStart)   / DAMPING_VALUE) * loop), DEC );
            Serial.println();
        }
        else 
        {
            analogWrite( PIN_SEC1S,  (((sec1sValueEnd   - sec1sValueStart)   / DAMPING_VALUE) * loop) + sec1sValueStart );
            Serial.print( "  Up: ");
            Serial.print( (((sec1sValueEnd   - sec1sValueStart)   / DAMPING_VALUE) * loop) + sec1sValueStart, DEC );
            Serial.println();
        }
        
        
        delay( 100 );
    }

    // Make final adjustments.
    analogWrite( PIN_HR10S,  hour10sValueEnd );
    analogWrite( PIN_HR1S,   hour1sValueEnd );
    analogWrite( PIN_MIN10S, min10sValueEnd );
    analogWrite( PIN_MIN1S,  min1sValueEnd );
    analogWrite( PIN_SEC10S, sec10sValueEnd );
    analogWrite( PIN_SEC1S,  sec1sValueEnd );


    hour10sValueStart = hour10sValueEnd;
    hour1sValueStart  = hour1sValueEnd;
    min10sValueStart  = min10sValueEnd;
    min1sValueStart   = min1sValueEnd;
    sec10sValueStart  = sec10sValueEnd;
    sec1sValueStart   = sec1sValueEnd;
    
}


//**************************************
//*                     
//**************************************
void printToConsole()
{
    DateTime now = rtc.now();
    Serial.print( now.hour(),   DEC );
    Serial.print( " : ");
    Serial.print( now.minute(), DEC );
    Serial.print( " : ");
    Serial.print( now.second(), DEC );

    Serial.print( " >>>>> ");
    
    Serial.print( hour10sCount, DEC );
    Serial.print( hour1sCount,  DEC );
    Serial.print( " : ");
    Serial.print( min10sCount,  DEC );
    Serial.print( min1sCount,   DEC );
    Serial.print( " : ");
    Serial.print( sec10sCount,  DEC );
    Serial.print( sec1sCount,   DEC );

    Serial.println();
}

    
//**************************************
//*                     
//**************************************
void writeToLeds()
{
    

}

 

//*****************************************************************************
//*                 End of File
//*****************************************************************************
