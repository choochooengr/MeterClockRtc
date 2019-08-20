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
//*     Date  will read Month : Day : Year (20XX)
//*
//*****************************************************************************
//* Digital    : 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13
//* Analog     : A0, A1, A2, A3, A4, A5
//* Serial     : 0 (RX), 1 (TX)
//* Interrupts : 2 (Int 0), 3(Int 1). 
//* PWM        : 3, 5, 6, 9, 10, 11.
//* I2C        : A4 (I2C/SDA), A5 (I2C/SCL). 
//* SPI        : 10 (SPI/SS), 11 (SPI/MOSI), 12 (SPI/MISO), 13 (SPI/SCK).
//* LED        : 13 
//* Reset      : Bring this line LOW to reset the microcontroller.
//*
//*****************************************************************************
//*

//********************
// Includes:
//********************
#include <Wire.h>
#include "RTClib.h"



//********************
// Definitions:
//********************

// Arduino UNO I/O port Definitions:
// Digital PWM pins on UNO 3, 5, 6, 9, 10, and 11 for disply meters.
#define PIN_HR10S       11   // Hours  10's position
#define PIN_HR1S        10   // Hours  1's  position
#define PIN_MIN10S       9   // Minute 10's position
#define PIN_MIN1S        6   // Minute 1's  position
#define PIN_SEC10S       5   // Second 10's position
#define PIN_SEC1S        3   // Second 1's  position

// Digital I/O pins on UNO 2, 4, 7, 8, 12, 13 for LEDs.
#define PIN_LED1         2   // 
#define PIN_LED2         4   // 
#define PIN_LED3         7   // 
#define PIN_LED4         8   // 

// Analog input pins for time and date setting.
#define PIN_RUN_SET     A0   // 
#define PIN_DATE_TIME   A1   //
#define PIN_HR_YEAR     A2   //
#define PIN_MIN_MONTH   A3   //
#define PIN_SEC_DAY     A4   //
//#define PIN_          A5   //



//********************
// Global Variables:
//********************
RTC_Millis rtc;
DateTime currentTime;
byte debounceTime      = 10;

byte dampingValue      = 100;

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

bool runSet    =  0;
bool dateTime  =  0;
byte setYear   = 19; 
byte setMonth  =  1; 
byte setDay    =  1;
byte setHour   =  0;
byte setMinute =  0;
byte setSecond =  0;


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
    // Debounce run/set selector.
    if( analogRead( PIN_RUN_SET ) > 512  )
    {
        // Switch signal high
        // Wait for switch debounce
        delay( debounceTime );

        // Re-read switch after debounce time and 
        // if still high, set signal to high, else low.
        if( analogRead( PIN_RUN_SET ) > 512 )             
            runSet = 1;
        else
            runSet = 0; 
    }
    else
    {
        // Switch signal low.
        runSet = 0;
    }


    // Determine if run or set is selected
    if( runSet == 1 )
    {
        Serial.println( " run ");
        // Read current time from RTC (real time clock)
        readRtc();

        // Was used for debugging
        //printTimeToConsole();

        // Convert time values to corresponding PWM value.
        mapPwmValues();

        // Send the PWM values to the reapective meters.
        writeToMeters();

        // Set display LED pattern.
        writeToLeds();
    }
    else // Set was selected.
    {
        Serial.println( " set ");

        setTime();
    }
              
}


//********************************************************************************
//*                     
//********************************************************************************
void readRtc()
{
    byte hr12;

    
    // Read RTC and load current time values into time count.
    currentTime = rtc.now();

    // Change time from 24 to 12 hour format
    if( currentTime.hour() > 12 )
        hr12 = currentTime.hour() - 12;
    else
        hr12 = currentTime.hour();
    
    hour10sCount = hr12 / 10;
    hour1sCount  = hr12 % 10;
  
    min10sCount  = currentTime.minute() / 10;
    min1sCount   = currentTime.minute() % 10;
    sec10sCount  = currentTime.second() / 10;
    sec1sCount   = currentTime.second() % 10;

    // Add 1 to count to allow needle to go to full scale.
    // The needle return takes one sec anyway. We never see 0.
    sec1sCount++; 


    // This routine was used for debugging. 
    // May be needed as features are added.
    Serial.print( hr12,   DEC );
    Serial.print( " : ");
    Serial.print( currentTime.minute(), DEC );
    Serial.print( " : ");
    Serial.print( currentTime.second(), DEC );
    Serial.println();
}


