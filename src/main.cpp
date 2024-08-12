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

#define M1 3 
#define M2 1
#define EN 4
#define MID 512
#define PAD 0

void updateDisplay(uint8_t, bool);

void setup() { 
  Serial.begin(9600);

  analogReference(DEFAULT);
  pinMode(M1, OUTPUT);
  pinMode(M2, OUTPUT);
  pinMode(EN, OUTPUT);
  delay(200);

  oled.begin();  
  oled.setFont(FONT6X8P);
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

  oled.on();
  delay(500);

  updateDisplay(0, true);
  
  delay(500);
}

void loop()
{
    int analog_value = analogRead(A0)+1;
    analog_value = analog_value > 1023 ? 1023 : analog_value;
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

    delay(250);

    updateDisplay((uint8_t)val*100/255, normalized_analog_value > PAD);
    
    delay(250);

    Serial.print(analog_value);
    Serial.print(" [");
    Serial.print(val);
    Serial.print("]");
    Serial.println();
}

void updateDisplay(uint8_t duty_cycle, bool is_clockwise) {

  // Page 2 : Duty Cycle
  oled.setCursor(0,2); 
  oled.clearToEOL();
  oled.setCursor(48,2); 
  oled.print(F("Duty Cycle"));  
  oled.setCursor(2,2); 
  oled.print((unsigned char)duty_cycle);
  oled.setCursor(22,2);
  oled.print(F(" %"));

  // Page 3 : Direction meter
  oled.setCursor(0,3);
  oled.clearToEOL();
  oled.setCursor(2,3);

  for (uint8_t i = duty_cycle/10; i > 0; i--)
      oled.print(is_clockwise ? F("<") : F(">"));
}