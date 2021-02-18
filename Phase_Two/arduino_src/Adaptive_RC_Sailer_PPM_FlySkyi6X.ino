/* Adaptive RC Sailer (02-16-2021) FlySky i6X Six Channel
  This software is distributed under the terms of the GNU GPLv3.0 License.
  Copyright (c) 2020-2021
  Author: Dave Kender <adaptivercsailing@adaptivercsailing.gmail.com>
  Visit web page at www.adaptivercsailer.com
  and GitHub at github.com/adaptivercsailing/adaptivercsailing

  Requires eRCaGuy_PPM_Writer-0.2.1
  eRCaGuy_PPM_Writer-0.2.1
  https://github.com/ElectricRCAircraftGuy/eRCaGuy_PPM_Writer
  Contributed by Gabriel Staples  https://www.electricrcaircraftguy.com/

  Requires Bounce2 if not already installed in Arduino library
  https://www.arduinolibraries.info/libraries/bounce2

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/* Additional Design Notes:

   Set Up for FlySky i6X Six Channel (with Normal Pulse Train Polarity) 
   Input Control Signals (From Adaptive Momentary ON/OFF Switches)
   Servos: Sails(In & Out) = Channel 3 : Rudder(Left & Right) = Channel 1 
   Output Signals (PPM on Arduino Pin 9) to RC Transmitter = TX Sails & TX Rudder
   **************************************CAUTION**********************************************
   *
   * Use voltage divider network or level shifter to reduce Arduino output voltage (5VDC) to FlySky input voltage (3.3 VDC)
   * Regardless, use at least a 1000 ohm resistor in series with the PPM input. 
   * 
   **************************************CAUTION**********************************************
   setPPMPolarity
      -PPM_WRITER_NORMAL has a HIGH base-line signal, with channelSpace pulses that are LOW 
      -PPM_WRITER_INVERTED has a LOW base-line signal, with channelSpace pulses that are HIGH  
      -to set the polarity to be inverted, for instance, call: setPPMPolarity(PPM_WRITER_INVERTED);
   This version does not automatically return the rudder to the center position.
   Serial Monitor Output is for debugging purposes only.
   Computer values are meant to indicate proper operation, NOT actual voltages.
*/

#include <Bounce2.h>
#include <eRCaGuy_PPM_Writer.h>
const byte CH1 = 0,
           CH2 = 1,
           CH3 = 2,
           CH4 = 3,
           CH5 = 4,
           CH6 = 5;

#define SailsOutSwitchPin 3                 // D3 Ring
#define SailsInSwitchPin 2                  // D2 Tip
int SailsIn;                                // SailsIn = Decrement
int SailsOut;                               // SailsOut = Increment
int SailsSyncDelay = 5;
int SailsSteps = 16;
int SailsPulseWidth = 1500;
int SailsPulseWidthInc = 40;
int SailsCountMin = -8;
int SailsCountMax = +8;
int SailsCount = 0;
int SailsCountDelay = 25;
Bounce debouncer1 = Bounce();
Bounce debouncer2 = Bounce();

#define RudderLeftSwitchPin 17              // A3 Ring
#define RudderRightSwitchPin 16             // A2 Tip
int RudderLeft;                             // RudderLeft = Decrement
int RudderRight;                            // RudderRight = Increment
int RudderSyncDelay = 5;
int RudderSteps = 16;
int RudderPulseWidth = 1500;
int RudderPulseWidthInc = 40;
int RudderCountMin = -8;
int RudderCountMax = +8;
int RudderCount = 0;
int RudderCountDelay = 25;
Bounce debouncer3 = Bounce();
Bounce debouncer4 = Bounce();

void sails_control()
{
  if (SailsIn == 0)
  {
    SailsCount = SailsCount - 1;
    if (SailsCount < SailsCountMin)
    {
      SailsCount = SailsCountMin;
    }
  }

  if (SailsOut == 0)
  {
    SailsCount = SailsCount + 1;
    if (SailsCount > SailsCountMax)
    {
      SailsCount = SailsCountMax;
    }
  }

  // New 02-20-2021 : See Adaptive RC Sailer PPM Design Notes
  PPMWriter.setChannelVal(CH3, (SailsPulseWidth + SailsCount * SailsPulseWidthInc) * 2);
  if (SailsIn == 0 || SailsOut == 0)
  {
    //Serial.print("Sails: Steps =  "); Serial.print(SailsCount);
    //Serial.print("  Control Pulse (msec) = "); Serial.println(SailsPulseWidth + SailsCount * SailsPulseWidthInc);
  }
  delay(SailsCountDelay);
}

void rudder_control()
{
  if (RudderLeft == 0)
  {
    RudderCount = RudderCount - 1;
    if (RudderCount <= RudderCountMin)
    {
      RudderCount = RudderCountMin;
    }
  }

  if (RudderRight == 0)
  {
    RudderCount = RudderCount + 1;
    if (RudderCount >= RudderCountMax)
    {
      RudderCount = RudderCountMax;
    }
  }

  // New 02-20-2021 : See Adaptive RC Sailer PPM Design Notes
  PPMWriter.setChannelVal(CH1, (RudderPulseWidth + RudderCount * RudderPulseWidthInc) * 2);
  if (RudderLeft == 0 || RudderRight == 0)
  {
    //Serial.print("Rudder: Steps =  "); Serial.print(RudderCount);
    //Serial.print("  Control Pulse (msec) = "); Serial.println(RudderPulseWidth + RudderCount * RudderPulseWidthInc);
  }
  delay(RudderCountDelay);
}

void setup()
{
  pinMode(SailsInSwitchPin, INPUT_PULLUP);
  pinMode(SailsOutSwitchPin, INPUT_PULLUP);
  pinMode(RudderLeftSwitchPin, INPUT_PULLUP);
  pinMode(RudderRightSwitchPin, INPUT_PULLUP);
  debouncer1.attach(SailsInSwitchPin); debouncer1.interval(5);
  debouncer2.attach(SailsOutSwitchPin); debouncer2.interval(5);
  debouncer3.attach(RudderLeftSwitchPin); debouncer3.interval(5);
  debouncer4.attach(RudderRightSwitchPin); debouncer4.interval(5);
  // Initially Set Channels 1 thru 6 to 1500 usec
  // Channel 3 = Sails; Channel 4 = Rudder
  // PPMWriter.setPPMPolarity(PPM_WRITER_INVERTED);
  PPMWriter.setNumChannels(6);
  PPMWriter.setChannelVal(CH1, 1500 * 2);
  PPMWriter.setChannelVal(CH2, 1500 * 2);
  PPMWriter.setChannelVal(CH3, 1500 * 2);
  PPMWriter.setChannelVal(CH4, 1500 * 2);
  PPMWriter.setChannelVal(CH5, 1500 * 2);
  PPMWriter.setChannelVal(CH6, 1500 * 2);
  // Start PPM Train
  PPMWriter.begin();
}

void loop()
{
  debouncer1.update(); SailsIn = debouncer1.read();
  debouncer2.update(); SailsOut = debouncer2.read();
  sails_control();
  debouncer3.update(); RudderLeft = debouncer3.read();
  debouncer4.update(); RudderRight = debouncer4.read();
  rudder_control();
}
