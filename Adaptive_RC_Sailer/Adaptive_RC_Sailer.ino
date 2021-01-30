/* Adaptive RC Sailer (12-28-2020)
  This software is distributed under the terms of the GNU GPLv3.0 License.
  Copyright (c) 2020
  Author: Dave Kender <adaptivercsailing@adaptivercsailing.gmail.com>
  Visit web page at www.adaptivercsailer.com
  and GitHub at github.com/adaptivercsailing/adaptivercsailing

  ###################################  IMPORTANT  ##############################
  ###                                                                        ###
  ###  Current design is based on 5.0 VDC radio system (e.g. HiTec LASER 4)  ###
  ###  If 3.3 VDC (e.g. FlySky)is used, see alternate PCB layout.            ###
  ###                                                                        ###
  ###  See additional design note below.                                     ###
  ###                                                                        ###
  ###################################  IMPORTANT  ##############################

  Requires bounce2 if not already installed in Arduino library
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

   Adaptive RC Sailer incorporates X9C103 Digital Potentiometers
   Input Control Signals (momentary ON/OFF): Sails(In & Out) and Rudder(Left & Right)
   Output Signals to RC Transmitter = TX Sails & TX Rudder
   Design Note: This version does not automatically return the rudder to the center position.

   X9C Notes:
   !INC =  1(Inc/Dec)  Vcc =   8
   U/!D =  2(Up/Dwn)   !CS =   7(Chip Select)
   Vh/Rh = 3           Vl/Vr = 6
   GND =   4           Vw/Rw = 5 (Output Signal)
   Note: Must keep !INC LOW while taking !CS HIGH
         U/!D may be changed while !CS is LOW

   Digital Pins D0 - D13 = Pin # 0 - 13
   Analog Pins  A0 - A5 = Pin # 14 - 19
   A0=14   A1=15   A2=16   A3=17   A4=18   A5=19

   X9C_A = Sail Control
   X9C_B = Rudder Control

   Serial Monitor Output is for debugging purposes only.
   Computer values are meant to indicate proper operation, NOT actual voltages.
*/

#include <Bounce2.h>

// Sails Control Variables X9C_A
#define X9C_A_Sails_ChipSelectPin7 11       // D11
#define X9C_A_Sails_UpDownPin2 15           // A1
#define X9C_A_Sails_IncDecPin1 14           // A0
#define SailsOutSwitchPin 3                 // D3 Ring
#define SailsInSwitchPin 2                  // D2 Tip
// X9C_A_Sails #A Pin#5=Sails Servo         // Tip
int SailsIn;                                // SailsIn = Decrement
int SailsOut;                               // SailsOut = Increment
int SyncDelay = 5;
int CountDelay = 10;
int SailsCount_n = 50;
int SailsCountMin = 20;
int SailsCountMax = 70;
Bounce debouncer1 = Bounce();
Bounce debouncer2 = Bounce();

// Rudder Control Variables X9C_B
#define X9C_B_Rudder_ChipSelectPin7 10      // D10
#define X9C_B_Rudder_UpDownPin2 5           // D5
#define X9C_B_Rudder_IncDecPin1 4           // D4
#define RudderLeftSwitchPin 17              // A3 Ring
#define RudderRightSwitchPin 16             // A2 Tip
// X9C_B_Rudder #B Pin#5=Rudder Servo       // Ring
int RudderLeft;                             // RudderLeft = Decrement
int RudderRight;                            // RudderRight = Increment
//int SyncDelay=5;
//int CountDelay=10;
int RudderCount_n = 50;
int RudderCountMin = 20;
int RudderCountMax = 70;
Bounce debouncer3 = Bounce();
Bounce debouncer4 = Bounce();

