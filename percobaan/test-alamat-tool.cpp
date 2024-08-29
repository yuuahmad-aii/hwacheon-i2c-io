#include <Arduino.h>
#include <Wire.h>

#define i2c_add 0x27

void setup()
{
    Serial.begin(115200);

    Wire.setSDA(PB11);
    Wire.setSCL(PB10);
    Wire.begin();

    pinMode(PC13, OUTPUT);

    Serial.println("Connect: ");
    delay(3000);

    // scan i2c
    byte error, address;
    int nDevices;

    Serial.println("Scanning...");

    nDevices = 0;
    for (address = 1; address < 127; address++)
    {
        Wire.beginTransmission(address);
        error = Wire.endTransmission();

        if (error == 0)
        {
            Serial.print("I2C device found at address 0x");
            if (address < 16)
                Serial.print("0");
            Serial.print(address, HEX);
            Serial.println("  !");

            nDevices++;
        }
        else if (error == 4)
        {
            Serial.print("Unknown error at address 0x");
            if (address < 16)
                Serial.print("0");
            Serial.println(address, HEX);
        }
    }
    if (nDevices == 0)
        Serial.println("No I2C devices found\n");
    else
        Serial.println("done\n");

    digitalWrite(PC13, HIGH); // turn the LED on (HIGH is the voltage level)
    delay(1000);              // wait for a second
    digitalWrite(PC13, LOW);  // turn the LED off by making the voltage LOW
    delay(3000);              // wait for a second
                              // delay(500); // wait 5 seconds for next scan
}

void loop()
{
    Wire.requestFrom(i2c_add, 1); // request 6 bytes from slave device #8

    while (Wire.available())
    {                           // slave may send less than requested
        int c = Wire.read();    // receive a byte as character
        Serial.println(c, BIN); // print the character
    }
    //  Serial.println(as5600.rawAngle() * AS5600_RAW_TO_DEGREES);
}