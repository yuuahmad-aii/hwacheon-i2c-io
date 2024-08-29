#include <Arduino.h>
#include <Wire.h>
#include <bitset>

#define i2c_add 0x27   // ini khusus untuk absolute encoder
#define i2c_add_2 0x20 // ini untuk io lainnya

#define verbose 1
// #define scanningI2c 1
#define verbose_tools 1
#define verbose_tail 1
#define verbose_motor 1
// #define verbose_error 1
#define verbose_perintah_pc 1

unsigned char gerak_motor = 'C'; // A=CW, B=CCW, C=STOP
byte value_i2c_atc_encoder = 0b00000000;
byte value_i2c_io = 0b00000000;

// // variabel untuk parsing perintah serial
const byte numChars = 32;     // Panjang maksimal pesan yang diterima
char receivedChars[numChars]; // Buffer untuk menyimpan pesan yang diterima
bool newData = false;         // Menunjukkan apakah ada data baru yang diterima
char karakter_awal;
char perintah_pc = '0';

void setup()
{
    Serial.begin(115200);

    Wire.setSDA(PB11);
    Wire.setSCL(PB10);
    Wire.begin();

    pinMode(PC13, OUTPUT);

    Serial.println("Connect: ");
    delay(3000);

#ifdef scanningI2c
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
#endif
}

void verbose_output()
{
    // untuk proxy dan lainnya
    Serial.print("T:");
    Serial.print(value_i2c_io, BIN);
    // if (nilai_input[5])
    //     Serial.print("O"); // spindle orientation ok
    // if (nilai_input[0])
    //     Serial.print("F"); // umbrella didepan

// untuk pergerakan motor
#ifdef verbose_motor
    Serial.print("|M:");
    switch (gerak_motor)
    {
    case 'A':
        Serial.print("A");
        break;
    case 'B':
        Serial.print("B");
        break;
    case 'C':
        Serial.print("C");
        break;
    default:
        break;
    }
#endif

// untuk tools
#ifdef verbose_tools
    Serial.print("|P:");
    Serial.print(value_i2c_atc_encoder, BIN);
    // nilai_input[4] == 1 ? Serial.print("1") : Serial.print("0");
#endif

// untuk eroor
#ifdef verbose_error
    Serial.print("|E:");
    Serial.print("unknown");
#endif

// verbose tulis kembali perintah pc
#ifdef verbose_perintah_pc
    Serial.print("|R:");
    Serial.print(perintah_pc);
#endif

    Serial.println(); // enter, baris selanjutnya
}

void recvWithEndMarker()
{
    static byte ndx = 0;
    char endMarker = '\n';
    char rc;

    while (Serial.available() > 0 && newData == false)
    {
        rc = Serial.read();
        rc = toUpperCase(rc);

        if (rc != endMarker)
        {
            receivedChars[ndx] = rc;
            ndx++;
            if (ndx >= numChars)
                ndx = numChars - 1;
        }
        else
        {
            receivedChars[ndx] = '\0'; // Menandai akhir string
            ndx = 0;
            newData = true;
        }
    }
}

void parsing_perintah_pc()
{
    char *strtokIndx;

    // Mendapatkan karakter pertama dari buffer
    strtokIndx = strtok(receivedChars, ",");
    karakter_awal = strtokIndx[0];
    switch (strtokIndx[0])
    {
        // -----LIST PERINTAH DARI PC-----
        // Tool ATC Rotation CW (A)
        // Tool ATC Rotation CCW (B)
        // Stop Rotation (C)
        // Lock ATC (L)
        // Unlock ATC (U)
        // Check Tool Position (P:8 digit biner)
        // Tail Stock (T:1/0) (1 (Jemput) ditekan, 0 (Tinggal) dilepas)

    case 'J':
        perintah_pc = 'H';
        break;
    case 'T':
        perintah_pc = 'J';
        break;
    case 'L':
        perintah_pc = 'P';
        break;
    case 'U':
        perintah_pc = 'U';
        break;
    case 'A':
        perintah_pc = 'A';
        gerak_motor = 'A';
        break;
    case 'B':
        perintah_pc = 'B';
        gerak_motor = 'B';
        break;
    case 'C':
        perintah_pc = 'C';
        gerak_motor = 'C';
        break;
    default:
        break;
    }
}

void baca_sinyal_i2c()
{
    Wire.requestFrom(i2c_add, 1); // request 6 bytes from slave device #8

    while (Wire.available())
    {                                        // slave may send less than requested
        value_i2c_atc_encoder = Wire.read(); // receive a byte as character
        // std::bitset<8> binary(read_i2c_atc_encoder); // Konversi byte ke bitset 8 bit
        // Serial.println(binary.to_string().c_str());  // Print sebagai string dengan leading zeros
        // Serial.println(read_i2c_atc_encoder, BIN); // print the character
    }

    Wire.requestFrom(i2c_add_2, 1); // request 6 bytes from slave device #8

    while (Wire.available())
    {                               // slave may send less than requested
        value_i2c_io = Wire.read(); // receive a byte as character
        // std::bitset<8> binary(read_i2c_io);         // Konversi byte ke bitset 8 bit
        // Serial.println(binary.to_string().c_str()); // Print sebagai string dengan leading zeros
        // Serial.println(read_i2c_io, BIN); // print the character
    }
}

void gerakkan_motor()
{
    switch (gerak_motor)
    {
#if stepper_motor
    case 'A':
        my_stepper.setSpeed(3500);
        digitalWriteFast(digitalPinToPinName(dir_stepper), HIGH);
        my_stepper.runSpeed();
        break;
    case 'B':
        my_stepper.setSpeed(3500);
        digitalWriteFast(digitalPinToPinName(dir_stepper), LOW);
        my_stepper.runSpeed();
        break;
    case 'C':
        my_stepper.stop();
        my_stepper.setCurrentPosition(0);
        break;
    default:
        break;
#else
    case 'A':
        break;
    case 'B':
        break;
    case 'C':
        break;
    default:
        break;
#endif
    }
}

void loop()
{
    // recvWithEndMarker();
    // if (newData == true)
    // {
    //     parsing_perintah_pc();
    //     newData = false;
    // }
    baca_sinyal_i2c();
    verbose_output();
    // gerakkan_motor();
}