#include <Arduino.h>
#include <Wire.h>

#define AS5600_ADDRESS 0x36   // Alamat I2C untuk AS5600
#define AS5600_RAW_ANGLE 0x0C // Register untuk membaca raw angle

void setup()
{
    Serial.begin(9600);

    // Inisialisasi I2C kedua (Wire) pada pin PB10 (SCL) dan PB11 (SDA)
    Wire.setSDA(PB11);
    Wire.setSCL(PB10);
    Wire.begin();

    // Tunggu hingga serial monitor siap
    while (!Serial)
    {
        delay(10);
    }

    Serial.println("AS5600 Sensor Reading:");
    byte error, address;
    int nDevices;

    Serial.println("Scanning...");

    nDevices = 0;
    for (address = 1; address < 127; address++)
    {
        // The i2c_scanner uses the return value of
        // the Write.endTransmisstion to see if
        // a device did acknowledge to the address.
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
    delay(500);               // wait for a second
    digitalWrite(PC13, LOW);  // turn the LED off by making the voltage LOW
    delay(500);               // wait for a second
    // delay(500); // wait 5 seconds for next scan
    delay(5000);
}

uint16_t readAS5600RawAngle()
{
    uint16_t rawAngle = 0;

    Wire.beginTransmission(AS5600_ADDRESS);
    Wire.write(AS5600_RAW_ANGLE);
    Wire.endTransmission(false);

    Wire.requestFrom(AS5600_ADDRESS, 2);

    if (Wire.available() == 2)
    {
        rawAngle = Wire.read();                   // Baca MSB
        rawAngle = (rawAngle << 8) | Wire.read(); // Baca LSB dan gabungkan
    }

    return rawAngle;
}

void loop()
{
    uint16_t rawAngle = readAS5600RawAngle();
    float angle = (rawAngle * 360.0) / 4096.0; // Mengonversi raw angle ke derajat

    Serial.print("Raw Angle: ");
    Serial.print(rawAngle);
    Serial.print(" - Angle: ");
    Serial.print(angle);
    Serial.println(" degrees");

    // delay(500);
}
