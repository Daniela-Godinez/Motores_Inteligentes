#include <Wire.h>

#define AS5600_ADDRESS 0x36
#define CONF_REG 0x07
#define ANGLE_REG 0x0E

const int analogPin = A0;

void setup() {
  Serial.begin(9600);
  Wire.begin();

  // Configuración inicial del modo analógico
  configureAnalogMode();
}

void loop() {
  // Leer ángulo vía I2C
  uint16_t angle = readAngle();
  float angleDegrees = (angle * 360.0) / 4096.0;
  Serial.print("Ángulo (I2C): ");
  Serial.println(angleDegrees);

  // Leer salida analógica
  int analogValue = analogRead(analogPin);
  float percentage = (analogValue / 1023.0) * 5.0;
  Serial.print("Analog Output (V): ");
  Serial.print(percentage);
  Serial.println("V");

  delay(1000);
}

void configureAnalogMode() {
  Wire.beginTransmission(AS5600_ADDRESS);
  Wire.write(CONF_REG);
  Wire.write(0x00); // MSB - Configuración básica
  Wire.write(0x10); // LSB - Configura salida en modo analógico
  Wire.endTransmission();
}

uint16_t readAngle() {
  Wire.beginTransmission(AS5600_ADDRESS);
  Wire.write(ANGLE_REG);
  Wire.endTransmission();

  Wire.requestFrom(AS5600_ADDRESS, 2);
  uint16_t angle = Wire.read() << 8 | Wire.read();

  return angle;
}
