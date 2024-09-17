#include <Arduino.h>
#include <Wire.h>
#include <bitset>

#define i2c_add 0x27 // ini khusus untuk absolute encoder
// #define i2c_add_2 0x20 // ini untuk io lainnya

#define SCANNING_I2C 1
#define VERBOSE_TOOLS 1
#define VERBOSE_TAIL 1
// #define VERBOSE_MOTOR 1
// #define VERBOSE_ERROR 1
#define VERBOSE_PERINTAH_PC 1

unsigned char gerak_motor = 'C'; // A=CW, B=CCW, C=STOP
int value_i2c_atc_encoder = 0;

// // variabel untuk parsing perintah serial
const byte num_chars = 32;      // Panjang maksimal pesan yang diterima
char received_chars[num_chars]; // Buffer untuk menyimpan pesan yang diterima
bool new_data = false;          // Menunjukkan apakah ada data baru yang diterima
char karakter_awal;
char perintah_pc = '0';
bool kirim_verbose = false;

// pin input
#define INPUT_LOCK_ATC PB5
#define INPUT_TAIL_DEPAN PB9
#define INPUT_TAIL_BELAKANG PB8
#define INPUT_PEDAL PB3
// pin output
#define OUTPUT_LOCK_ATC PB14
#define OUTPUT_CW_ATC PB15
#define OUTPUT_TAIL_JEMPUT PC15
#define OUTPUT_TAIL_TINGGAL PA15

void setup()
{
  Serial.begin(115200);

  Wire.setSDA(PB11);
  Wire.setSCL(PB10);
  Wire.begin();

  // pinmode output
  pinMode(PC13, OUTPUT); // lampu indikator
  pinMode(OUTPUT_LOCK_ATC, OUTPUT);
  pinMode(OUTPUT_TAIL_JEMPUT, OUTPUT);
  pinMode(OUTPUT_TAIL_TINGGAL, OUTPUT);
  pinMode(OUTPUT_CW_ATC, OUTPUT);
  // pinmode input
  pinMode(INPUT_LOCK_ATC, INPUT_PULLUP);
  pinMode(INPUT_TAIL_DEPAN, INPUT_PULLUP);
  pinMode(INPUT_TAIL_BELAKANG, INPUT_PULLUP);
  pinMode(INPUT_PEDAL, INPUT_PULLUP);

  // buat semua menjadi low
  digitalWriteFast(digitalPinToPinName(OUTPUT_TAIL_JEMPUT), LOW);
  digitalWriteFast(digitalPinToPinName(OUTPUT_TAIL_TINGGAL), LOW);
  digitalWriteFast(digitalPinToPinName(OUTPUT_CW_ATC), LOW);
  digitalWriteFast(digitalPinToPinName(OUTPUT_LOCK_ATC), LOW);

  Serial.println("Connect: ");
  delay(3000);

#ifdef SCANNING_I2C
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

    // digitalWrite(PC13, HIGH); // turn the LED on (HIGH is the voltage level)
    // delay(1000);              // wait for a second
    // digitalWrite(PC13, LOW);  // turn the LED off by making the voltage LOW
    // delay(3000);              // wait for a second
#endif
}

void verbose_output()
{
  // untuk proxy dan lainnya
  Serial.print("T:");

  Serial.print(digitalReadFast(digitalPinToPinName(INPUT_LOCK_ATC)) ? "U" : "L");
  Serial.print(!digitalReadFast(digitalPinToPinName(INPUT_TAIL_DEPAN)) ? "D" : "");
  Serial.print(!digitalReadFast(digitalPinToPinName(INPUT_TAIL_BELAKANG)) ? "B" : "");
  Serial.print(!digitalReadFast(digitalPinToPinName(INPUT_PEDAL)) ? "P" : "");

  // if (nilai_input[0])
  //     Serial.print("F"); // umbrella didepan

// untuk pergerakan motor
#ifdef VERBOSE_MOTOR
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
#ifdef VERBOSE_TOOLS
  Serial.print("|P:");
  Serial.print((std::bitset<8>(value_i2c_atc_encoder)).to_string().c_str());
#endif

// untuk eroor
#ifdef VERBOSE_ERROR
  Serial.print("|E:");
  Serial.print("unknown");
#endif

// VERBOSE tulis kembali perintah pc
#ifdef VERBOSE_PERINTAH_PC
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

  while (Serial.available() > 0 && new_data == false)
  {
    rc = Serial.read();
    rc = toUpperCase(rc);

    if (rc != endMarker)
    {
      received_chars[ndx] = rc;
      ndx++;
      if (ndx >= num_chars)
        ndx = num_chars - 1;
    }
    else
    {
      received_chars[ndx] = '\0'; // Menandai akhir string
      ndx = 0;
      new_data = true;
    }
  }
}

void parsing_perintah_pc()
{
  char *strtokIndx;

  // Mendapatkan karakter pertama dari buffer
  strtokIndx = strtok(received_chars, ",");
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
    // jemput tail stock
  case '!':
    kirim_verbose = false;
    break; // jemput tail stock
  case '?':
    kirim_verbose = true;
    break;
    // jemput tail stock
  case 'J':
    digitalWriteFast(digitalPinToPinName(OUTPUT_TAIL_JEMPUT), HIGH);
    digitalWriteFast(digitalPinToPinName(OUTPUT_TAIL_TINGGAL), LOW);
    perintah_pc = 'J';
    break;
    // tinggal tail stock
  case 'T':
    digitalWriteFast(digitalPinToPinName(OUTPUT_TAIL_JEMPUT), LOW);
    digitalWriteFast(digitalPinToPinName(OUTPUT_TAIL_TINGGAL), HIGH);
    perintah_pc = 'T';
    break;
    // stop tail stock
  case 'S':
    digitalWriteFast(digitalPinToPinName(OUTPUT_TAIL_JEMPUT), LOW);
    digitalWriteFast(digitalPinToPinName(OUTPUT_TAIL_TINGGAL), LOW);
    perintah_pc = 'S';
    break;
    // perintah lock atc
  case 'L':
    digitalWriteFast(digitalPinToPinName(OUTPUT_LOCK_ATC), LOW);
    perintah_pc = 'L';
    break;
    // perintah unlock atc
  case 'U':
    digitalWriteFast(digitalPinToPinName(OUTPUT_LOCK_ATC), HIGH);
    perintah_pc = 'U';
    break;
    // atc bergerak cw
  case 'A':
    digitalWriteFast(digitalPinToPinName(OUTPUT_CW_ATC), HIGH);
    perintah_pc = 'A';
    gerak_motor = 'A';
    break;
    // b atau ccw tidak ada
  case 'B':
    perintah_pc = 'B';
    gerak_motor = 'B';
    break;
    // matikan pergerakan atc
  case 'C':
    digitalWriteFast(digitalPinToPinName(OUTPUT_CW_ATC), LOW);
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
    value_i2c_atc_encoder = Wire.read(); // receive a byte as character

  // Wire.requestFrom(i2c_add_2, 1); // request 6 bytes from slave device #8
  // while (Wire.available())
  //   value_i2c_io = Wire.read(); // receive a byte as character
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
  recvWithEndMarker();
  if (new_data == true)
  {
    parsing_perintah_pc();
    new_data = false;
  }
  if (kirim_verbose)
  {
    baca_sinyal_i2c();
    verbose_output();
  }

  // gerakkan_motor();
}