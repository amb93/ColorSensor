/*
 MIT License

Copyright (c) 2022 Developium

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
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

 * Arduino library for the Vishay VEML3328 and VEML3328sl sensors
 * funtions:
 * bool RGBCIR.begin(void); look for the sensor in the I2C bus, return 0 if sensor not found
 * void RGBCIR.Enable(void); Turn on the sensor'channels (off by default)
 * void Disable(void); Turn off the sensor
 * uint16_t getRed(void); get the red channel's reading from 0-65535
 * uint16_t getGreen(void); get the green channel's reading from 0-65535
 * uint16_t getBlue(void); get the blue channel's reading from 0-65535
 * uint16_t getClear(void); get the clear channel's reading from 0-65535
 * uint16_t getIR(void); get the IR channel's reading from 0-65535
 * void setSensitivity(bool); toggle between two sensitivity modes; 0 for low sensitivity and 1 for high
 * void setGain(float); set the gain of the sensor, choose between 0.5, 1, 2, 4 (1 default)
 * void setDG(uint8_t); set the digital gain of the sensor, choose between 1, 2, 4 (1 default)
 * void setIntegrationTime(uint16_t); set the integration time of the sensor, choose between IT_50MS, IT_100MS, IT_200MS, IT_400MS (in milliseconds)
 * uint16_t getCommandReg(void); Read the value of the command register
 * 
 * refer to the datasheet: https://www.vishay.com/docs/80012/veml3328sl.pdf
 * 
 * By: amb93
 */

#include "Wire.h"
#include "VEML3328.h"

VEML3328 RGBCIR;

float R_Cal=1.0;
float G_Cal=1.0;
float B_Cal=1.0;

char incomingByte = '1';

float Red;
float Green;
float Blue;
float Hue;
float Val;
float Sat;

void setup() {
  Serial.begin(9600);
  pinMode(4,OUTPUT);
  digitalWrite(4,HIGH);
  Wire.begin(); 
  if(!RGBCIR.begin()) {
    Serial.println("ERROR: couldn't detect the sensor");
    while(1){}
  }
  Serial.println("Vishay VEML3328 RGBCIR color sensor");
  RGBCIR.Enable(); 
  RGBCIR.setGain(4);
  RGBCIR.setSensitivity(high_sens);
  RGBCIR.setDG(1);
  RGBCIR.setIntegrationTime(IT_50MS);

  delay(1000);
}

void loop() {

   if (Serial.available() > 0) {
    // read the incoming byte:
    incomingByte = Serial.read();

    // say what you got:
      if (incomingByte == '0')
      {
        R_Cal = RGBCIR.getRed();
        G_Cal = RGBCIR.getGreen();
        B_Cal = RGBCIR.getBlue();
      }
  }
  
  // Use the arduino's serial plotter

  
  Red = float(RGBCIR.getRed())/R_Cal;
  Green = float(RGBCIR.getGreen())/G_Cal;
  Blue = float(RGBCIR.getBlue())/B_Cal;
  Hue = RGB2H(Red,Green,Blue);
  Val = RGB2V(Red,Green,Blue);
  Sat = RGB2S(Red,Green,Blue);
  
  Serial.print(Sat*255);   //read blue channel
  Serial.print(','); 
  Serial.print(Val*255);    //read red channel
  Serial.print(',');  
  Serial.println(Hue);  //read green channel 
  //Serial.print(',');  
  //Serial.print(RGBCIR.getClear());  //read clear channel
  //Serial.print(',');  
  //Serial.println(RGBCIR.getIR());   //read IR channel
  delay(50);
}

float RGB2H(float r,float g,float b)
{
  float Cmax = max(max(r,g),b);
  float Cmin = min(min(r,g),b);
  float diff = Cmax - Cmin;
  float h;
  if (diff == 0)
  {
    return 0;
  }
  else if (Cmax == r)
  {
    h = (g - b) / diff ;
     
  }
  else if (Cmax == g)
  {
     h = 2.0 + (b - r) / diff ;
     
  }

    else if (Cmax == b)
  {
     h = 4.0 + (r - g) / diff ;
  }
  h *= 60.0; 
  if (h<0.0) h +=360.0;
  
  return h;
}

float RGB2S(float r,float g,float b)
{
  float Cmax = max(max(r,g),b);
  float Cmin = min(min(r,g),b);
  float diff = Cmax - Cmin;
  float S;
  float V;
  V = Cmax;
  S =   (Cmax-Cmin)/Cmax;
  if (V==0) S=0;
  
  return S;
}

float RGB2V(float r,float g,float b)
{
  float Cmax = max(max(r,g),b);
  float V;
  V = Cmax;
  return V;
}