//********************************************************************************
//*                     
//********************************************************************************
void setCount()
{
    // load current time values into time count.
    hour10sCount = setHour   / 10;
    hour1sCount  = setHour   % 10;
    min10sCount  = setMinute / 10;
    min1sCount   = setMinute % 10;
    sec10sCount  = setSecond / 10;
    sec1sCount   = setSecond % 10;

    // Add 1 to count to allow needle to go to full scale.
    // The needle return takes one sec anyway. We never see 0.
    sec1sCount++; 
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
        analogWrite( PIN_SEC1S,  (calcVal * float(loop)) + float(sec1sValueStart ) );
        //Serial.println( (calcVal * float(loop)) + float(sec1sValueStart ), DEC);

       
        //delay( 100 );
        delay( dampingValue );
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
//*                     
//********************************************************************************
void setTime()
{
    byte hr12;
    
    bool hourYearOld = 0;
    bool hourYearNew = 0;
    
    bool minMonthOld  = 0;
    bool minMonthNew  = 0;

    bool secDayOld  = 0;
    bool secDayNew  = 0;

    bool runSetOld  = 0;
    bool runSetNew  = 0;


    // Collect current time from RTC
    currentTime = rtc.now();
    setYear   = currentTime.year();
    setMonth  = currentTime.month();
    setDay    = currentTime.day();
    setHour   = currentTime.hour(); 
    setMinute = currentTime.minute(); 
    setSecond = currentTime.second();

    // Change time from 24 to 12 hour format
    if( currentTime.hour() > 12 )
        setHour = currentTime.hour() - 12;
    else
        setHour = currentTime.hour();

        

    dampingValue      = 10;

    while( runSet == 0 )
    {

        // Debounce date/time selector.
        if( analogRead( PIN_DATE_TIME ) > 512  )
        {
            delay( debounceTime );
            if( analogRead( PIN_DATE_TIME ) > 512 )             
                dateTime = 1;
            else
                dateTime = 0; 
        }
        else
            dateTime = 0; 
            

        // Debounce hour selector.
        hourYearNew = ( analogRead( PIN_HR_YEAR ) > 512 );
        if( hourYearNew != hourYearOld )
        {
            hourYearOld = hourYearNew;
            delay( debounceTime );
            hourYearNew = ( analogRead( PIN_HR_YEAR ) > 512 );
            if( hourYearNew == hourYearOld && hourYearNew == 0 )
            {
                if( dateTime == 0 )
                {                
                    setHour++;
                    if( setHour >= 13 )
                        setHour = 0;
                }
                else
                {                
                    setYear++;
                    if( setYear >= 30 )
                        setYear = 0;
                }                
            }
        }


        // Debounce minute selector.
        minMonthNew = ( analogRead( PIN_MIN_MONTH ) > 512 );
        if( minMonthNew != minMonthOld )
        {
            minMonthOld = minMonthNew;
            delay( debounceTime );
            minMonthNew = ( analogRead( PIN_MIN_MONTH ) > 512 );
            if( minMonthNew == minMonthOld && minMonthNew == 0 )
            {
              if( dateTime == 0 )
                {                
                    setMinute++;
                    if( setMinute >= 60 )
                        setMinute = 0;
                }
                else
                {                
                    setMonth++;
                    if( setMonth >= 30 )
                        setMonth = 0;
                } 
            }
        }


        // Debounce second selector.
        secDayNew = ( analogRead( PIN_SEC_DAY ) > 512 );
        if( secDayNew != secDayOld )
        {
            secDayOld = secDayNew;
            delay( debounceTime );
            secDayNew = ( analogRead( PIN_SEC_DAY ) > 512 );
            if( secDayNew == secDayOld && secDayNew == 0 )
            {
                if( dateTime == 0 )
                {                
                    setSecond++;
                    if( setSecond >= 60 )
                        setSecond = 0;
                }
                else
                {                
                    setDay++;
                    if( setDay >= 31 )
                        setDay = 0;
                } 
            }
        }


        if( dateTime == 0 )
            Serial.print( "Date: " );
        else
            Serial.print( "Time: " );
       
        Serial.print( setYear, DEC );
        Serial.print( " : " );
        Serial.print( setMonth, DEC );
        Serial.print( " : " );
        Serial.print( setDay, DEC);
        Serial.print( "         " );

        Serial.print( setHour, DEC );
        Serial.print( " : " );
        Serial.print( setMinute, DEC );
        Serial.print( " : " );
        Serial.print( setSecond, DEC );
        Serial.println( "  " );


        setCount();
        mapPwmValues();
        writeToMeters();


        // Debounce run/set selector.
        if( analogRead( PIN_RUN_SET ) > 512  )
        {
            delay( debounceTime );
            if( analogRead( PIN_RUN_SET ) > 512 )             
                runSet = 1;
            else
                runSet = 0; 
        }
        else
            runSet = 0;        
        
    }

        dampingValue      = 100;

        Serial.print( "Final Time Set: ");
        Serial.print( setHour, DEC);
        Serial.print( " : ");
        Serial.print( setMinute, DEC);
        Serial.print( " : ");
        Serial.print( setSecond, DEC);
        Serial.println( "  ");
    
    // Caclulate DateTime structure( 2000+year, month, day, hour, min, sec );
    setSecond = 0;
    DateTime dt ( setYear, setMonth, setDay, setHour, setMinute, setSecond );
    
    // This routine sets the date and time 
    rtc.adjust( dt );    

}


    
 

//********************************************************************************
//*                 End of File
//********************************************************************************

