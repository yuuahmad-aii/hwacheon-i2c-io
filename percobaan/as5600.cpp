#include <Arduino.h>
#include "AS5600.h"

AS5600L as5600(0x36); //  use default Wire

void setup()
{
    Serial.begin(115200);
    Serial.println(__FILE__);
    Serial.print("AS5600_LIB_VERSION: ");
    Serial.println(AS5600_LIB_VERSION);

    Wire.setSDA(PB11);
    Wire.setSCL(PB10);
    Wire.begin();

    Serial.print(as5600.getAddress());
    as5600.setDirection(AS5600_CLOCK_WISE); //  default, just be explicit.
    int b = as5600.isConnected();
    Serial.print("Connect: ");
    Serial.println(b);
}

void loop()
{
    Serial.println(as5600.readAngle());
    // Serial.print("\t");
    // Serial.println(as5600.rawAngle());
    //  Serial.println(as5600.rawAngle() * AS5600_RAW_TO_DEGREES);
}