// ***
// *** Pinout ATtiny25/45/85:
// *** PDIP/SOIC/TSSOP
// *** =============================================================================================
// ***
// ***        (PCINT5/RESET/ADC0/dW) PB5   [1]*  [8]   VCC
// *** (PCINT3/XTAL1/CLKI/OC1B/ADC3) PB3   [2]   [7]   PB2 (SCK/USCK/SCL/ADC1/T0/INT0/PCINT2)
// *** (PCINT4/XTAL2/CLKO/OC1B/ADC2) PB4   [3]   [6]   PB1 (MISO/DO/AIN1/OC0B/OC1A/PCINT1)
// ***                               GND   [4]   [5]   PB0 (MOSI/DI/SDA/AIN0/OC0A/OC1A/AREF/PCINT0)
// ***
#include <Arduino.h>
#include <Tiny4kOLED.h>

#define M1 3 //0
#define M2 1
#define EN 4
#define V_REF 4.4
#define MID 512
#define PAD 16

float R1 = 6800.0;
float R2 = 3800.0;
float input_voltage = 0.0;

void updateDisplay(uint8_t, bool);

void setup() { 
  analogReference(DEFAULT);
  pinMode(M1, OUTPUT);
  pinMode(M2, OUTPUT);
  pinMode(EN, OUTPUT);
  delay(200);

  oled.begin();  
  oled.setFont(FONT6X8);
  oled.clear();
  oled.switchRenderFrame();
  oled.clear();
  oled.switchRenderFrame();

  // Page 0
  oled.setCursor(15,0); // 13 pixels to center the following text
  oled.print(F("~ Lost Marbles ~")); // 17 characters x 6 pixels = 102 pixels

  // Page 1
  oled.setCursor(0,1);
  oled.fillToEOL(0x02);

  //  Page 2
  oled.setCursor(2,2);
  oled.print(F("Duty Cycle "));

  // Page 3
  oled.setCursor(0,3);
  oled.clearToEOL();

  oled.on();
  delay(200);

  updateDisplay(0, true);
}

void loop()
{
    int analog_value = analogRead(A0);
    int normalized_analog_value = analog_value - MID;
    uint8_t val = (abs(normalized_analog_value) - PAD) / 2;  

    if(normalized_analog_value > PAD) {
      digitalWrite(M1, HIGH);
      digitalWrite(M2, LOW);
    }
    else if(normalized_analog_value < -PAD) {
      digitalWrite(M1, LOW);
      digitalWrite(M2, HIGH);
    }
    else{
      digitalWrite(M1, LOW);
      digitalWrite(M2, LOW);
    }

    analogWrite(EN, val);

    float temp = V_REF * (R1+R2) / R2;
    input_voltage = analog_value * temp / 1024.0; 

    updateDisplay((uint8_t)val*100/255, normalized_analog_value > PAD);

    delay(500);
}


void updateDisplay(uint8_t duty_cycle, bool is_clockwise) {
  oled.setCursor(70,2); //11 characters x 6 plus 4 pixel spacer
  oled.print((unsigned char)duty_cycle);
  oled.setCursor(84,2);
  oled.print(F("%"));

  // Direction meter
  oled.setCursor(2,3);
  oled.clearToEOL();

  oled.startData();
  for (uint8_t i = duty_cycle/10+1; i > 0; i--) {     
    if(is_clockwise){
    //   0x00, 0x00, 0x41, 0x22, 0x14, 0x08,  ">"
      oled.sendData(0x00);
      oled.sendData(0x00);
      oled.sendData(0x41);
      oled.sendData(0x22);
      oled.sendData(0x14);
      oled.sendData(0x08);
    }
    else{
      //  0x00, 0x08, 0x14, 0x22, 0x41, 0x00 "<"
      oled.sendData(0x00);
      oled.sendData(0x08);
      oled.sendData(0x14);
      oled.sendData(0x22);
      oled.sendData(0x41);
      oled.sendData(0x00);
    }
  }
  oled.endData();
}