void initialize_sails()
// Initializes X9C_A_Sails to midrange position and indexes Counter to 50.
{
  for (int i = 0; i < 102; i++)
  {
    digitalWrite(X9C_A_Sails_ChipSelectPin7, LOW);
    delay(SyncDelay);
    digitalWrite(X9C_A_Sails_UpDownPin2, LOW);
    delay(SyncDelay);
    digitalWrite(X9C_A_Sails_IncDecPin1, LOW);
    delay(SyncDelay);
    digitalWrite(X9C_A_Sails_ChipSelectPin7, HIGH);
    delay(SyncDelay);
    digitalWrite(X9C_A_Sails_IncDecPin1, HIGH);
  }

  for (int i = 0; i < 50; i++)
  {
    digitalWrite(X9C_A_Sails_ChipSelectPin7, LOW);
    delay(SyncDelay);
    digitalWrite(X9C_A_Sails_UpDownPin2, HIGH);
    delay(SyncDelay);
    digitalWrite(X9C_A_Sails_IncDecPin1, LOW);
    delay(SyncDelay);
    digitalWrite(X9C_A_Sails_ChipSelectPin7, HIGH);
    delay(SyncDelay);
    digitalWrite(X9C_A_Sails_IncDecPin1, HIGH);
  }
}

void initialize_rudder()
// Initializes X9C_B_Rudder to midrange position and indexes Counter to 50.
{
  for (int i = 0; i < 102; i++)
  {
    digitalWrite(X9C_B_Rudder_ChipSelectPin7, LOW);
    delay(SyncDelay);
    digitalWrite(X9C_B_Rudder_UpDownPin2, LOW);
    delay(SyncDelay);
    digitalWrite(X9C_B_Rudder_IncDecPin1, LOW);
    delay(SyncDelay);
    digitalWrite(X9C_B_Rudder_ChipSelectPin7, HIGH);
    delay(SyncDelay);
    digitalWrite(X9C_B_Rudder_IncDecPin1, HIGH);
  }

  for (int i = 0; i < 50; i++)
  {
    digitalWrite(X9C_B_Rudder_ChipSelectPin7, LOW);
    delay(SyncDelay);
    digitalWrite(X9C_B_Rudder_UpDownPin2, HIGH);
    delay(SyncDelay);
    digitalWrite(X9C_B_Rudder_IncDecPin1, LOW);
    delay(SyncDelay);
    digitalWrite(X9C_B_Rudder_ChipSelectPin7, HIGH);
    delay(SyncDelay);
    digitalWrite(X9C_B_Rudder_IncDecPin1, HIGH);
  }
}

void sails_control()
{
  if (SailsIn == 0)
  {
    SailsCount_n = SailsCount_n - 1;
    if (SailsCount_n <= SailsCountMin)
    {
      SailsCount_n = SailsCount_n + 1;
      goto SailsExit;
    }
    digitalWrite(X9C_A_Sails_ChipSelectPin7, LOW);
    delay(SyncDelay);
    digitalWrite(X9C_A_Sails_UpDownPin2, LOW);
    delay(SyncDelay);
    digitalWrite(X9C_A_Sails_IncDecPin1, LOW);
    delay(SyncDelay);
    digitalWrite(X9C_A_Sails_ChipSelectPin7, HIGH);
    delay(SyncDelay);
    digitalWrite(X9C_A_Sails_IncDecPin1, HIGH);
    Serial.print("nCount(Sails In) = "); Serial.println(SailsCount_n);
    delay(CountDelay);
  }

  if (SailsOut == 0)
  {
    SailsCount_n = SailsCount_n + 1;
    if (SailsCount_n >= SailsCountMax)
    {
      SailsCount_n = SailsCount_n - 1;
      goto SailsExit;
    }
    digitalWrite(X9C_A_Sails_ChipSelectPin7, LOW);
    delay(SyncDelay);
    digitalWrite(X9C_A_Sails_UpDownPin2, HIGH);
    delay(SyncDelay);
    digitalWrite(X9C_A_Sails_IncDecPin1, LOW);
    delay(SyncDelay);
    digitalWrite(X9C_A_Sails_ChipSelectPin7, HIGH);
    delay(SyncDelay);
    digitalWrite(X9C_A_Sails_IncDecPin1, HIGH);
    Serial.print("nCount(Sails Out) = "); Serial.println(SailsCount_n);
    delay(CountDelay);
  }
SailsExit: {}
}

