/*                            attiny45/85
(reset)   (D5/A0/ADC0)      pb5 -+---+- power 
(LED +ve) (D3/A3/ADC3)      pb3 -+*  +- pb2 (OLED SCL)   (D2/A1/SCL/SCK)
(MIC +ve) (D4/A2/ADC2)      pb4 -+   +- pb1 (buzz -ve) (D1/MISO)
(LED, ELECTRET MIC -ve)  ground -+---+- pb0 (OLED SDA)   (D0/SDA/MOSI)

INPORTANT: Connect A 50K Ohm Resistor Between +ve Of Electret Microphone (pb4) And 
Power, To Power The Microphone. The Value Of This Microphone Adjusts The 
Sensitivity Of The Microphone. The Greater The Resistor, The More Sensitive
The Microphone. Note, That If The Resistor Is Too Large, The Microphone Will 
Not Get Enough Power (And Thus Become Less Sensitive).  

The Louder The Volume Sensed By The Electret Mic, The Higher The 
Voltage Read By pb4 (Analog Input 2).
We Can Also Use The Electret Mic As A Wind Sensor, So Blowing On Microphone Is 
Equivalent To Loud Sound.*/

#include <TinyWireM.h>
#include <Tiny4kOLED.h>
#include "pitches.h"

int buzz = 1;
int mic = 2;  // A2
int led = 3;

// For Mic
int micSample;           // Volume Picked Up By Microphone
int maxSample = 0;       // Maximum Volume For That Sample
int totalMaxVolume = 0;  // Maximum Volume Picked Up By Microphone
int averageVolume = 0;   // Average Volume For Background Sound

// For Buzzer
// We will Set Up An Array To Store Our Notes In The Melody
int melody[] = 
{
  NOTE_C4_1, NOTE_C4, NOTE_D4, NOTE_C4, NOTE_F4, NOTE_E4,
  NOTE_C4_1, NOTE_C4, NOTE_D4, NOTE_C4, NOTE_G4, NOTE_F4,
  NOTE_C4_1, NOTE_C4, NOTE_C5, NOTE_A4, NOTE_F4, NOTE_F4,
  NOTE_E4, NOTE_D4, NOTE_AS4, NOTE_AS4, NOTE_A4,
  NOTE_F4, NOTE_G4, NOTE_F4
};

// Note Durations: 4 = Quarter Note, 8 = Eighth Note, etc.:
int noteDurations[] = 
{
  4, 4, 2, 2, 2, 1,
  4, 4, 2, 2, 2, 1,
  4, 4, 2, 2, 4, 4,
  2, 1, 4, 4, 2, 2, 2, 1
};

/* sizeof Gives The Number Of Bytes. 
Each int Value Is Composed Of Two Bytes (Becasuse They Are 16 bits)
There Are Two Values (16 + 16) Per Note (Pitch And Duration).
So For Each Note There Are Four Bytes */
int notes = sizeof(melody) / sizeof(melody[0]) / 2;

// This Calculates The Duration Of A Whole Note In ms
int divider = 0, noteDuration = 0;

void setup() {

  pinMode(buzz, OUTPUT);  // Sings Happy Birthday
  pinMode(led, OUTPUT);   // Turns Off After Blowing

  digitalWrite(led, HIGH);

  //For OLED
  oled.begin(128, 64, sizeof(tiny4koled_init_128x64br), tiny4koled_init_128x64br);

  oled.on();
  oled.clear();

  oled.setFontX2(FONT6X8);

  oled.setCursor(15, 0);
  oled.print("Happiest");

  oled.setCursor(15, 11);
  oled.print("Birthday");

  oled.setCursor(39, 14);
  oled.print("EVER");

  // For Buzzer
  for (int thisNote = 0; thisNote < 26; thisNote++) 
  {

    /* To Calculate The Note Duration, Take One Second Divided By The
    Note Type. Quarter Note = 1000 / 4, Eighth Note = 1000/8, etc.*/
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(buzz, melody[thisNote], noteDuration);

    int pauseBetweenNotes = noteDuration + 80;  // Delay Between Pulse
    delay(pauseBetweenNotes);
  }

  // OLED Part - 2
  oled.clear();
  oled.setFont(FONT8X16);

  oled.setCursor(4, 0);
  oled.print("Make A Wish And");

  oled.setCursor(12, 11);
  oled.print("Blow Away The");

  oled.setCursor(41, 30);
  oled.print("Candle");

  // For Microphone
  // Calibrate For Average Volume By Taking 100 Samples And Averaging
  for (int i = 0; i < 100; i++) 
  {
    micSample = micSample + analogRead(mic);
  }
  averageVolume = micSample / 100;
  averageVolume = averageVolume + 4;  // Adjust Silence Threshold.
  // Increase This Number (The 4) To Make Microphone Less Sensitive To Noise.
}

void loop() 
{
  // For Microphone
  maxSample = 0;  // Reset The Maximum Volume For This Sample

  // Take 20 Samples Of Volume And Pick The Maximum Volume
  for (int i = 0; i < 20; i++) 
  {
    micSample = analogRead(mic);
    if (micSample > maxSample) 
    {
      maxSample = micSample;
    }
  }

  // Update Maximum Volume Read By The Microphone For Calibration
  if (maxSample > totalMaxVolume) 
  {
    totalMaxVolume = maxSample;
  }

  // Blow Air On The Sensor. 
  if (maxSample > averageVolume + 30) 
  {
    digitalWrite(led, LOW);

    // Congratulatory Message
    oled.clear();

    oled.setFont(FONT8X16);

    oled.setCursor(3, 0);
    oled.print("Congratulations");

    oled.setCursor(6, 11);
    oled.print("Happy Birthday");

    oled.setCursor(46, 30);
    oled.print("Name");  // Center The Name Of The Person Using Trial And Error

    // Secret Message
    delay(10000);

    for (int i = 0; i < 5; i++) 
    {
      digitalWrite(led, LOW);
      delay(200);
      digitalWrite(led, HIGH);
      delay(200);
    }

    // Wait Till Threshold Increases
    while (analogRead(mic) <= averageVolume + 40) 
    {
      // Wait Until A Loud Sound (Air Blow) Is Detected
    }

    digitalWrite(led, LOW);

    
    oled.clear();
    oled.setFont(FONT8X16);

    oled.setCursor(4, 0);
    oled.print("Enter A Secret");

    oled.setCursor(10, 12);
    oled.print("Message Here");
  }
}