void rudder_control()
{
  if (RudderLeft == 0)
  {
    RudderCount_n = RudderCount_n - 1;
    if (RudderCount_n <= RudderCountMin)
    {
      RudderCount_n = RudderCount_n + 1;
      goto RudderExit;
    }
    digitalWrite(X9C_B_Rudder_ChipSelectPin7, LOW);
    delay(SyncDelay);
    digitalWrite(X9C_B_Rudder_UpDownPin2, LOW);
    delay(SyncDelay);
    digitalWrite(X9C_B_Rudder_IncDecPin1, LOW);
    delay(SyncDelay);
    digitalWrite(X9C_B_Rudder_ChipSelectPin7, HIGH);
    delay(SyncDelay);
    digitalWrite(X9C_B_Rudder_IncDecPin1, HIGH);
    Serial.print("nCount(Rudder Left) = "); Serial.println(RudderCount_n);
    delay(CountDelay);
  }

  if (RudderRight == 0)
  {
    RudderCount_n = RudderCount_n + 1;
    if (RudderCount_n >= RudderCountMax)
    {
      RudderCount_n = RudderCount_n - 1;
      goto RudderExit;
    }
    digitalWrite(X9C_B_Rudder_ChipSelectPin7, LOW);
    delay(SyncDelay);
    digitalWrite(X9C_B_Rudder_UpDownPin2, HIGH);
    delay(SyncDelay);
    digitalWrite(X9C_B_Rudder_IncDecPin1, LOW);
    delay(SyncDelay);
    digitalWrite(X9C_B_Rudder_ChipSelectPin7, HIGH);
    delay(SyncDelay);
    digitalWrite(X9C_B_Rudder_IncDecPin1, HIGH);
    Serial.print("nCount(Rudder Right) = "); Serial.println(RudderCount_n);
    delay(CountDelay);
  }
RudderExit: {}
}

void setup()
{
  pinMode(SailsInSwitchPin, INPUT_PULLUP);
  pinMode(SailsOutSwitchPin, INPUT_PULLUP);
  pinMode(X9C_A_Sails_ChipSelectPin7, OUTPUT);
  pinMode(X9C_A_Sails_UpDownPin2, OUTPUT);
  pinMode(X9C_A_Sails_IncDecPin1, OUTPUT);
  pinMode(RudderLeftSwitchPin, INPUT_PULLUP);
  pinMode(RudderRightSwitchPin, INPUT_PULLUP);
  pinMode(X9C_B_Rudder_ChipSelectPin7, OUTPUT);
  pinMode(X9C_B_Rudder_UpDownPin2, OUTPUT);
  pinMode(X9C_B_Rudder_IncDecPin1, OUTPUT);
  debouncer1.attach(SailsInSwitchPin); debouncer1.interval(5);
  debouncer2.attach(SailsOutSwitchPin); debouncer2.interval(5);
  debouncer3.attach(RudderLeftSwitchPin); debouncer3.interval(5);
  debouncer4.attach(RudderRightSwitchPin); debouncer4.interval(5);

  Serial.begin(115200);
  Serial.println("X9C_A_Sails103 Sails Demo with Prototype PCB 12-26-2020");
  Serial.print(" nCount @ Voltage Minimum = "); Serial.print(SailsCountMin); Serial.println("%");
  Serial.print(" nCount @ Voltage Maximum = "); Serial.print(SailsCountMax); Serial.println("%");
  Serial.println("X9C_B_Rudder103 Rudder Demo with Prototype PCB 12-26-2020");
  Serial.print(" nCount @ Voltage Minimum = "); Serial.print(RudderCountMin); Serial.println("%");
  Serial.print(" nCount @ Voltage Maximum = "); Serial.print(RudderCountMax); Serial.println("%");

  initialize_sails();
  initialize_rudder();
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
